#ifndef __PIXEL_REQUEST_H__
#define __PIXEL_REQUEST_H__

#include <stdlib.h>

#include "pixel.h"
#include "controller.h"

/**
 * @brief Type of the request being sent to the controller
 *
 */
enum pixel_controller_request_type
{
    PIXEL_CONTROLLER_REQUEST_NONE,
    PIXEL_CONTROLLER_REQUEST_UPDATE,
    PIXEL_CONTROLLER_REQUEST_OPERATION,
};

/**
 * @brief Send a request to the pixel controller
 * @note This is what should be sent to the controller
 */
struct pixel_controller_request
{
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
        struct pixel_update_data update[PIXEL_NUMBER_OF_LEDS];

        /**
         * @brief Change controller's operation type
         */
        enum pixel_controller_operation_mode operation;

    } request;
};

#endif