#ifndef __HID_REPORTS_H__
#define __HID_REPORTS_H__

#include <stdint.h>

/**
 * @brief Type of LampArray device in LampArrayAttributesReport
 *
 */
enum LampArrayKind
{
    LAMPARRAY_KIND_KEYBOARD = 0x01,
    LAMPARRAY_KIND_MOUSE = 0x02,
    LAMPARRAY_KIND_GAME_CONTROLLER = 0x03,
    LAMPARRAY_KIND_PERIPHERAL = 0x04,
    LAMPARRAY_KIND_SCENE = 0x05,
    LAMPARRAY_KIND_NOTIFICATION = 0x06,
    LAMPARRAY_KIND_CHASSIS = 0x07,
    LAMPARRAY_KIND_WEARABLE = 0x08,
    LAMPARRAY_KIND_FURNITURE = 0x09,
    LAMPARRAY_KIND_ART = 0x0A,
};

/**
 * @brief Specify the purpose of a single Lamp in LampAttributesResponseReport
 * @note Flags are permitted to be combined.
 */
enum LampArrayPurposes
{
    LAMPARRAY_PURPOSE_CONTROL = 0x01,
    LAMPARRAY_PURPOSE_ACCENT = 0x02,
    LAMPARRAY_PURPOSE_BRANDING = 0x04,
    LAMPARRAY_PURPOSE_STATUS = 0x08,
    LAMPARRAY_PURPOSE_ILLUMINATION = 0x10,
    LAMPARRAY_PURPOSE_PRESENTATION = 0x20,
};

/**
 * @brief Report #1
 */
struct __packed LampArrayAttributesReport
{
    uint8_t ReportId;
    uint16_t LampCount;
    uint32_t BoundingBoxWidthInMicrometers;
    uint32_t BoundingBoxHeightInMicrometers;
    uint32_t BoundingBoxDepthInMicrometers;
    uint32_t LampArrayKind;
    uint32_t MinUpdateIntervalInMicroseconds;
};

/**
 * @brief Report #2
 */
struct __packed LampAttributesRequestReport
{
    uint8_t ReportId;
    uint16_t LampId;
};

/**
 * @brief Report #3
 */
struct __packed LampAttributesResponseReport
{
    uint8_t ReportId;
    uint16_t LampId;
    uint32_t PositionXInMicrometers;
    uint32_t PositionYInMicrometers;
    uint32_t PositionZInMicrometers;
    uint32_t LampPurposes;
    uint32_t UpdateLatencyInMicroseconds;
    uint8_t RedLevelCount;
    uint8_t GreenLevelCount;
    uint8_t BlueLevelCount;
    uint8_t IntensityLevelCount;
    uint8_t IsProgrammable;
    uint8_t InputBinding;
};

/**
 * @brief Report #5
 */
struct __packed LampRangeUpdateRecord
{
    uint8_t ReportId;
    uint8_t LampUpdateFlags;
    uint16_t LampIdStart;
    uint16_t LampIdEnd;
    uint8_t RedUpdateChannel;
    uint8_t GreenUpdateChannel;
    uint8_t BlueUpdateChannel;
    uint8_t IntensityUpdateChannel;
};

/**
 * @brief HID report #6
 */
struct __packed LampArrayControlReport
{
    uint8_t ReportId;
    uint8_t AutonomousMode : 1;
};

#endif