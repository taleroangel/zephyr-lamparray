#include "hid/hid.h"
#include "hid/report.h"
#include "error/error.h"

#include <stdlib.h>
#include <stdint.h>
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
static K_SEM_DEFINE(sem_transfer_complete, 0, 1);

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
 * @brief Callback when current USB HID Input has been completed
 */
static inline void hid_input_ready_isr(const struct device *_)
{
    ARG_UNUSED(_);
    // Give semaphore to indicate availability
    k_sem_give(&sem_transfer_complete);
}

static int hid_get_report(
    const struct device *dev,
    struct usb_setup_packet *setup,
    int32_t *len,
    uint8_t **data)
{
    uint8_t request[2];
    // USB always uses LittleEndian
    sys_put_le16(setup->wValue, request);

    // Print the request
    LOG_INF("%02X, %02X", request[0], request[1]);

    return 0;
}

static int hid_set_report(
    const struct device *dev,
    struct usb_setup_packet *setup,
    int32_t *len,
    uint8_t **data)
{
}

/**
 * @brief Callbacks for USB HID Class status change.
 */
static const struct hid_ops hid_callback_ops = {
    /* This one is important, is it called when USB host is ready to accept input */
    .int_in_ready = hid_input_ready_isr,
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

    // Busy wait...
    while (true)
    {
        k_busy_wait(1000);
    }
}
