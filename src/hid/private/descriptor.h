#ifndef __HID_DESCRIPTOR_H__
#define __HID_DESCRIPTOR_H__

#include "pixel/controller.h"

#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/sys/util_macro.h>

// SV (Const, Var, Abs)
#define __FEATURE_CST_VAR_ABS 0x03
// DV (Data, Var, Abs)
#define __FEATURE_DATA_VAR_ABS 0x02
// Lighting and Illumination Page
#define __LIGHTING_ILLUMINATION_PAGE 0x59
// LampArray
#define __LAMPARRAY_USAGE 0x01
// LampArrayAttributesReport
#define __LAMPARRAY_ATTRIBUTES_REPORT_USAGE 0x02
// LampCount
#define __LAMPARRAY_LAMPCOUNT_USAGE 0x03
// BoundingBoxWidthInMicrometers
#define __LAMPARRAY_BOUNDING_BOX_WIDTH_USAGE 0x04
// BoundingBoxHeightInMicrometers
#define __LAMPARRAY_BOUNDING_BOX_HEIGHT_USAGE 0x05
// BoundingBoxDepthInMicrometers
#define __LAMPARRAY_BOUNDING_BOX_DEPTH_USAGE 0x06
// LampArrayKindChassis
#define __LAMPARRAY_KIND_USAGE 0x07
// MinUpdateIntervalInMicroseconds
#define __LAMPARRAY_MINUPDTINTUS_USAGE 0x08
// LampAttributesRequestReport
#define __LAMPARRAY_ATTRIBUTES_REQUEST_REPORT 0x20
// LampId
#define __LAMPARRAY_LAMPID_USAGE 0x21
// LampIdStart
#define __LAMPARRAY_LAMPID_START_USAGE 0x61
// LampIdEnd
#define __LAMPARRAY_LAMPID_END_USAGE 0x62
// LampAttributesResponseReport
#define __LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT 0x22
// PositionXInMicrometers
#define __LAMPARRAY_POISITION_X_USAGE 0x23
// PositionYInMicrometers
#define __LAMPARRAY_POISITION_Y_USAGE 0x24
// PositionZInMicrometers
#define __LAMPARRAY_POISITION_Z_USAGE 0x25
// LampPurposes
#define __LAMPARRAY_LAMPPURPOSES_USAGE 0x26
// UpdateLatencyInMicroseconds
#define __LAMPARRAY_UPDTLTCYUS_USAGE 0x27
// RedLevelCount
#define __LAMPARRAY_RED_LEVELCOUNT_USAGE 0x28
// GreenLevelCount
#define __LAMPARRAY_GREEN_LEVELCOUNT_USAGE 0x29
// BlueLevelCount
#define __LAMPARRAY_BLUE_LEVELCOUNT_USAGE 0x2A
// IntensityLevelCount
#define __LAMPARRAY_INTENSITY_LEVELCOUNT_USAGE 0x2B
// IsProgrammable
#define __LAMPARRAY_IS_PROGRAMMABLE_USAGE 0x2C
// InputBinding
#define __LAMPARRAY_INPUT_BINDING_USAGE 0x2D
// LampMultiUpdateReport
#define __LAMPARRAY_MULTIUPDATE_REPORT_USAGE 0x50
// LampUpdateFlags
#define __LAMPARRAY_UPDATE_FLAGS_USAGE 0x55
// RedUpdateChannel
#define __LAMPARRAY_RED_UPDATE_CHANNEL_USAGE 0x51
// GreenUpdateChannel
#define __LAMPARRAY_GREEN_UPDATE_CHANNEL_USAGE 0x52
// BlueUpdateChannel
#define __LAMPARRAY_BLUE_UPDATE_CHANNEL_USAGE 0x53
// IntensityUpdateChannel
#define __LAMPARRAY_INTENSITY_UPDATE_CHANNEL_USAGE 0x54
// LampRangeUpdateReport
#define __LAMPARRAY_RANGE_UPDATE_REPORT_USAGE 0x60
// LampArrayControlReport
#define __LAMPARRAY_CONTROL_REPORT_USAGE 0x70
// AutonomousMode
#define __LAMPARRAY_AUTONOMOUS_MODE_USAGE 0x71

