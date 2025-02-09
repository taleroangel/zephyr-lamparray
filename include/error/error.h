#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdint.h>
#include <zephyr/fatal.h>

/**
 * @brief All the code errors
 *
 */
typedef enum __ERROR_REASONS_T
{
    ERROR_REASON_UNKNOWN,
    ERROR_REASON_EARLY_TERMINATION,
    ERROR_REASON_OOM,
    ERROR_REASON_HARDWARE,
    ERROR_REASON_USB,
} error_reason_t;

/**
 * @brief Enter a non-recoverable state and halt code execution
 * @param reason Root cause for the error
 * @param errno Specific error cause (errno)
 */
void application_panic(error_reason_t reason, int errno);

/**
 * @brief Handler for void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
 */
void panic_handler(unsigned int reason, const struct arch_esf *esf);

#endif