#include "error/error.h"
#include "pixel/controller.h"

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/logging/log.h>
#include <zephyr/drivers/led_strip.h>

LOG_MODULE_REGISTER(pixel, CONFIG_PIXEL_MODULE_LOG_LEVEL);

int new_pixel_multiupdate_request(
    struct pixel_multiupdate_request **self,
    size_t size)
{
    // Check arguments
    if (*self != NULL)
        return -EINVAL;

    // Create the structure
    *self = (struct pixel_multiupdate_request *)malloc(sizeof(struct pixel_multiupdate_request));
    if (self == NULL)
        return -ECANCELED;

    // Allocate parts of the structure
    (*self)->data = (struct pixel_update_data *)malloc(size);
    (*self)->size = size;

    // Check allocation
    if ((*self)->data == NULL)
    {
        free(*self);
        return -ECANCELED;
    }

    return 0;
};

void dispose_pixel_multiupdate_request(
    struct pixel_multiupdate_request **self)
{
    // Deallocate internal struct
    free((*self)->data);

    // Deallocate object
    free(*self);

    // Set to null (for safety)
    *self = NULL;
}

/**
 * @brief Strip 0 device (from DeviceTree)
 */
static const struct device *const strip_0 = DEVICE_DT_GET(STRIP_DT_NODE);

void pixel_controller_main(void *_p1, void *_p2, void *_p3)
{
    // Notify compiler about ununsed arguments
    ARG_UNUSED(_p1);
    ARG_UNUSED(_p2);
    ARG_UNUSED(_p3);

    int err = 0;

    // Initialize device
    if (!device_is_ready(strip_0))
    {
        application_panic(ERROR_REASON_HARDWARE, 0);
    }

    // Structure with the LED data
    struct led_rgb pixel_data[PIXEL_NUMBER_OF_LEDS];
    memset(pixel_data, 0, sizeof(pixel_data));

    while (true)
    {
        // Update pixel data
        led_strip_update_rgb(strip_0, pixel_data, PIXEL_NUMBER_OF_LEDS);
        k_busy_wait(CONFIG_MIN_UPDATE_TIME);
    }
}