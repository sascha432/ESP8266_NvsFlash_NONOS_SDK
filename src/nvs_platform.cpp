/**
  Author: sascha_lammers@gmx.de
*/

#include <Arduino.h>
#include <interrupts.h>
#include "nvs_platform.hpp"

namespace nvs {

    #if NVS_FLASH_NO_INTERRUPT_LOCK

        volatile bool Lock::_locked = false;

        Lock::Lock()
        {
            ets_intr_lock();
            if (!_locked) {
                _locked = true;
                ets_intr_unlock();
                return;
            }
            ets_intr_unlock();
            ::printf_P(PSTR("nvs function called while locked\n"));
            panic();
        }

        Lock::~Lock()
        {
            ets_intr_lock();
            _locked = false;
            ets_intr_unlock();
        }

    #else

        Lock::Lock()
        {
            ets_intr_lock();
        }

        Lock::~Lock()
        {
            ets_intr_unlock();
        }

    #endif
}

extern "C" void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ::printf_P(PSTR("%s:%u: %s: "), file, line, function);
    ::printf_P(PSTR("%x: %s\n"), rc, expression);
    panic();
}
