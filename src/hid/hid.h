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
 *
 */
#define HID_LAMPARRAY_THREAD_STACK_SIZE 1024

/**
 * @brief hid_activity_main() priority
 *
 */
#define HID_LAMPARRAY_THREAD_PRIORITY 1

/**
 * @brief Timeout for HID failure
 *
 */
#define HID_LAMPARRAY_TIMEOUT K_SECONDS(30)

/**
 * @brief HID Report Descriptor for the device
 */
#define HID_LAMPARRAY_REPORT_DESCRIPTOR() {                                                                                                     \
                                                                                                                                                \
    HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP) /* Use the 'Generic Desktop Page' */,                                                                 \
    HID_USAGE(HID_USAGE_GEN_DESKTOP_MOUSE), /* Use the 'Mouse' usage type */                                                                    \
                                                                                                                                                \
    HID_COLLECTION(HID_COLLECTION_APPLICATION), /* CA: A group of Main items that might be familiar to applications */                          \
                                                                                                                                                \
    HID_USAGE(HID_USAGE_GEN_DESKTOP_POINTER), /* Define a 'Pointer' item within the mouse */                                                    \
                                                                                                                                                \
    HID_COLLECTION(HID_COLLECTION_PHYSICAL), /* CP: Used for a set of data items that represent data points collected at one geometric point */ \
                                                                                                                                                \
    HID_USAGE_PAGE(HID_USAGE_GEN_BUTTON), /* Buttons */                                                                                         \
    HID_USAGE_MIN8(0),                                                                                                                          \
    HID_USAGE_MAX8(0),                                                                                                                          \
    HID_LOGICAL_MIN8(0),                                                                                                                        \
    HID_LOGICAL_MAX8(1),                                                                                                                        \
    HID_REPORT_SIZE(8),                                                                                                                         \
    HID_REPORT_COUNT(1),                                                                                                                        \
    HID_INPUT(1U),                                                                                                                              \
                                                                                                                                                \
    HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP), /* Describe the pointer's coordinate system X, Y */                                                  \
                                                                                                                                                \
    HID_USAGE(HID_USAGE_GEN_DESKTOP_X), /* A linear translation in the X direction (DV), from left to right. */                                 \
                                                                                                                                                \
    HID_USAGE(HID_USAGE_GEN_DESKTOP_Y), /* A linear translation in the Y direction (DV), from far to near. */                                   \
    HID_USAGE(HID_USAGE_GEN_DESKTOP_WHEEL),                                                                                                     \
                                                                                                                                                \
    HID_LOGICAL_MIN8(-127), /* Define maximum values for axes */                                                                                \
    HID_LOGICAL_MAX8(127),                                                                                                                      \
                                                                                                                                                \
    HID_REPORT_SIZE(8), /* Set report size of 1 byte per field for 2 fields */                                                                  \
    HID_REPORT_COUNT(3),                                                                                                                        \
    HID_INPUT(0b00000110U), /* (Data, Variable, Relative) */                                                                                    \
                                                                                                                                                \
    HID_END_COLLECTION, /* HID_COLLECTION(HID_COLLECTION_PHYSICAL) */                                                                           \
    HID_END_COLLECTION /* HID_COLLECTION(HID_COLLECTION_APPLICATION) */,                                                                        \
};

/**
 * @brief This thread handles LampArray hid
 *
 */
extern void hid_lamparray_main(void *, void *, void *);

#endif /* __HID_H__ */