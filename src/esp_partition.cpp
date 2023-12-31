/**
  Author: sascha_lammers@gmx.de
*/

#include "nvs_common.h"
#include "esp_partition.h"
#include <string.h>

/*

custom implementation that supports 2 NVS partitions only! An NVS partition requires at least 3 sectors, 6 or more are recommended for wear leveling.

more information can be found here: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html

NVS partition 1 must be initialized before use ('nvs_flash_init', at least 3 sectors are required)
NVS partition 2 must be initialized before use (0 sectors can be used if not required)

to release the RAM, use 'nvs_flash_deinit'

eagle.ld example

// nvs       @0x405AB000 (32KB, 32768)
// nvs2      @0x405B3000 (160KB, 163840)

PROVIDE ( _NVS_start = 0x405B3000 );
PROVIDE ( _NVS_end = 0x405DA000 );
PROVIDE ( _NVS2_start = 0x405B3000 );
PROVIDE ( _NVS2_end = 0x405DA000 );


*/

SECTION_EXTERN_UINT32(NVS);
#if NVS_PARTITIONS == 2
    SECTION_EXTERN_UINT32(NVS2)
#endif

#define NVS_PART_LABEL_1 "nvs"
#define NVS_PART_LABEL_2 "nvs2"

static const esp_partition_t nvs_partitions[2] = {
    { ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, SECTION_START_ADDR(NVS), SECTION_CALC_SIZE(NVS), NVS_PART_LABEL_1, false }
    #if NVS_PARTITIONS == 2
        , { ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, SECTION_START_ADDR(NVS2), SECTION_CALC_SIZE(NVS2), NVS_PART_LABEL_2, false }
    #endif
};

extern "C" const esp_partition_t* esp_partition_find_first(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* label)
{
    ESP_LOGD(TAG, "type=%u subtype=%u label=%s", type, subtype, label ? label : "NULL");
    if (type != ESP_PARTITION_TYPE_DATA && (subtype != ESP_PARTITION_SUBTYPE_DATA_NVS && subtype != ESP_PARTITION_SUBTYPE_ANY)) {
        return nullptr;
    }
    if (label) {
        for(const auto &part: nvs_partitions) {
            if (!strcmp(label, part.label)) {
                return &part;
            }
        }
        return nullptr;
    }
    return &nvs_partitions[0];
}

extern "C" esp_partition_iterator_t esp_partition_find(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* label)
{
    ESP_LOGD(TAG, "type=%u subtype=%u label=%s", type, subtype, label ? label : "NULL");
    if (type != ESP_PARTITION_TYPE_DATA && (subtype != ESP_PARTITION_SUBTYPE_DATA_NVS && subtype != ESP_PARTITION_SUBTYPE_ANY)) {
        return nullptr;
    }
    if (label) {
        for(const auto part: nvs_partitions) {
            if (!strcmp(label, part.label)) {
                return new esp_partition_iterator_opaque_(&part);
            }
        }
        return nullptr;
    }
    #if NVS_PARTITIONS == 2
        auto iterator = new esp_partition_iterator_opaque_(&nvs_partitions[0], &nvs_partitions[1]);
    #else
        auto iterator = new esp_partition_iterator_opaque_(&nvs_partitions[0]);
    #endif
    return iterator;
}

extern "C" void esp_partition_iterator_release(esp_partition_iterator_t iterator)
{
    ESP_LOGD(TAG, "iterator=%p", iterator);
    if (iterator) {
        delete iterator;
    }
}

extern "C" esp_partition_iterator_t esp_partition_next(esp_partition_iterator_t iterator)
{
    ESP_LOGD(TAG, "iterator=%p part=%p next=%p", iterator, iterator ? iterator->partition : nullptr, iterator ? iterator->next : nullptr);
    if (iterator) {
        if (iterator->next) {
            iterator->partition = iterator->next;
            iterator->next = nullptr;
            return iterator;
        }
        else {
            iterator->partition = nullptr;
        }
    }
    return nullptr;
}

extern "C" const esp_partition_t* esp_partition_get(esp_partition_iterator_t iterator)
{
    ESP_LOGD(TAG, "iterator=%p part=%p next=%p", iterator, iterator ? iterator->partition : nullptr, iterator ? iterator->next : nullptr);
    if (!iterator || !iterator->partition) {
        return nullptr;
    }
    return iterator->partition;
}

// check bounds and write absolute start address into "start_address"
static esp_err_t esp_partition_check_bounds(const esp_partition_t* partition, size_t offset, size_t size, uint32_t &start_address)
{
    start_address = partition->address + offset;
    uint32_t end = partition->address + partition->size;
    // ESP_LOGD(TAG, "label=%s addr=%x size=%u ofs=%u len=%u range=%u-%u e1=%d e2=%d", partition->label, partition->address, partition->size, offset, size, start_address, end, (start_address >= end), (start_address + size > end));
    if (start_address >= end) {
        return ESP_ERR_INVALID_ARG;
    }
    if (start_address + size > end) {
        return ESP_ERR_INVALID_SIZE;
    }
    return ESP_OK;
}

extern "C" esp_err_t esp_partition_read(const esp_partition_t* partition, size_t src_offset, void *dst, size_t size)
{
    // ESP_LOGD(TAG, "label=%s src_ofs=%u size=%u", partition->label, src_offset, size);
    esp_err_t err;
    uint32_t start;
    if ((err = esp_partition_check_bounds(partition, src_offset, size, start)) != ESP_OK) {
        return err;
    }
    return ESP.flashRead(start, reinterpret_cast<uint8_t *>(dst), size) ? ESP_OK : ESP_ERR_FLASH_BASE;
}

extern "C" esp_err_t esp_partition_write(const esp_partition_t* partition, size_t dst_offset, const void *src, size_t size)
{
    ESP_LOGD(TAG, "label=%s src_ofs=%u size=%u", partition->label, dst_offset, size);
    esp_err_t err;
    uint32_t start;
    if ((err = esp_partition_check_bounds(partition, dst_offset, size, start)) != ESP_OK) {
        return err;
    }
    return ESP.flashWrite(start, reinterpret_cast<const uint8_t *>(src), size) ? ESP_OK : ESP_ERR_FLASH_BASE;
}

extern "C" esp_err_t esp_partition_erase_range(const esp_partition_t* partition, uint32_t start_addr, uint32_t size)
{
    ESP_LOGD(TAG, "label=%s src_ofs=%u size=%u", partition->label, start_addr, size);
    esp_err_t err;
    uint32_t start;
    if ((err = esp_partition_check_bounds(partition, start_addr, size, start)) != ESP_OK) {
        return err;
    }
    if ((start % SPI_FLASH_SEC_SIZE) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((size % SPI_FLASH_SEC_SIZE) != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    start /= SPI_FLASH_SEC_SIZE;
    size /= SPI_FLASH_SEC_SIZE;
    while(size--) {
        if (!ESP.flashEraseSector(start++)) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}
