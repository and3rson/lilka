#include "sys.h"

namespace lilka {

#define FOREACH_PARTITION()                                                                                          \
    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL); \
    int i = 0;                                                                                                       \
    for (i = 0; iterator != NULL; i++, iterator = esp_partition_next(iterator))

int Sys::get_partition_labels(String labels[]) {
    FOREACH_PARTITION() {
        const esp_partition_t* partition = esp_partition_get(iterator);
        labels[i] = String(partition->label);
    }
    return i;
}

uint64_t Sys::get_partition_address(const char* label) {
    FOREACH_PARTITION() {
        const esp_partition_t* partition = esp_partition_get(iterator);
        if (strcmp(partition->label, label) == 0) {
            return partition->address;
        }
    }
    return 0;
}

uint64_t Sys::get_partition_size(const char* label) {
    FOREACH_PARTITION() {
        const esp_partition_t* partition = esp_partition_get(iterator);
        if (strcmp(partition->label, label) == 0) {
            return partition->size;
        }
    }
    return 0;
}

void Sys::print_partition_table() {
    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (iterator != NULL) {
        const esp_partition_t* partition = esp_partition_get(iterator);
        printf(
            "Partition label=%10s type=%02X subtype=%02X address=0x%08X size=0x%08X\n",
            partition->label,
            partition->type,
            partition->subtype,
            partition->address,
            partition->size
        );
        iterator = esp_partition_next(iterator);
    }
}

Sys sys;

} // namespace lilka
