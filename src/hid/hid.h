/**
 * @file hid.h
 * @author Angel Talero (angelgotalero@outlook.com)
 * @brief USB HID related functionality
 * @version 0.1
 * @date 2024-12-15
 *
 * @copyright Copyright (c) 2024
 */

#ifndef __HID_H__
#define __HID_H__

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/drivers/usb/usb_dc.h>
#include <zephyr/drivers/usb/udc_buf.h>

/**
 * @brief hid_activity_main() thread stack size
 */
#define HID_LAMPARRAY_THREAD_STACK_SIZE 1024

/**
 * @brief hid_activity_main() priority
 */
#define HID_LAMPARRAY_THREAD_PRIORITY 1

/**
 * @brief Timeout for HID failure
 */
#define HID_LAMPARRAY_TIMEOUT K_SECONDS(30)

/**
 * @brief This thread handles LampArray hid
 *
 */
extern void hid_lamparray_main(void *, void *, void *);

#endif /* __HID_H__ */