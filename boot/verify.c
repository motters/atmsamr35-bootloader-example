#include "verify.h"

#include <string.h>

/* Simple public domain implementation of the standard CRC32 checksum.
 * Outputs the checksum for each file given as a command line argument.
 * Invalid file names and files that cause errors are silently skipped.
 * The program reads from stdin if it is called with no arguments.
 *
 * From http://home.thep.lu.se/~bjorn/crc/ */

static uint32_t crc32_for_byte(uint32_t r)
{
    for(int j = 0; j < 8; ++j)
        r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
    return r ^ (uint32_t)0xFF000000L;
}

static uint32_t crc32(const void *data, uint32_t n_bytes)
{
    uint32_t crc = 0;
    static uint32_t table[0x100];
    if(!*table)
        for(size_t i = 0; i < 0x100; ++i)
            table[i] = crc32_for_byte(i);
    for(size_t i = 0; i < n_bytes; ++i)
        crc = table[(uint8_t)crc ^ ((uint8_t*)data)[i]] ^ crc >> 8;

    return crc;
}

/**
 * Check the slot / app flash CRC matches what is expected
 *
 * @param slot
 * @param hdr
 * @return
 */
bool crc_verification(image_slot_t slot,  const image_hdr_t *hdr)
{
    // Find address
    void *addr = NULL;
    switch (slot)
    {
        case IMAGE_SLOT_1:
            addr = &__approm_start__;
            break;
        default:
            return false;
    }

    // Find the size of the header
    addr += sizeof(image_hdr_t);

    // Get the size of the firmware bin
    uint32_t len = hdr->data_size;

    // Calculate the CRC of the app firmware
    uint32_t calc_app_crc = crc32(addr, len);

    // Obtains the CRC from flash
    uint32_t header_crc = hdr->crc;

    // Does the calc crc match the one in flash
    if (calc_app_crc == header_crc)
        return true;

    // CRC Failed
    return false;
}

void generate_application_hash(unsigned char* output)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    BYTE page_output[64] = {};

    // 2048 total pages - 64 pages where the app starts - 2 pages for the app header
    for (int idx = 0; idx < 2048-64+2; ++idx)
    {
        read_page(64-2+idx, page_output);
        sha256_update(&ctx, page_output, strlen(page_output));
    }

    sha256_final(&ctx, output);
}


#if GEN_KEY==1
static uint64_t g_rand = 88172645463325252ull;
static int fake_rng(uint8_t *dest, unsigned size)
{
    while (size) {
        g_rand ^= (g_rand << 13);
        g_rand ^= (g_rand >> 7);
        g_rand ^= (g_rand << 17);

        unsigned amount = (size > 8 ? 8 : size);
        memcpy(dest, &g_rand, amount);
        dest += amount;
        size -= amount;
    }
    return 1;
}
#endif


bool security_verification(image_slot_t slot,  const image_hdr_t *hdr)
{
    // Create a hash of the firmware
    unsigned char output[SHA256_BLOCK_SIZE];
    generate_application_hash(&output);

#if GEN_KEY==0
    // Set the public key
    uint8_t public[uECC_BYTES * 2] = {
        108, 171, 139, 43, 141, 113, 38, 126, 108, 27, 24, 120, 12, 53, 61, 5, 150, 162, 26, 197, 163, 203, 65, 164, 156, 108,
        21, 107, 254, 100, 224, 231, 125, 123, 168, 252, 196, 77, 131, 235, 134, 78, 118, 82, 135, 150, 213, 245, 145,
        166, 54, 41, 163, 238, 207, 173, 3, 84, 239, 239, 164, 212, 124, 245 };

    // The expected sign
    //uint8_t sig[uECC_BYTES * 2] = ;
#else
    uECC_set_rng(&fake_rng);

    uint8_t public[uECC_BYTES * 2];
    uint8_t private[uECC_BYTES];
    uint8_t hash[uECC_BYTES];
    uint8_t sig[uECC_BYTES * 2];

    if (!uECC_make_key(public, private)) {
        printf("uECC_make_key() failed\n");
        return false;
    }
    memcpy(hash, public, uECC_BYTES);

    if (!uECC_sign(private, output, sig)) {
        printf("uECC_sign() failed\n");
        return false;
    }
    printf("\r\nFirmware signature:\r\n");
    for(int i = 0; i < sizeof(sig); i++)
        printf("%d, ", sig[i]);

    printf("\r\n\r\nFirmware public key: \r\n");
    for(int i = 0; i < sizeof(public); i++)
        printf("%d, ", public[i]);

    printf("\r\n");
#endif

    // Check the firmware matches the sig
    if (!uECC_verify(public, output, hdr->signature))
        return false;

    return true;
}