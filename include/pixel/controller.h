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

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

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
 * @brief Send a update request to the controller on many leds
 * @note Do not create directly, use functions instead
 */
struct pixel_multiupdate_request
{
    struct pixel_update_data *data;
    size_t size;
};

/**
 * @brief Create a new 'pixel_multiupdate_request' object.
 * @note This function allocated memory with 'malloc', call 'dispose_pixel_multiupdate_request' to deallocate.
 *
 * @param self A (pointer to a) 'NULL' pointer, this is where the structure will be allocated
 * @param size Number of LEDs to allocated
 * @return int 0 on success, -ERRNO on failure
 */
int new_pixel_multiupdate_request(struct pixel_multiupdate_request **self, size_t size);

/**
 * @brief Destroy (and deallocate) a 'pixel_multiupdate_request' object.
 * @param self Pointer (to a pointer) to an existing 'pixel_multiupdate_request' in memory.
 * This pointer will be freed and automatically set to NULL
 */
void dispose_pixel_multiupdate_request(struct pixel_multiupdate_request **self);

/**
 * @brief Entry point (thread) for pixel controller
 */
void pixel_controller_main(void *_p1, void *_p2, void *_p3);

#endif