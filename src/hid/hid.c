#include "private/descriptor.h"
#include "private/reports.h"

#include "hid/hid.h"
#include "error/error.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/drivers/usb/usb_dc.h>
#include <zephyr/drivers/usb/udc_buf.h>

LOG_MODULE_REGISTER(hid);

/**
 * @brief Check if the usb device is ready to send requests
 */
static volatile bool usb_is_ready = false;

/**
 * @brief Semaphore to indicate the USB interface is ready
 * @note Take this semaphore when {usb_is_ready} is false, this will block until
 * the interface is once again ready.
 */
static K_SEM_DEFINE(sem_usb_ready, 0, 1);

/**
 * @brief Semaphore to indicate the USB HID input transfer was successfull and a new transfer is possible
 * @note Take this semaphore after doing some operation to wait for the previous transfer to complete
 */
static K_SEM_DEFINE(sem_in_ready, 0, 1);

/**
 * @brief Semaphore to indicate the USB HID output transfer was successfull and a new transfer is possible
 * @note Take this semaphore after doing some operation to wait for the previous transfer to complete
 */
static K_SEM_DEFINE(sem_out_ready, 0, 1);

/**
 * @brief Mutex for exclusive access to the hid_device
 * @note Use this before any operation on the USB device
 */
static K_MUTEX_DEFINE(mtx_usb_device);

/**
 * @brief Report descriptor for the HID interface
 * @note Keep this variable in static memory, if not program will fail
 */
static uint8_t hid_report_desc[] = HID_LAMPARRAY_REPORT_DESCRIPTOR();

/**
 * @brief Callback when USB status changes, this functions gives {sem_usb_ready} on USB connection successfull
 *
 * @param cb_status Current USB status
 * @param params Parameter for bit-mask
 */
static inline void usb_status_isr(
    enum usb_dc_status_code cb_status,
    const uint8_t *params)
{
    switch (cb_status)
    {
    // USB connection failed
    case USB_DC_ERROR:
        application_panic(USB_DC_ERROR, 0);
        break;

    // USB interface is ready, signal threads
    case USB_DC_CONNECTED:
        [[fallthrough]];
    case USB_DC_CONFIGURED:
        // Signal the USB interface is ready
        usb_is_ready = true;
        k_sem_give(&sem_usb_ready);
        break;

    // USB interface is not ready
    case USB_DC_DISCONNECTED:
        [[fallthrough]];
    default:
        usb_is_ready = false;
        break;
    }
}

/**
 * @brief Callback when current USB HID Input operation has been completed
 */
static inline void hid_input_ready_isr(const struct device *_)
{
    ARG_UNUSED(_);
    // Give semaphore to indicate availability
    k_sem_give(&sem_in_ready);
}

/**
 * @brief Callback when current USB HID Output operation has been completed
 */
static inline void hid_output_ready_isr(const struct device *_)
{
    ARG_UNUSED(_);
    // Give semaphore to indicate availability
    k_sem_give(&sem_out_ready);
}

/**
 * @brief Show 'usb_setup_package' structure contents
 *
 * @param name Name of the calling function
 * @param setup 'usb_setup_package' structure
 */
static ALWAYS_INLINE void debug_print_usb_setup_packet(const char *name, struct usb_setup_packet *setup)
{
    LOG_INF("%s: UsbSetupPacket\n\tbmRequestType = (Recipient: %01X, Type: %01x, Direction: %01x)\n\tbRequest = %02X\n\twValue = %04X\n\twIndex = %04x\n\twLength = %04x",
            name,
            setup->RequestType.recipient,
            setup->RequestType.type,
            setup->RequestType.direction,
            setup->bRequest,
            setup->wValue,
            setup->wIndex,
            setup->wLength);
}

/**
 * @brief Set the attribute report object
 *
 * @param report Attribute report number
 * @param data Raw data from host
 * @param len Length of input data
 * @return 0 on success, negative errno on failure
 */
static int set_attribute_report(enum LampArrayReportType report, const uint8_t *data, size_t len)
{
    // Check arguments
    if (data == NULL || len == 0)
        return -EINVAL;

    switch (report)
    {
    case LAMPARRAY_CONTROL_REPORT:
        struct LampArrayControlReport report = {};
        memcpy(&report, data, len);
        LOG_INF("LampArrayControlReport { .AutonomousMode = %d }", report.AutonomousMode);
        break;

    default:
        // Operation not supported
        return -ENOTSUP;
    }

    return 0;
}

/**
 * @brief Send attribute reports to the host
 * @note Callback for 'hid_ops'
 */
