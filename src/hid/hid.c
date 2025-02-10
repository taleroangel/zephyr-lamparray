#include "private/descriptor.h"
#include "private/reports.h"

#include "hid/hid.h"
#include "error/error.h"
#include "pixel/request.h"

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
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(hid, CONFIG_HID_MODULE_LOG_LEVEL);

/**
 * @brief zbus channel for communicating with pixel controller
 */
ZBUS_CHAN_DECLARE(pixel_controller_zbus_channel);

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

    // USB interface is ready
    case USB_DC_CONNECTED:
        [[fallthrough]];
    case USB_DC_CONFIGURED:
        break;

    // USB interface is not ready
    case USB_DC_DISCONNECTED:
        [[fallthrough]];
    default:
        // TODO: Enable autonomous mode
        break;
    }
}

#ifdef CONFIG_DEBUG
/**
 * @brief Show 'usb_setup_package' structure contents
 *
 * @param name Name of the calling function
 * @param setup 'usb_setup_package' structure
 */
static ALWAYS_INLINE void debug_print_usb_setup_packet(const char *name, struct usb_setup_packet *setup)
{
    LOG_DBG("%s: UsbSetupPacket\n\tbmRequestType = (Recipient: %01X, Type: %01x, Direction: %01x)"
            "\n\tbRequest = %02X"
            "\n\twValue = %04X"
            "\n\twIndex = %04x"
            "\n\twLength = %04x",
            name,
            setup->RequestType.recipient,
            setup->RequestType.type,
            setup->RequestType.direction,
            setup->bRequest,
            setup->wValue,
            setup->wIndex,
            setup->wLength);
}
#endif

/**
 * @brief Counter for the requested LampId
 * This is used by both Get and Set reports
 */
static uint16_t current_lamp_id = 0;

/**
 * @brief Helper function for 'get_attribute_report', writes data to target HID
 *
 * @param target Target data structure from HID host
 * @param target_len  Target data length from HID host
 * @param data Data to copy into target
 * @param len Length of the data to copy
 * @param max_len Max data accepted by host
 * @return int 0 on Success, -ERRNO otherwise
 */
static int hid_report_write_host(
    uint8_t **target,
    size_t *target_len,
    void *data,
    size_t len,
    size_t max_len)
{
    // Guard for parameters
    if (target == NULL || target_len == NULL || data == NULL)
        return -ENOTSUP;

    // Structure doesn't fit HOST frame length
    if (len > max_len)
    {
        LOG_ERR("get_attribute_report: (%d) wont fit in (%d) frame", len, max_len);
        return -EOVERFLOW;
    }

    // Copy data to HID device
    memcpy(*target, data, len);
    *target_len = len;

    // Return no error;
    return 0;
}

/**
 * @brief Current pixel values before being send to the controller
 */
static struct pixel_update_data pixel_data[LAMPARRAY_NUMBER_LED];

/**
 * @brief Get the attribute report object for sending to the HID host
 *
 * @param type Report to be send
 * @param data Pointer to data to be set
 * @param len Pointer to length of data transmitted
 * @param max_len Maximum length allowed
 * @return int 0 if success, negative errno if failed
 */
