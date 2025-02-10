#include <app_version.h>

#include <error/error.h>
#include <pixel/request.h>
#include <pixel/controller.h>
#include <hid/hid.h>

#include <stdint.h>
#include <stdlib.h>

#include <zephyr/fatal.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

/* Register error handler */
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
    panic_handler(reason, esf);
}

// Define channel for sending requests to the pixel controller
ZBUS_CHAN_DEFINE(
    pixel_controller_zbus_channel,
    struct pixel_controller_request,
    NULL,
    NULL,
    ZBUS_OBSERVERS(pixel_controller_zbus_observer),
    ZBUS_MSG_INIT(.type = PIXEL_CONTROLLER_REQUEST_NONE));

/* Declare the Pixel controller thread handler */
K_THREAD_DEFINE(
    pixel_controller_thread,
    PIXEL_CONTROLLER_THREAD_STACK_SIZE,
    pixel_controller_main,
    /* arguments */
    NULL, NULL, NULL,
    PIXEL_CONTROLLER_THREAD_PRIORITY,
    K_ESSENTIAL, 0);

/* Declare the LampArray HID thread handler */
K_THREAD_DEFINE(
    hid_lamparray_thread,
    HID_LAMPARRAY_THREAD_STACK_SIZE,
    hid_lamparray_main,
    /* arguments */
    NULL, NULL, NULL,
    HID_LAMPARRAY_THREAD_PRIORITY,
    0, 0);