static int hid_get_report(
    const struct device *dev,
    struct usb_setup_packet *setup,
    int32_t *len,
    uint8_t **data)
{
    debug_print_usb_setup_packet("GetReport", setup);

    // Check operation type
    if (setup->bRequest != HID_REPORT_REQUEST_KIND_GET_REPORT)
    {
        LOG_WRN("GetReport: [bRequest] (%d) ignored", setup->bRequest);
        return 0;
    }

    // Get the request data
    uint8_t request_buffer[2];
    sys_put_le16(setup->wValue, request_buffer);

    // Get values from request data
    uint8_t report_id = request_buffer[0];
    uint8_t report_type = request_buffer[1];

    // Check if requesting feature
    if (report_type != HID_REPORT_TYPE_FEATURE && report_type != HID_REPORT_TYPE_INPUT)
    {
        LOG_ERR("GetReport: HidReportType [wValueH] not supported (%d)", report_type);
        return -ENOTSUP;
    }

    // Check which report is being solicited
    switch (report_id)
    {
    case LAMPARRAY_ATTRIBUTES_REPORT:
        /* Return LampArrayAttributes */
        const struct LampArrayAttributesReport attributes_report = {
            .ReportId = LAMPARRAY_ATTRIBUTES_REPORT,
            .LampCount = LAMPARRAY_NUMBER_LED,
            .BoundingBoxWidthInMicrometers = 70,
            .BoundingBoxHeightInMicrometers = 55,
            .BoundingBoxDepthInMicrometers = 1,
            .LampArrayKind = LAMPARRAY_KIND_PERIPHERAL,
            .MinUpdateIntervalInMicroseconds = CONFIG_MIN_UPDATE_TIME,
        };

        // Structure doesn't fit HOST frame length
        if (sizeof(attributes_report) > setup->wLength)
        {
            LOG_ERR("GetReport: (%d) wont fit in (%d) frame",
                    sizeof(attributes_report), setup->wLength);
            return -EOVERFLOW;
        }

        // Copy data
        memcpy(*data, &attributes_report, sizeof(attributes_report));
        *len = sizeof(attributes_report);

        return 0;

    default:
        LOG_ERR("GetReport: bad index (%d)", report_id);
        return -ENOTSUP;
    }
}

/**
 * @brief Set attribute report data given from host
 * @note Callback for 'hid_ops'
 */
static int hid_set_report(
    const struct device *dev,
    struct usb_setup_packet *setup,
    int32_t *len,
    uint8_t **data)
{
    int err = 0;
    debug_print_usb_setup_packet("SetReport", setup);

    // Check operation type
    if (setup->bRequest != HID_REPORT_REQUEST_KIND_SET_REPORT)
    {
        LOG_WRN("SetReport: [bRequest] (%d) ignored", setup->bRequest);
        return 0;
    }

    // Get the request data
    uint8_t request_buffer[2];
    sys_put_le16(setup->wValue, request_buffer);

    // Get values from request data
    uint8_t report_id = request_buffer[0];
    uint8_t report_type = request_buffer[1];

    // Check which report is being set
    switch (report_type)
    {
    case HID_REPORT_TYPE_FEATURE:
        // Set the attribute report
        if ((err = set_attribute_report(
                 (enum LampArrayReportType)report_id,
                 *data,
                 setup->wLength)) < 0)
        {
            LOG_ERR("SetReport: failed to set attribute reports (errno=%d)", err);
        }
        break;

    default:
        LOG_ERR("SetReport: bad index (%d)", report_type);
        return -ENOTSUP;
    }

    // No return data
    return err;
}

/**
 * @brief Callbacks for USB HID Class status change.
 */
static const struct hid_ops hid_callback_ops = {
    // Host is ready to accept outgoing data
    .int_in_ready = hid_input_ready_isr,
    // Host is ready to accept incoming data
    .int_out_ready = hid_output_ready_isr,
    /* HID report data */
    .get_report = hid_get_report,
    .set_report = hid_set_report,
};

/**
 * @brief Initialize the USB interface and the USB HID Class.
 *
 * @param self Pointer to an hid_activity_t instance
 * @param hid_device HID device from 'get_device_binding'.
 * @return 0 on success, negative errno code on fail.
 */
static int hid_interface_init(const struct device *hid_device)
{
    int err = 0;

    // Check arguments
    if (hid_device == NULL)
    {
        return -EINVAL;
    }

    // Get lock on USB device
    k_mutex_lock(&mtx_usb_device, K_FOREVER);

    // Log the hexdump of the 'hid_report_desc'
    LOG_HEXDUMP_INF(
        hid_report_desc, sizeof(hid_report_desc),
        "USB HID Report Descriptor");

    // Register the HID device
    usb_hid_register_device(
        hid_device,
        // The device descriptor should be kept on memory during the entire program execution
        hid_report_desc,
        sizeof(hid_report_desc),
        &hid_callback_ops);

    // Log the hexdump of the 'hid_report_desc'
    LOG_INF("(Registered) HID Report Descriptor");

    // Initialize the HID Class
    if ((err = usb_hid_init(hid_device)) < 0)
    {
        return err;
    }

    // Release the USB device
    k_mutex_unlock(&mtx_usb_device);

    // Enable the USB interface
    if ((err = usb_enable(usb_status_isr)) < 0)
    {
        return err;
    }

    // Return sucess
    LOG_INF("(Ready) USB Interface");
    return 0;
}

void hid_lamparray_main(void *_p1, void *_p2, void *_p3)
{
    int err = 0;

    // Notify compiler about unused arguments
    ARG_UNUSED(_p1);
    ARG_UNUSED(_p2);
    ARG_UNUSED(_p3);

    // Get the HID device binding
    const struct device *hid_device = device_get_binding("HID_0");
    if (hid_device == NULL)
    {
        application_panic(ERROR_REASON_HARDWARE, 0);
    }

    // Initialize the interface
    if ((err = hid_interface_init(hid_device)) < 0)
    {
        application_panic(ERROR_REASON_HARDWARE, err);
    }

    while (true)
    {
        k_yield();
    }
}