static int get_attribute_report(
    enum LampArrayReportType type,
    uint8_t **data,
    size_t *len,
    size_t max_len)
{
    // Check arguments
    if (data == NULL || len == NULL)
        return -EINVAL;

    // Check which report is being solicited
    switch (type)
    {
    case LAMPARRAY_ATTRIBUTES_REPORT:
        LOG_DBG("(LampArrayAttributesReport) sent");
        const struct LampArrayAttributesReport attributes_report = {
            .ReportId = LAMPARRAY_ATTRIBUTES_REPORT,
            .LampCount = LAMPARRAY_NUMBER_LED,
            .BoundingBoxWidthInMicrometers = LAMP_BOUNDING_BOX_WH,
            .BoundingBoxHeightInMicrometers = LAMP_BOUNDING_BOX_WH,
            .BoundingBoxDepthInMicrometers = LAMP_BOUNDING_BOX_DEPTH,
            .LampArrayKind = LAMPARRAY_KIND_PERIPHERAL,
            .MinUpdateIntervalInMicroseconds = CONFIG_MIN_UPDATE_TIME,
        };
        // Write data to host
        hid_report_write_host(
            data, len,
            (void *)&attributes_report,
            sizeof(attributes_report), max_len);
        break;

    case LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT:
        LOG_DBG("(LampAttributesResponseReport) sent");
        const struct LampAttributesResponseReport attributes_response_report = {
            .ReportId = LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT,
            .LampId = current_lamp_id,
            .PositionXInMicrometers = (LAMP_BOUNDING_BOX_WH * current_lamp_id),
            .PositionYInMicrometers = (LAMP_BOUNDING_BOX_WH * current_lamp_id),
            .PositionZInMicrometers = (LAMP_BOUNDING_BOX_DEPTH * current_lamp_id),
            .LampPurposes = LAMPARRAY_PURPOSE_ACCENT,
            .UpdateLatencyInMicroseconds = CONFIG_MIN_UPDATE_TIME,
            .RedLevelCount = pixel_data[current_lamp_id].red,
            .GreenLevelCount = pixel_data[current_lamp_id].green,
            .BlueLevelCount = pixel_data[current_lamp_id].blue,
            .IntensityLevelCount = pixel_data[current_lamp_id].intensity,
            .IsProgrammable = 0xFF,
            .InputBinding = 0x00,
        };
        // Write data to host
        hid_report_write_host(
            data, len,
            (void *)&attributes_response_report,
            sizeof(attributes_response_report), max_len);

        // Important! LampArray specifies that this should increment on each call
        // Carefull with overflowing this counter, this should reset at max number of LEDs
        current_lamp_id = (current_lamp_id + 1) % LAMPARRAY_NUMBER_LED;

        break;

    default:
        LOG_ERR("get_attribute_report: No report for (%d)", type);
        return -ENOTSUP;
    }

    return 0;
}

/**
 * @brief Set the attribute report object
 *
 * @param report Attribute report number
 * @param data Raw data from host
 * @param len Length of input data
 * @return 0 on success, negative errno on failure
 */
