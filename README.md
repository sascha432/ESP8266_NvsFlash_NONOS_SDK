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

`nvs_flash_init()` allocated quite a lot RAM. to release it, use `nvs_flash_deinit()` if NVS is not required anymore.

## Changelog

### 0.0.2

- New version

### 0.0.1

- Initial release