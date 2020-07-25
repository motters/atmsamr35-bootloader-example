#pragma once

#include <stdio.h>
#include <stdint.h>

#include <memory_map_api.h>
#include <GlobalStartup.h>
#include <shared_memory.h>
#include <image.h>

#include "utils.h"

void app_start(const image_hdr_t *hdr);