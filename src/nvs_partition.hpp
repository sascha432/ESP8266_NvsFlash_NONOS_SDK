// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ESP_PARTITION_HPP_
#define ESP_PARTITION_HPP_

#include "intrusive_list.h"
#include "partition.hpp"

#define ESP_ENCRYPT_BLOCK_SIZE 16

#define PART_NAME_MAX_SIZE              16   /*!< maximum length of partition name (excluding null terminator) */

namespace nvs {

/**
 * Implementation of Partition for NVS.
 *
 * It is implemented as an intrusive_list_node to easily store instances of it. NVSStorage and NVSPage take pointer
 * references of this class to abstract their partition operations.
 */
class NVSPartition : public Partition, public intrusive_list_node<NVSPartition> {
public:
    /**
     * Copy partition_name to mPartitionName and initialize mESPPartition.
     *
     * @param partition_name the name of the partition as in the partition table, must be non-NULL!
     * @param partition an already initialized partition structure
     */
    NVSPartition(const esp_partition_t* partition);

    /**
     * No need to de-initialize mESPPartition here, if you used esp_partition_find_first.
     * Otherwise, the user is responsible for de-initializing it.
     */
    virtual ~NVSPartition() { }

    const char *get_partition_name() override;

    /**
     * Look into \c esp_partition_read for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition API
     */
    esp_err_t read_raw(size_t src_offset, void* dst, size_t size) override;

    /**
     * Look into \c esp_partition_read for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_ARG if size isn't a multiple of ESP_ENCRYPT_BLOCK_SIZE
     *      - other error codes from the esp_partition API
     */
    esp_err_t read(size_t src_offset, void* dst, size_t size) override;

    /**
     * Look into \c esp_partition_write for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - error codes from the esp_partition API
     */
    esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) override;

    /**
     * Look into \c esp_partition_write for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_ARG if size isn't a multiple of ESP_ENCRYPT_BLOCK_SIZE
     *      - other error codes from the esp_partition API
     */
    esp_err_t write(size_t dst_offset, const void* src, size_t size) override;

    /**
     * Look into \c esp_partition_erase_range for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - error codes from the esp_partition API
     */
    esp_err_t erase_range(size_t dst_offset, size_t size) override;

    /**
     * @return the base address of the partition.
     */
    uint32_t get_address() override;

    /**
     * @return the size of the partition in bytes.
     */
    uint32_t get_size() override;

protected:
    const esp_partition_t* mESPPartition;
};

} // nvs

#endif // ESP_PARTITION_HPP_
