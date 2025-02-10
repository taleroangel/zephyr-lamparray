#include "error/error.h"
#include "pixel/request.h"
#include "pixel/controller.h"

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(pixel, CONFIG_PIXEL_MODULE_LOG_LEVEL);

/**
 * @brief Strip 0 device (from DeviceTree)
 */
static const struct device *const strip_0 = DEVICE_DT_GET(STRIP_DT_NODE);

/**
 * @brief Scale 255 values with intensity
 *
 * @param color Original
 * @param intensity Target intensity
 * @return uint8_t Color with intensity applied
 */
static uint8_t color_scale_intensity(uint8_t color, uint8_t intensity)
{
    float weight = ((float)intensity / 255.0f);
    return (uint8_t)CLAMP((int)((float)color * weight), 0, 255);
}

/**
 * @brief Cast 'struct pixel_update_data' to 'struct led_rgb' with intensities applied
 *
 * @param from Incoming RGBI data
 * @return struct led_rgb Outgoing RGB data
 */
static struct led_rgb cast_to_rgb(const struct pixel_update_data *from)
{
    struct led_rgb to;
    to.r = color_scale_intensity(from->red, from->intensity);
    to.g = color_scale_intensity(from->green, from->intensity);
    to.b = color_scale_intensity(from->blue, from->intensity);

    LOG_DBG("[%d](%02X, %02X, %02X)", from->id, to.r, to.g, to.b);
    return to;
}

/**
 * @brief Handle incoming controller requests
 */
ZBUS_SUBSCRIBER_DEFINE(pixel_controller_zbus_observer, PIXEL_CONTROLLER_QUEUE_MAX);

// Thread entrypoint
void pixel_controller_main(void *_p1, void *_p2, void *_p3)
{
    // Notify compiler about ununsed arguments
    ARG_UNUSED(_p1);
    ARG_UNUSED(_p2);
    ARG_UNUSED(_p3);

    // Initialize device
    if (!device_is_ready(strip_0))
        application_panic(ERROR_REASON_HARDWARE, 0);

    LOG_INF("pixel_controller_main: ready");

    // Structure with the LED data
    struct led_rgb pixel_data[PIXEL_NUMBER_OF_LEDS];
    memset(pixel_data, 0, sizeof(pixel_data));

    int err = 0;
    const struct zbus_channel *channel;

    // Wait indefinitely for messages in the zbus
    while ((err = zbus_sub_wait(&pixel_controller_zbus_observer, &channel, K_FOREVER)) == 0)
    {
        LOG_DBG("zbus: request arrival");

        // Get the request
        const struct pixel_controller_request *instruction = zbus_chan_const_msg(channel);

        switch (instruction->type)
        {
        case PIXEL_CONTROLLER_REQUEST_NONE:
            LOG_DBG("pixel_controller: PIXEL_CONTROLLER_REQUEST_NONE");
            break;

        case PIXEL_CONTROLLER_REQUEST_MULTIUPDATE:
            LOG_DBG("pixel_controller: PIXEL_CONTROLLER_REQUEST_MULTIUPDATE(%d)", instruction->n_updates);
            for (size_t i = 0; i < instruction->request.multiupdate.size; i++)
            {
                // Get update request
                const struct pixel_update_data *pixel_update = &instruction->request.multiupdate.data[i];
                // Apply color
                pixel_data[pixel_update->id] = cast_to_rgb(pixel_update);
            }
            break;

        case PIXEL_CONTROLLER_REQUEST_RANGEUPDATE:
            LOG_DBG("pixel_controller: PIXEL_CONTROLLER_REQUEST_RANGEUPDATE(%d)", instruction->n_updates);
            // Update every pixel from
            for (size_t i = instruction->request.rangeupdate.from;
                 i < instruction->request.rangeupdate.to; i++)
            {
                // Apply color
                pixel_data[i] = cast_to_rgb(&instruction->request.rangeupdate.values);
            }
            break;

        case PIXEL_CONTROLLER_REQUEST_AUTONOMOUS:
            LOG_DBG("pixel_controller: PIXEL_CONTROLLER_REQUEST_AUTONOMOUS(%d)", instruction->n_updates);
            // TODO: Set autonomous mode
            break;

        default:
            /* Bad instruction */
            application_panic(ERROR_REASON_BAD_INSTRUCTION, 0);
            break;
        }

        // Update pixel data
        led_strip_update_rgb(strip_0, pixel_data, PIXEL_NUMBER_OF_LEDS);
    }

    // Error on the zbus
    application_panic(ERROR_REASON_ZBUS, err);
}