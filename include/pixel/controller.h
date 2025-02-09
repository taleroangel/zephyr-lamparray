#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "pixel.h"

#include <stdlib.h>
#include <errno.h>

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

#endif