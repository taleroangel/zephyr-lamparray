/**
 * @file pixel.h
 * @author Angel Talero (angelgotalero@outlook.com)
 * @brief Pixel data definitions
 * @version 0.1
 * @date 2025-02-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */

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
    color_t red;
    color_t green;
    color_t blue;
    color_t intensity;
};

/**
 * @brief Pixel (LED) controller thread
 */
void pixel_controller_main(void *_p1, void *_p2, void *_p3);

#endif