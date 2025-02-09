#ifndef __PIXEL_H__
#define __PIXEL_H__

#include <stdint.h>

/**
 * @brief Index for a specific pixel in array
 */
typedef uint16_t pixel_id_t;

/**
 * @brief 8-bit color representation
 */
typedef uint8_t color_t;

/**
 * @brief Update request for a specific pixel
 */
struct pixel_update_data
{
    pixel_id_t id ;
    color_t red;
    color_t green;
    color_t blue;
    color_t intensity;
};

#endif