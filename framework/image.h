#pragma once

#include <stdint.h>
#include <limits.h>

#include "memory_map_api.h"

#define IMAGE_MAGIC 0xed9e

// Encase we have two paritions in the future
// Low on flash memory so probs just have one for mvp
typedef enum {
    IMAGE_SLOT_1 = 1,
} image_slot_t;

// Allow 32 bytes + 64 + 32 = 128 bytes aka 2 pages
// WARNING: if you change this struct you must modify the image_header.py script
typedef struct __attribute__((packed)) {
    uint16_t image_magic;
    uint32_t crc;
    uint32_t data_size;
    uint8_t signature[72];
    uint32_t signature_size;
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_patch;
    uint8_t version_rc;
    uint32_t vector_addr;
    uint16_t reserved_2;
    char git_sha[8];
    uint8_t reserved_3[24];
} image_hdr_t;


const image_hdr_t *image_get_header(image_slot_t slot);

