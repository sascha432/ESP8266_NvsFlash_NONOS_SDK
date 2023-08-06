/**
  Author: sascha_lammers@gmx.de
*/

#pragma once

#include <Esp.h>

#define SECTION_FLASH_START_ADDRESS 0x40200000U
#define SECTION_FLASH_END_ADDRESS   0x40300000U // 1MB, 256 sectors or 1048576 byte are addressable
#define SECTION_CALC_SIZE(name)     ((((uint32_t)&_##name##_end) - ((uint32_t)&_##name##_start)) + FLASH_SECTOR_SIZE)
#define SECTION_START_ADDR(name)    (((uint32_t)&_##name##_start) - SECTION_FLASH_START_ADDRESS)
