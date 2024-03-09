#ifndef LILKA_SYS_H
#define LILKA_SYS_H

#include <Arduino.h>
#include <esp_partition.h>

namespace lilka {

class Sys {
public:
    int get_partition_labels(String labels[]);
    uint64_t get_partition_address(const char* label);
    uint64_t get_partition_size(const char* label);
    void print_partition_table();
};

extern Sys sys;

} // namespace lilka

#endif // LILKA_SYS_H
