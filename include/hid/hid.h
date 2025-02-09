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

#include <zephyr/kernel.h>

enum HidReportRequestKind
{
    HID_REPORT_REQUEST_KIND_GET_REPORT = 0x01,
    HID_REPORT_REQUEST_KIND_GET_IDLE = 0x02,
    HID_REPORT_REQUEST_KIND_GET_PROTOCOL = 0x03,
    HID_REPORT_REQUEST_KIND_SET_REPORT = 0x09,
    HID_REPORT_REQUEST_KIND_SET_IDLE = 0x0A,
    HID_REPORT_REQUEST_KIND_SET_PROTOCOL = 0x0B,
};

enum HidReportType
{
    HID_REPORT_TYPE_INPUT = 1,
    HID_REPORT_TYPE_OUTPUT = 2,
    HID_REPORT_TYPE_FEATURE = 3,
};

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