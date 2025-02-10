/**
 * @file controller.h
 * @author Angel Talero (angelgotalero@outlook.com)
 * @brief LED controller related symbols
 * @version 0.1
 * @date 2025-02-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __PIXEL_CONTROLLER_H__
#define __PIXEL_CONTROLLER_H__

#include "pixel.h"

#include <stdlib.h>
#include <errno.h>

#include <zephyr/device.h>

/**
 * @brief pixel_controller_main() thread stack size
 */
#define PIXEL_CONTROLLER_THREAD_STACK_SIZE 1024

/**
 * @brief pixel_controller_main() priority
 */
#define PIXEL_CONTROLLER_THREAD_PRIORITY 2

/**
 * @brief Max size of queued request to the pixel controller
 */
#define PIXEL_CONTROLLER_QUEUE_MAX 10

/**
 * @brief LED strip device
 */
#define STRIP_DT_NODE DT_ALIAS(strip0)

// Get the number of leds from the DeviceTree node
#if DT_NODE_HAS_PROP(STRIP_DT_NODE, chain_length)

/**
 * @brief Number of LEDs for the 'strip0'
 */
#define _STRIP_0_NUMBER_OF_LEDS DT_PROP(STRIP_DT_NODE, chain_length)

/**
 * @brief Total number of LEDs given by DeviceTree node
 */
#define PIXEL_NUMBER_OF_LEDS _STRIP_0_NUMBER_OF_LEDS

#else
#error Unable to determine length of LED strip
#endif

/**
 * @brief Keep the controller status
 */
struct pixel_controller_status_t
{
    bool autonomous;
};

/**
 * @brief Entry point (thread) for pixel controller
 */
void pixel_controller_main(void *_p1, void *_p2, void *_p3);

#endif