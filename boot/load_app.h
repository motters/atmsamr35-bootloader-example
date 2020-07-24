#pragma once

#include <stdio.h>
#include <stdint.h>

#include <memory_map_api.h>
#include <GlobalStartup.h>
#include <shared_memory.h>
#include <image.h>

#include "utils.h"

bool app_verify(image_slot_t slot, const image_hdr_t *hdr);
void app_start(const image_hdr_t *hdr);