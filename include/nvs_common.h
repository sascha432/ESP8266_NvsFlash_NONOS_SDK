/**
  Author: sascha_lammers@gmx.de
*/

#pragma once

#ifndef __NVS_COMMON_H_INCLUDED
#define __NVS_COMMON_H_INCLUDED 1

// lock interrupts during operations
// if nvs functions are only called inside the loop() function and not inside interrupts or timers, locking is not required
#ifndef NVS_FLASH_NO_INTERRUPT_LOCK
#    define NVS_FLASH_NO_INTERRUPT_LOCK 0
#endif

#include "private/nvs_common_pvt.h"

#endif