static int set_attribute_report(
    enum LampArrayReportType report,
    const uint8_t *data,
    size_t len)
{
    // Check arguments
    if (data == NULL || len == 0)
        return -EINVAL;

    // Request being send to Pixel Controller
    struct pixel_controller_request pixctrl_request = {
        .type = PIXEL_CONTROLLER_REQUEST_NONE,
    };

    // When true, send 'pixel_data' to the controller
    static bool lamp_update_complete = false;

    // Errno code
    int err = 0;

    // Note: Breaking in this loop means sending the request
    switch (report)
    {
    case LAMPARRAY_ATTRIBUTES_REQUEST_REPORT:
        // Retrieve the report from host
        struct LampAttributesRequestReport attributes_request_report = {};
        memcpy(&attributes_request_report, data, len);
        LOG_DBG("LampAttributesRequestReport { .LampId = %d }", attributes_request_report.LampId);

        // Set the LampID for subsequent calls
        current_lamp_id = attributes_request_report.LampId;

        // Return instead of break!
        return err;

    case LAMPARRAY_MULTIUPDATE_REPORT:
        // Special case here, Report #4 handles variable length
        size_t copied_bytes = 0;

        // Copy header
        struct LampMultiUpdateReport_Begin multiupdate_begin = {};
        memcpy(&multiupdate_begin, data, sizeof(multiupdate_begin));
        copied_bytes += sizeof(multiupdate_begin);

        // Check size
        if (multiupdate_begin.LampCount > LAMPARRAY_NUMBER_LED)
        {
            LOG_ERR("set_attribute_report: buffer overflow (%lld)", multiupdate_begin.LampCount);
            return -EINVAL;
        }

        // Check if update flag is present
        lamp_update_complete = (bool)(multiupdate_begin.LampUpdateFlags == 0x01);

        // This struct will store data sent by the host
        struct update_data_t
        {
            uint16_t id;
            struct pixel_update_data color;
        };

        // Allocate array for (multiupdate_begin.LampCount) items
        // Only first (multiupdate_begin.LampCount) positions will be used
        // No use of malloc because... embedded systems :)
        struct update_data_t update_data[PIXEL_NUMBER_OF_LEDS];
        memset(update_data, 0, sizeof(update_data));

        // Get the LampId records first
        for (size_t i = 0; i < multiupdate_begin.LampCount; i++)
        {
            // Read data from host
            struct LampMultiUpdateReport_LampId lamp_id = {};
            memcpy(&lamp_id, (data + copied_bytes), sizeof(lamp_id));
            copied_bytes += sizeof(lamp_id);

            // Set ids
            update_data[i].id = lamp_id.LampId;
        }

        // Get the update channel data at last
        for (size_t i = 0; i < multiupdate_begin.LampCount; i++)
        {
            // Read data from host
            struct LampMultiUpdateReport_UpdateChannels update_channels = {};
            memcpy(&update_channels, (data + copied_bytes), sizeof(update_channels));
            copied_bytes += sizeof(update_channels);

            // Set color data
            update_data[i].color.red = update_channels.RedUpdateChannel;
            update_data[i].color.green = update_channels.GreenUpdateChannel;
            update_data[i].color.blue = update_channels.BlueUpdateChannel;
            update_data[i].color.intensity = update_channels.IntensityUpdateChannel;
        }

        // Check that data matches
        if (copied_bytes != len)
        {
            LOG_ERR("set_attribute_report: data mismatch (len=%d, recv=%d)", len, copied_bytes);
            return -EINVAL;
        }

        // Copy data to internal structure
        for (size_t i = 0; i < multiupdate_begin.LampCount; i++)
            pixel_data[update_data[i].id] = update_data[i].color;

        break;

    case LAMPARRAY_RANGE_UPDATE_REPORT:
        struct LampRangeUpdateRecord range_update_report = {};
        memcpy(&range_update_report, data, len);
        LOG_DBG("LampRangeUpdateRecord { "
                "\n\t.LampUpdateFlags = %d, "
                "\n\t.LampIdStart = %d, "
                "\n\t.LampIdEnd = %d, "
                "\n\t.RedUpdateChannel = %d, "
                "\n\t.GreenUpdateChannel = %d, "
                "\n\t.BlueUpdateChannel = %d, "
                "\n\t.IntensityUpdateChannel = %d }",
                range_update_report.LampUpdateFlags,
                range_update_report.LampIdStart,
                range_update_report.LampIdEnd,
                range_update_report.RedUpdateChannel,
                range_update_report.GreenUpdateChannel,
                range_update_report.BlueUpdateChannel,
                range_update_report.IntensityUpdateChannel);

        // Check if update flag is present
        lamp_update_complete = (bool)(range_update_report.LampUpdateFlags == 0x01);

        // Set pixel data within range
        // Beware: update range end is inclusive
        for (size_t i = range_update_report.LampIdStart; i <= range_update_report.LampIdEnd; i++)
        {
            pixel_data[i].red = range_update_report.RedUpdateChannel;
            pixel_data[i].green = range_update_report.GreenUpdateChannel;
            pixel_data[i].blue = range_update_report.BlueUpdateChannel;
            pixel_data[i].intensity = range_update_report.IntensityUpdateChannel;
        }
        break;

    case LAMPARRAY_CONTROL_REPORT:
        // Get the header
        struct LampArrayControlReport control_report = {};
        memcpy(&control_report, data, len);
        LOG_DBG("LampArrayControlReport { .AutonomousMode = %d }", control_report.AutonomousMode);

        // Check the requested operation mode
        enum pixel_controller_operation_mode opmode =
            (control_report.AutonomousMode == 1)
                ? PIXEL_CONTROLLER_OPERATION_MODE_AUTONOMOUS
                : PIXEL_CONTROLLER_OPERATION_MODE_SLAVE;

        // Set request properties
        pixctrl_request.type = PIXEL_CONTROLLER_REQUEST_OPERATION;
        pixctrl_request.request.operation = opmode;

        // Send request to controller
        if ((err = zbus_chan_pub(
                 &pixel_controller_zbus_channel,
                 &pixctrl_request, K_USEC(CONFIG_MIN_UPDATE_TIME))) < 0)
        {
            LOG_ERR("set_attributes_report: zbus error (errno=%d)", err);
            return err;
        }

        // This breaks, meaning that latest LampMultiUpdateReports or LampRangeUpdateReports
        // will take effect if LampUpdateComplete was true.
        break;

    default:
        // Operation not supported
        LOG_WRN("set_attribute_report: (%d) not supported", report);

        // Return instead of break!
        return -ENOTSUP;
    }

    // Check if no update request is required
    if (!lamp_update_complete)
        return err;

    // Set properties of the request
    pixctrl_request.type = PIXEL_CONTROLLER_REQUEST_UPDATE;
    memcpy(pixctrl_request.request.update, pixel_data, sizeof(pixel_data));

    // Send the request to pixctrl
    if ((err = zbus_chan_pub(
             &pixel_controller_zbus_channel,
             &pixctrl_request,
             // Update time of each LED
             K_USEC(CONFIG_MIN_UPDATE_TIME * LAMPARRAY_NUMBER_LED))) < 0)
    {
        LOG_ERR("set_attributes_report: zbus error (errno=%d)", err);
        return err;
    }

    LOG_INF("set_attribute_report: Requested update to pixctrl");
    return err;
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
#ifdef CONFIG_DEBUG
    debug_print_usb_setup_packet("GetReport", setup);
#endif

    // Check operation type
    int err = 0;
    if (setup->bRequest != HID_REPORT_REQUEST_KIND_GET_REPORT)
    {
        LOG_WRN("hid_get_Report: [bRequest] (%d) ignored", setup->bRequest);
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
        LOG_ERR("hid_get_report: HidReportType [wValueH] not supported (%d)", report_type);
        return -ENOTSUP;
    }

    // Get the report
    switch (report_type)
    {
    case HID_REPORT_TYPE_FEATURE:
        // Get the report being solicited
        if ((err = get_attribute_report(
                 (enum LampArrayReportType)report_id,
                 data, len, setup->wLength)) < 0)
        {
            LOG_ERR("hid_get_report: failed to get attribute reports (errno=%d)", err);
            return err;
        }
        break;

    default:
        LOG_ERR("hid_get_report: bad index (%d)", report_type);
        return -ENOTSUP;
    }

    // No return data (err = 0 on success)
    return err;
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
#ifdef CONFIG_DEBUG
    debug_print_usb_setup_packet("SetReport", setup);
#endif

    // Check operation type
    int err = 0;
    if (setup->bRequest != HID_REPORT_REQUEST_KIND_SET_REPORT)
    {
        LOG_WRN("hid_set_report: [bRequest] (%d) ignored", setup->bRequest);
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
            LOG_ERR("hid_set_report: failed to set attribute reports (errno=%d)", err);
        }
        break;

    default:
        LOG_ERR("hid_set_report: bad index (%d)", report_type);
        return -ENOTSUP;
    }

    // No return data (err = 0 on success)
    return err;
}

