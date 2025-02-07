#ifndef __HID_REPORT_H__
#define __HID_REPORT_H__

#include <zephyr/usb/class/usb_hid.h>

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
// LampArrayKindChassis
#define __LAMPARRAY_KIND_CHASSIS_USAGE 0x07
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
 * @brief LampArrayKind parameter
 */
#define LAMPARRAY_KIND __LAMPARRAY_KIND_CHASSIS_USAGE

/**
 * @brief HID Report Descriptor for the device
 */
#define HID_LAMPARRAY_REPORT_DESCRIPTOR() {                                 \
                                                                            \
    /* LampArray HID Descriptor */                                          \
    HID_USAGE_PAGE(__LIGHTING_ILLUMINATION_PAGE),                           \
    HID_USAGE(__LAMPARRAY_USAGE),                                           \
                                                                            \
    HID_COLLECTION(HID_COLLECTION_APPLICATION),                             \
                                                                            \
    /* Report #1 */                                                         \
    HID_REPORT_ID(1),                                                       \
    HID_USAGE(__LAMPARRAY_ATTRIBUTES_REPORT_USAGE),                         \
    HID_COLLECTION(HID_COLLECTION_LOGICAL),                                 \
    HID_USAGE(__LAMPARRAY_LAMPCOUNT_USAGE),                                 \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0x00, 0x00), /* 65535 */                  \
    HID_REPORT_SIZE(16),                                                    \
    HID_REPORT_COUNT(1),                                                    \
    HID_FEATURE(__FEATURE_CST_VAR_ABS), /* Feature(Cnst, Var, Abs) */       \
    HID_USAGE(LAMPARRAY_KIND),                                              \
    HID_USAGE(__LAMPARRAY_MINUPDTINTUS_USAGE),                              \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0xFF, 0x7F), /* 2147483647 */             \
    HID_REPORT_SIZE(32),                                                    \
    HID_REPORT_COUNT(2),                                                    \
    HID_FEATURE(__FEATURE_CST_VAR_ABS), /* Feature(Cnst, Var, Abs) */       \
    HID_END_COLLECTION,                                                     \
                                                                            \
    /* Report #2 */                                                         \
    HID_REPORT_ID(2),                                                       \
    HID_USAGE(__LAMPARRAY_ATTRIBUTES_REQUEST_REPORT),                       \
    HID_COLLECTION(HID_COLLECTION_LOGICAL),                                 \
    HID_USAGE(__LAMPARRAY_LAMPID_USAGE),                                    \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0x00, 0x00), /* 65535 */                  \
    HID_REPORT_SIZE(16),                                                    \
    HID_REPORT_COUNT(1),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_END_COLLECTION,                                                     \
                                                                            \
    /* Report #3 */                                                         \
    HID_REPORT_ID(3),                                                       \
    HID_USAGE(__LAMPARRAY_ATTRIBUTES_RESPONSE_REPORT),                      \
    HID_COLLECTION(HID_COLLECTION_LOGICAL),                                 \
    HID_USAGE(__LAMPARRAY_LAMPID_USAGE),                                    \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0x00, 0x00), /* 65535 */                  \
    HID_REPORT_SIZE(16),                                                    \
    HID_REPORT_COUNT(1),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_USAGE(__LAMPARRAY_LAMPPURPOSES_USAGE),                              \
    HID_USAGE(__LAMPARRAY_UPDTLTCYUS_USAGE),                                \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0xFF, 0x7F), /* 2147483647 */             \
    HID_REPORT_SIZE(32),                                                    \
    HID_REPORT_COUNT(2),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_USAGE(__LAMPARRAY_RED_LEVELCOUNT_USAGE),                            \
    HID_USAGE(__LAMPARRAY_GREEN_LEVELCOUNT_USAGE),                          \
    HID_USAGE(__LAMPARRAY_BLUE_LEVELCOUNT_USAGE),                           \
    HID_USAGE(__LAMPARRAY_INTENSITY_LEVELCOUNT_USAGE),                      \
    HID_USAGE(__LAMPARRAY_IS_PROGRAMMABLE_USAGE),                           \
    HID_USAGE(__LAMPARRAY_INPUT_BINDING_USAGE),                             \
    HID_LOGICAL_MIN8(0x00),                                                 \
    HID_LOGICAL_MAX16(0xFF, 0x00),                                          \
    HID_REPORT_SIZE(8),                                                     \
    HID_REPORT_COUNT(6),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_END_COLLECTION,                                                     \
                                                                            \
    /* Report #4 */                                                         \
    HID_REPORT_ID(4),                                                       \
    HID_USAGE(__LAMPARRAY_MULTIUPDATE_REPORT_USAGE),                        \
    HID_COLLECTION(HID_COLLECTION_LOGICAL),                                 \
    HID_USAGE(__LAMPARRAY_LAMPCOUNT_USAGE),                                 \
    HID_USAGE(__LAMPARRAY_UPDATE_FLAGS_USAGE),                              \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX8(8),                                                    \
    HID_REPORT_SIZE(8),                                                     \
    HID_REPORT_COUNT(2),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_USAGE(__LAMPARRAY_LAMPID_USAGE),                                    \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0x00, 0x00), /* 65535 */                  \
    HID_REPORT_SIZE(16),                                                    \
    HID_REPORT_COUNT(1),                 /* Number of LEDs */               \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
                                                                            \
    /* LED Report Data */                                                   \
    HID_USAGE(__LAMPARRAY_RED_UPDATE_CHANNEL_USAGE),                        \
    HID_USAGE(__LAMPARRAY_GREEN_UPDATE_CHANNEL_USAGE),                      \
    HID_USAGE(__LAMPARRAY_BLUE_UPDATE_CHANNEL_USAGE),                       \
    HID_USAGE(__LAMPARRAY_INTENSITY_UPDATE_CHANNEL_USAGE),                  \
                                                                            \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX16(0xFF, 0),                                             \
    HID_REPORT_SIZE(8),                                                     \
    HID_REPORT_COUNT(4),                 /* Number of LEDs x 4(channels) */ \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_END_COLLECTION,                                                     \
                                                                            \
    /* Report #5 */                                                         \
    HID_REPORT_ID(5),                                                       \
    HID_USAGE(__LAMPARRAY_RANGE_UPDATE_REPORT_USAGE),                       \
    HID_COLLECTION(HID_COLLECTION_LOGICAL),                                 \
    HID_USAGE(__LAMPARRAY_UPDATE_FLAGS_USAGE),                              \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX8(8),                                                    \
    HID_REPORT_SIZE(8),                                                     \
    HID_REPORT_COUNT(1),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_USAGE(__LAMPARRAY_LAMPID_START_USAGE),                              \
    HID_USAGE(__LAMPARRAY_LAMPID_END_USAGE),                                \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX32(0xFF, 0xFF, 0x00, 0x00), /* 65535 */                  \
    HID_REPORT_SIZE(16),                                                    \
    HID_REPORT_COUNT(2),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_USAGE(__LAMPARRAY_RED_UPDATE_CHANNEL_USAGE),                        \
    HID_USAGE(__LAMPARRAY_GREEN_UPDATE_CHANNEL_USAGE),                      \
    HID_USAGE(__LAMPARRAY_BLUE_UPDATE_CHANNEL_USAGE),                       \
    HID_USAGE(__LAMPARRAY_INTENSITY_UPDATE_CHANNEL_USAGE),                  \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX16(0xFF, 0),                                             \
    HID_REPORT_SIZE(8),                                                     \
    HID_REPORT_COUNT(4),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_END_COLLECTION,                                                     \
                                                                            \
    /* Report #6 */                                                         \
    HID_REPORT_ID(6),                                                       \
    HID_USAGE(__LAMPARRAY_CONTROL_REPORT_USAGE),                            \
    HID_COLLECTION(HID_COLLECTION_LOGICAL),                                 \
    HID_USAGE(__LAMPARRAY_AUTONOMOUS_MODE_USAGE),                           \
    HID_LOGICAL_MIN8(0),                                                    \
    HID_LOGICAL_MAX8(1),                                                    \
    HID_REPORT_SIZE(8),                                                     \
    HID_REPORT_COUNT(1),                                                    \
    HID_FEATURE(__FEATURE_DATA_VAR_ABS), /* Feature(Data, Var, Abs) */      \
    HID_END_COLLECTION,                                                     \
                                                                            \
    HID_END_COLLECTION,                                                     \
}

#endif