/**
  Author: sascha_lammers@gmx.de
*/

#include <Arduino.h>
#include <interrupts.h>
#include "nvs_platform.hpp"

namespace nvs {

    // we cannot use a semaphore, otherwise any call from an interrupt while it is locked will fail

    Lock::Lock()
    {
        ets_intr_lock();
    }

    Lock::~Lock()
    {
        ets_intr_unlock();
    }
}

extern "C" void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ::printf_P(PSTR("%s:%u: %s: "), file, line, function);
    ::printf_P(PSTR("%x: %s\n"), rc, expression);
    panic();
}
