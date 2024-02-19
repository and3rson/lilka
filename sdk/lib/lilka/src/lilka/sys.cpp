#include "sys.h"

namespace lilka {

int Sys::get_partition_labels(String labels[]) {
    const esp_partition_t *partition;
    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    int count = 0;
    while (iterator != NULL) {
        partition = esp_partition_get(iterator);
        labels[count++] = String(partition->label);
        iterator = esp_partition_next(iterator);
    }
    return count;
}

void Sys::print_partition_table() {
    const esp_partition_t *partition;
    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (iterator != NULL) {
        partition = esp_partition_get(iterator);
        printf("Partition label=%10s type=%02X subtype=%02X address=0x%08X size=0x%08X\n", partition->label, partition->type, partition->subtype, partition->address, partition->size);
        iterator = esp_partition_next(iterator);
    }
}

Sys sys;

} // namespace lilka