/**
 * @brief Callbacks for USB HID Class status change.
 */
static const struct hid_ops hid_callback_ops = {
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

    // Log the hexdump of the 'hid_report_desc'
    LOG_HEXDUMP_DBG(
        hid_report_desc, sizeof(hid_report_desc),
        "HID Report Descriptor");

    // Register the HID device
    usb_hid_register_device(
        hid_device,
        // The device descriptor should be kept on memory during the entire program execution
        hid_report_desc,
        sizeof(hid_report_desc),
        &hid_callback_ops);

    // Log the hexdump of the 'hid_report_desc'
    LOG_INF("hid_interface_init: device registered");

    // Initialize the HID Class
    if ((err = usb_hid_init(hid_device)) < 0)
    {
        return err;
    }

    // Enable the USB interface
    if ((err = usb_enable(usb_status_isr)) < 0)
    {
        return err;
    }

    // Set initial pixel data
    memset(pixel_data, 0xFF, sizeof(pixel_data));

    // Return sucess
    LOG_INF("hid_interface_init: interface ready");
    return 0;
}

void hid_lamparray_main(void *_p1, void *_p2, void *_p3)
{
    // Notify compiler about unused arguments
    ARG_UNUSED(_p1);
    ARG_UNUSED(_p2);
    ARG_UNUSED(_p3);

    int err = 0;

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
}
