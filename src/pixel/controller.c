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
    return (struct led_rgb){
        .r = color_scale_intensity(from->red, from->intensity),
        .g = color_scale_intensity(from->green, from->intensity),
        .b = color_scale_intensity(from->blue, from->intensity)};
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

    // Errno code
    int err = 0;

    // Initialize device
    if (!device_is_ready(strip_0))
        application_panic(ERROR_REASON_HARDWARE, 0);

    // Current controller operation mode
    enum pixel_controller_operation_mode opmode =
        PIXEL_CONTROLLER_OPERATION_MODE_AUTONOMOUS;

    // Structure with the LED data
    struct led_rgb pixel_data[PIXEL_NUMBER_OF_LEDS];
    memset(pixel_data, 0, sizeof(pixel_data));

    // Channel for recieving requests
    const struct zbus_channel *channel;

    // Wait indefinitely for messages in the zbus
    while ((err = zbus_sub_wait(&pixel_controller_zbus_observer, &channel, K_FOREVER)) == 0)
    {
        // Get the request
        const struct pixel_controller_request *instruction = zbus_chan_const_msg(channel);
        LOG_DBG("Solving request (%d)", instruction->type);

        switch (instruction->type)
        {
        case PIXEL_CONTROLLER_REQUEST_NONE:
            break;

        case PIXEL_CONTROLLER_REQUEST_UPDATE:
            // Update every pixel from
            for (size_t i = 0; i < PIXEL_NUMBER_OF_LEDS; i++)
            {
                // Apply color
                pixel_data[i] = cast_to_rgb(&instruction->request.update[i]);
            }
            break;

        case PIXEL_CONTROLLER_REQUEST_OPERATION:
            // Set the new operation mode
            opmode = instruction->request.operation;
            LOG_INF("Operation mode (%d)", opmode);
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