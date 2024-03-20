#pragma once
/*
    This file should be a place to store common for all filesystems
    parts
*/
// definitions required for SPIFFS and SDCard Browsers
namespace lilka {
typedef enum {
    ENT_FILE,
    ENT_DIRECTORY,
} EntryType;

typedef struct {
    String name;
    EntryType type;
    size_t size;
} Entry;
} // namespace lilka