/**
 * @brief Number of LEDs in LampArray
 * @note This variable is set from PIXEL_NUMBER_OF_LEDS
 */
#define LAMPARRAY_NUMBER_LED PIXEL_NUMBER_OF_LEDS

/**
 * @brief Beginning of the LampArray HID report descriptor
 */
#define HID_LAMPARRAY_REPORT_BEGIN                \
    HID_USAGE_PAGE(__LIGHTING_ILLUMINATION_PAGE), \
        HID_USAGE(__LAMPARRAY_USAGE),             \
        HID_COLLECTION(HID_COLLECTION_APPLICATION)

/**
 * @brief All of the 6 report types ordered
 */
enum LampArrayReportType
{
    LAMPARRAY_ATTRIBUTES_REPORT = 1,
    LAMPARRAY_ATTRIBUTES_REQUEST_REPORT = 2,
    LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT = 3,
    LAMPARRAY_MULTIUPDATE_REPORT = 4,
    LAMPARRAY_RANGE_UPDATE_REPORT = 5,
    LAMPARRAY_CONTROL_REPORT = 6,
};

/**
 * @brief LampArray HID first report descriptor
 * @note LampArrayAttributesReport
 */
#define HID_LAMPARRAY_REPORT_1                                            \
    HID_REPORT_ID(LAMPARRAY_ATTRIBUTES_REPORT),                           \
        HID_USAGE(__LAMPARRAY_ATTRIBUTES_REPORT_USAGE),                   \
        HID_COLLECTION(HID_COLLECTION_LOGICAL),                           \
        HID_USAGE(__LAMPARRAY_LAMPCOUNT_USAGE),                           \
        HID_LOGICAL_MIN8(0),                                              \
        HID_LOGICAL_MAX16(0xFF, 0xFF), /* 65535 */                        \
        HID_REPORT_SIZE(16),                                              \
        HID_REPORT_COUNT(1),                                              \
        HID_FEATURE(__FEATURE_CST_VAR_ABS), /* Feature(Cnst, Var, Abs) */ \
        HID_USAGE(__LAMPARRAY_BOUNDING_BOX_WIDTH_USAGE),                  \
        HID_USAGE(__LAMPARRAY_BOUNDING_BOX_HEIGHT_USAGE),                 \
        HID_USAGE(__LAMPARRAY_BOUNDING_BOX_DEPTH_USAGE),                  \
        HID_USAGE(__LAMPARRAY_KIND_USAGE),                                \
        HID_USAGE(__LAMPARRAY_MINUPDTINTUS_USAGE),                        \
        HID_LOGICAL_MIN8(0),                                              \
        HID_LOGICAL_MAX32(0xFF, 0xFF, 0xFF, 0x7F), /* 2147483647 */       \
        HID_REPORT_SIZE(32),                                              \
        HID_REPORT_COUNT(5),                                              \
        HID_FEATURE(__FEATURE_CST_VAR_ABS), /* Feature(Cnst, Var, Abs) */ \
        HID_END_COLLECTION

/**
 * @brief LampArray HID second report descriptor
 * @note LampAttributesRequestReport
 */
#define HID_LAMPARRAY_REPORT_2                                             \
    HID_REPORT_ID(LAMPARRAY_ATTRIBUTES_REQUEST_REPORT),                    \
        HID_USAGE(__LAMPARRAY_ATTRIBUTES_REQUEST_REPORT),                  \
        HID_COLLECTION(HID_COLLECTION_LOGICAL),                            \
        HID_USAGE(__LAMPARRAY_LAMPID_USAGE),                               \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX16(0xFF, 0xFF), /* 65535 */                         \
        HID_REPORT_SIZE(16),                                               \
        HID_REPORT_COUNT(1),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_END_COLLECTION

