#ifndef __PIXEL_REQUEST_H__
#define __PIXEL_REQUEST_H__

#include <stdlib.h>

#include "pixel.h"
#include "controller.h"

/**
 * @brief Send an update request to the controller on many leds
 * @note Do not create directly, use functions instead (malloc used)
 */
struct pixel_multiupdate_request
{
    struct pixel_update_data data[PIXEL_NUMBER_OF_LEDS];
    size_t size;
};

/**
 * @brief Send an update request to the controller on a range of leds
 */
struct pixel_rangeupdate_request
{
    pixel_id_t from;
    pixel_id_t to;
    struct pixel_update_data values;
};

/**
 * @brief Type of the request being sent to the controller
 *
 */
enum pixel_controller_request_type
{
    PIXEL_CONTROLLER_REQUEST_NONE,
    PIXEL_CONTROLLER_REQUEST_MULTIUPDATE,
    PIXEL_CONTROLLER_REQUEST_RANGEUPDATE,
    PIXEL_CONTROLLER_REQUEST_AUTONOMOUS,
};

/**
 * @brief Send a request to the pixel controller
 * @note This is what should be sent to the controller
 */
struct pixel_controller_request
{
    /**
     * @brief Number of updated properties, this is required so that senders can
     * calculate time of delay
     */
    uint8_t n_updates;

    /**
     * @brief Specifies the type contained in the 'union'
     */
    enum pixel_controller_request_type type;

    /**
     * @brief Holds the request content (acording to the request type)
     */
    union
    {
        /**
         * @brief Update many pixels at once
         */
        struct pixel_multiupdate_request multiupdate;

        /**
         * @brief Update a range of pixels
         */
        struct pixel_rangeupdate_request rangeupdate;

        /**
         * @brief Enable or disable the autonomous mode
         */
        bool autonomous;

    } request;
};

#endif