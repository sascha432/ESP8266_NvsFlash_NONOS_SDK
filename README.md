# NVS Storage for ESP8266_NONOS_SDK

This is a port of the nvs_flash code of the ESP8266_RTOS_SDK

The nvs_flash provides a key value storage with wear leveling. A very good replacement for the EEPROM class that overwrites the same sector each time a single byte is changed and is limited to 4KB of data.

## Documentation

[https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html)

## Partitions

2 Partitions are supported and need to be defined in the eagle.ld file. The default names are `nvs` and `nvs2`. The names can be changed in `esp_partition.cpp`. If `NVS_PARTITIONS` is set to 1, the second partition is not used

## Second partition

The second partition is optional and `NVS_PARTITIONS` has to be set to 2. It needs to be initialized before use with `nvs_flash_init_partition`

## Modifications

The code is only slightly modified, but relies on a replacement of the `esp_partition_*` functions since the NONOS_SDK does not have a readable runtime partition table. The partitions are grabbed from the eagle.ld linking file

## Encryption

Encryption is not supported

## RAM usage

`nvs_flash_init()` allocates quite a lot RAM. To release about 5KB, use `nvs_flash_deinit()` if NVS is not required anymore. Same for `nvs_flash_init_partition()`. Initializing the partition again requires some time depending on th size (16ms for 32KB) and blocks interrupts. If none of the NVS functions are used in interrupt, the lock can be changed to a semaphore and won't block interrupt. (TODO add option to use a semaphore)

## Changelog

### 0.0.2

- Added example app

### 0.0.1

- Initial release