/**
 * @brief LampArray HID third report descriptor
 * @note LampAttributesResponseReport
 */
#define HID_LAMPARRAY_REPORT_3                                             \
    HID_REPORT_ID(LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT),                   \
        HID_USAGE(__LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT),                 \
        HID_COLLECTION(HID_COLLECTION_LOGICAL),                            \
        HID_USAGE(__LAMPARRAY_LAMPID_USAGE),                               \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX16(0xFF, 0xFF), /* 65535 */                         \
        HID_REPORT_SIZE(16),                                               \
        HID_REPORT_COUNT(1),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_USAGE(__LAMPARRAY_POISITION_X_USAGE),                          \
        HID_USAGE(__LAMPARRAY_POISITION_Y_USAGE),                          \
        HID_USAGE(__LAMPARRAY_POISITION_Z_USAGE),                          \
        HID_USAGE(__LAMPARRAY_LAMPPURPOSES_USAGE),                         \
        HID_USAGE(__LAMPARRAY_UPDTLTCYUS_USAGE),                           \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX32(0xFF, 0xFF, 0xFF, 0x7F), /* 2147483647 */        \
        HID_REPORT_SIZE(32),                                               \
        HID_REPORT_COUNT(5),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_USAGE(__LAMPARRAY_RED_LEVELCOUNT_USAGE),                       \
        HID_USAGE(__LAMPARRAY_GREEN_LEVELCOUNT_USAGE),                     \
        HID_USAGE(__LAMPARRAY_BLUE_LEVELCOUNT_USAGE),                      \
        HID_USAGE(__LAMPARRAY_INTENSITY_LEVELCOUNT_USAGE),                 \
        HID_USAGE(__LAMPARRAY_IS_PROGRAMMABLE_USAGE),                      \
        HID_USAGE(__LAMPARRAY_INPUT_BINDING_USAGE),                        \
        HID_LOGICAL_MIN8(0x00),                                            \
        HID_LOGICAL_MAX8(0xFF),                                            \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(6),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_END_COLLECTION

/**
 * @brief LampArray HID fourth report descriptor (First half)
 * @note LampMultiUpdateReport: Define each of the LEDs
 */
#define HID_LAMPARRAY_REPORT_4_BEGIN                                       \
    HID_REPORT_ID(LAMPARRAY_MULTIUPDATE_REPORT),                           \
        HID_USAGE(__LAMPARRAY_MULTIUPDATE_REPORT_USAGE),                   \
        HID_COLLECTION(HID_COLLECTION_LOGICAL),                            \
        HID_USAGE(__LAMPARRAY_LAMPCOUNT_USAGE),                            \
        HID_USAGE(__LAMPARRAY_UPDATE_FLAGS_USAGE),                         \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX8(LAMPARRAY_NUMBER_LED),                            \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(2),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_USAGE(__LAMPARRAY_LAMPID_USAGE),                               \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX16(0xFF, 0xFF), /* 65535 */                         \
        HID_REPORT_SIZE(16),                                               \
        HID_REPORT_COUNT(LAMPARRAY_NUMBER_LED), /* Number of LEDs */       \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS)     /* Feature(Data, Var, Abs) */

/**
 * @brief LampArray HID fourth report descriptor (Middle part)
 * @note Should be present for each of the LEDs, put in a for loop
 */
#define HID_LAMPARRAY_REPORT_4_LED_DEFINITION(_, __)       \
    HID_USAGE(__LAMPARRAY_RED_UPDATE_CHANNEL_USAGE),       \
        HID_USAGE(__LAMPARRAY_GREEN_UPDATE_CHANNEL_USAGE), \
        HID_USAGE(__LAMPARRAY_BLUE_UPDATE_CHANNEL_USAGE),  \
        HID_USAGE(__LAMPARRAY_INTENSITY_UPDATE_CHANNEL_USAGE)

