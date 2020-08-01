#pragma once

#include <uECC.h>
#include <keys.h>

#include <sha256.h>
#include <flash.h>
#include <image.h>

#define VERIFY_DEBUG_COMMENTS 0

bool security_verification(image_slot_t slot,  const image_hdr_t *hdr);
bool crc_verification(image_slot_t slot, const image_hdr_t *hdr);