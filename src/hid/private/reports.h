#ifndef __HID_REPORTS_H__
#define __HID_REPORTS_H__

#include <stdint.h>

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

struct __packed LampAttributesRequestReport
{
    uint8_t ReportId;
    uint16_t LampId;
};

struct __packed LampArrayControlReport
{
    uint8_t ReportId;
    uint8_t AutonomousMode : 1;
};

#endif