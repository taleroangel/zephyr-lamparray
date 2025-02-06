#include "error.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log.h>

#include <zephyr/sys/reboot.h>

LOG_MODULE_REGISTER(panic);

inline void application_panic(error_reason_t reason, int error_code)
{
    LOG_ERR("Application issued panic with code: (%d), errno: (%d)", reason, error_code);
    k_oops();
}

ALWAYS_INLINE void panic_handler(unsigned int reason, const struct arch_esf *esf)
{
    LOG_PANIC();
    sys_reboot(SYS_REBOOT_COLD);
}