/**
 * @brief LampArray HID fourth report descriptor (Second half)
 * @note LampMultiUpdateReport: Define each of the LEDs
 */
#define HID_LAMPARRAY_REPORT_4_END                                                     \
    HID_LOGICAL_MIN8(0),                                                               \
        HID_LOGICAL_MAX8(0xFF),                                                        \
        HID_REPORT_SIZE(8),                                                            \
        HID_REPORT_COUNT(LAMPARRAY_NUMBER_LED * 4), /* Number of LEDs x 4(channels) */ \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS),        /* Feature(Data, Var, Abs) */      \
        HID_END_COLLECTION

/**
 * @brief LampArray HID fifth report descriptor
 * @note LampRangeUpdateReport
 */
#define HID_LAMPARRAY_REPORT_5                                             \
    HID_REPORT_ID(LAMPARRAY_RANGE_UPDATE_REPORT),                          \
        HID_USAGE(__LAMPARRAY_RANGE_UPDATE_REPORT_USAGE),                  \
        HID_COLLECTION(HID_COLLECTION_LOGICAL),                            \
        HID_USAGE(__LAMPARRAY_UPDATE_FLAGS_USAGE),                         \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX8(8),                                               \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(1),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_USAGE(__LAMPARRAY_LAMPID_START_USAGE),                         \
        HID_USAGE(__LAMPARRAY_LAMPID_END_USAGE),                           \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX16(0xFF, 0xFF), /* 65535 */                         \
        HID_REPORT_SIZE(16),                                               \
        HID_REPORT_COUNT(2),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_USAGE(__LAMPARRAY_RED_UPDATE_CHANNEL_USAGE),                   \
        HID_USAGE(__LAMPARRAY_GREEN_UPDATE_CHANNEL_USAGE),                 \
        HID_USAGE(__LAMPARRAY_BLUE_UPDATE_CHANNEL_USAGE),                  \
        HID_USAGE(__LAMPARRAY_INTENSITY_UPDATE_CHANNEL_USAGE),             \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX8(0xFF),                                            \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(4),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_END_COLLECTION

/**
 * @brief LampArray HID sixth report descriptor
 * @note LampArrayControlReport
 */
#define HID_LAMPARRAY_REPORT_6                                             \
    HID_REPORT_ID(LAMPARRAY_CONTROL_REPORT),                               \
        HID_USAGE(__LAMPARRAY_CONTROL_REPORT_USAGE),                       \
        HID_COLLECTION(HID_COLLECTION_LOGICAL),                            \
        HID_USAGE(__LAMPARRAY_AUTONOMOUS_MODE_USAGE),                      \
        HID_LOGICAL_MIN8(0),                                               \
        HID_LOGICAL_MAX8(1),                                               \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(1),                                               \
        HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */ \
        HID_END_COLLECTION

/**
 * @brief LampArray HID last report descriptor
 */
#define HID_LAMPARRAY_REPORT_END HID_END_COLLECTION

/**
 * @brief Compile-time report descriptor
 */
#define HID_LAMPARRAY_REPORT_DESCRIPTOR() {                                     \
    HID_LAMPARRAY_REPORT_BEGIN,                                                 \
    HID_LAMPARRAY_REPORT_1,                                                     \
    HID_LAMPARRAY_REPORT_2,                                                     \
    HID_LAMPARRAY_REPORT_3,                                                     \
    HID_LAMPARRAY_REPORT_4_BEGIN,                                               \
    LISTIFY(LAMPARRAY_NUMBER_LED, HID_LAMPARRAY_REPORT_4_LED_DEFINITION, (, )), \
    HID_LAMPARRAY_REPORT_4_END,                                                 \
    HID_LAMPARRAY_REPORT_5,                                                     \
    HID_LAMPARRAY_REPORT_6,                                                     \
    HID_LAMPARRAY_REPORT_END,                                                   \
}

#endif