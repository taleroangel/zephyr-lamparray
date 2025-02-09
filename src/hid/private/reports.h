/**
 * @file reports.h
 * @author Angel Talero (angelgotalero@outlook.com)
 * @brief Structs for HID reports on LampArray, USB data from the Report Descriptor
 * is directly mapped to structures on this file
 * @note Naming is in CamelCase as opposed to snake_case like the rest of the project
 * because this is what the USB convention is. "When in Rome, do as Romans do"
 * @version 0.1
 * @date 2025-02-09
 *
 * @copyright Copyright (c) 2025
 *
 */

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
 * @brief Report #4 (First Half)
 * @note Report #4 is tricky because it comes in parts, make sure to join it in a proper way
 */
struct __packed LampMultiUpdateReport_Begin
{
    uint8_t ReportId;
    uint64_t LampCount;
    uint8_t LampUpdateFlags;
};

/**
 * @brief Comes after LampMultiUpdateReport_Begin, repeats itself
 * for every lamp in LampCount
 */
struct __packed LampMultiUpdateReport_LampId
{
    uint16_t LampId;
};

/**
 * @brief Comes after all the LampMultiUpdateReport_LampId elements,
 * repeats itself for every lamp in LampCount
 */
struct __packed LampMultiUpdateReport_UpdateChannels
{
    uint8_t RedUpdateChannel;
    uint8_t GreenUpdateChannel;
    uint8_t BlueUpdateChannel;
    uint8_t IntensityUpdateChannel;
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