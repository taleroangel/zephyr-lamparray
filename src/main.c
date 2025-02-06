#include <app_version.h>
#include <error/error.h>
#include <hid/hid.h>

#include <stdint.h>
#include <stdlib.h>

#include <zephyr/fatal.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// Register error handler
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
    panic_handler(reason, esf);
}

// Register threads
/* Declare the LampArray HID thread handler */
K_THREAD_DEFINE(
    hid_lamparray_thread,
    HID_LAMPARRAY_THREAD_STACK_SIZE,
    hid_lamparray_main,
    /* arguments */
    NULL, NULL, NULL,
    HID_LAMPARRAY_THREAD_PRIORITY,
    K_ESSENTIAL, 0);