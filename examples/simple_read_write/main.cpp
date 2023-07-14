/**
  Author: sascha_lammers@gmx.de
*/

// this example initializes the nvs_flash default partition and opens a namespace,
// incrementing a simple counter 100 times before it closes and de-initializes the
// nvs flash. it also displays the heap usage before and after init, open, close and
// deinit

#include <Arduino.h>
#include <nvs.h>
#include <nvs_flash.h>

#define _STRINGIFY(...)                     ___STRINGIFY(__VA_ARGS__)
#define ___STRINGIFY(...)                   #__VA_ARGS__

#define ERROR_CHECK(func) \
    { \
        esp_err_t err; \
        if ((err = func) != ESP_OK) { \
            Serial.printf_P(PSTR(_STRINGIFY(func) " failed with err=%x\n"), err); \
            halt(); \
        } \
    }

#define NVS_MY_NAMESPACE "mynamespace"

nvs_handle_t nvs;

static void halt()
{
    for(;;) {
        delay(1000);
    }
}

static void print_free_heap()
{
    Serial.printf_P(PSTR("free heap %u\n"), ESP.getFreeHeap());    
}

void setup()
{
    Serial.begin(115200);
    Serial.println(F("starting in 5 seconds..."));
    delay(5000);

    Serial.printf_P(PSTR("opening nvs_flash namespace=%s\n"), NVS_MY_NAMESPACE);

    print_free_heap();

    // initialize default partition
    ERROR_CHECK(nvs_flash_init());

    print_free_heap();

    // open namespace in the default partition
    ERROR_CHECK(nvs_open(NVS_MY_NAMESPACE, nvs_open_mode_t::NVS_READWRITE, &nvs));

    print_free_heap();

    // get physical info about the default partition
    auto part = esp_partition_find_first(esp_partition_type_t::ESP_PARTITION_TYPE_DATA, esp_partition_subtype_t::ESP_PARTITION_SUBTYPE_DATA_NVS, NVS_DEFAULT_PART_NAME);

    // get statistics about the default partition
    nvs_stats_t stats;
    ERROR_CHECK(nvs_get_stats(NULL, &stats));

    Serial.printf_P(PSTR(
        "statistics:\n"
        "used entries    %d\n"
        "free entries    %d\n"
        "total entries   %d\n"
        "namespace count %d\n"
        "partition       %s\n"
        "size            %u\n"
    ), 
        stats.used_entries, stats.free_entries, stats.total_entries, stats.namespace_count,
        part->label, part->size
    );
}

void loop()
{
    int32_t counter;

    // try to get counter
    if (nvs_get_i32(nvs, "counter", &counter) != ESP_OK) {
        counter = 0;
    }
    // increment counter and store it
    ERROR_CHECK(nvs_set_i32(nvs, "counter", ++counter));
    // make changes permanent
    ERROR_CHECK(nvs_commit(nvs));

    // display counter
    Serial.printf_P(PSTR("counter %d\n"), counter);

    // every 100 loops, stop application
    if (counter % 100 == 0) {
        print_free_heap();
        nvs_close(nvs);

        print_free_heap();
        ERROR_CHECK(nvs_flash_deinit());

        print_free_heap();

        Serial.println(F("application stopped, reset device to run again..."));
        halt();
    }

    delay(1000);
}
