/**
 * @file error.h
 * @author Angel Talero (angelgotalero@outlook.com)
 * @brief Error handling codes and functions
 * @version 0.1
 * @date 2025-02-09
 *
 * @copyright Copyright (c) 2025
 *
 */

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
    ERROR_REASON_UNKNOWN = 0x00,
    ERROR_REASON_EARLY_TERMINATION = 0x01,
    ERROR_REASON_OOM = 0x02,
    ERROR_REASON_HARDWARE = 0x03,
    ERROR_REASON_USB = 0x04,
    ERROR_REASON_ZBUS = 0x05,
    ERROR_REASON_BAD_INSTRUCTION = 0x06
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