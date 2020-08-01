#include "verify.h"

#include <string.h>
#include <keys.h>



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

/**
 * Create a sha-256 hash of the application
 *
 * @param hdr
 * @param output
 */
void generate_application_hash(const image_hdr_t *hdr, unsigned char* output)
{
    // Sha256 hash setup
    SHA256_CTX ctx;
    sha256_init(&ctx);

    // Find the size of the header
    void *addr = NULL;
    addr = &__approm_start__;
    addr += sizeof(image_hdr_t);

    // Slow but reliable for now
    for(size_t i = 0; i < hdr->data_size; ++i)
    {
        uint8_t value[1] = {((uint8_t*)addr)[0]};
        sha256_update(&ctx, value, 1);
        addr += 1;
    }

    // Compute the final hash
    sha256_final(&ctx, output);
}

/**
 * Verify the application's ECDSA signature
 *
 * @param slot
 * @param hdr
 * @return
 */
bool security_verification(image_slot_t slot,  const image_hdr_t *hdr)
{
    // Create a hash of the firmware
    unsigned char output[SHA256_BLOCK_SIZE];
    generate_application_hash(hdr, &output);

#if VERIFY_DEBUG_COMMENTS == 1
    printf("sha: ");
    for(int i = 0; i < SHA256_BLOCK_SIZE; i++)
        printf(" %x ", output[i]);
    printf("\n\n");

    printf("image sig: ");
    for(int i = 0; i < 64; i++)
        printf(" %d", hdr->signature[i]);
    printf("\n\n");

    printf("key: ");
    for(int i = 0; i < 64; i++)
        printf(" %d ", PUBLIC_KEY[i]);
    printf("\n\n");
#endif

    // Check the firmware matches the sig
    const struct uECC_Curve_t * curve = uECC_secp256k1();
    if (!uECC_verify(&PUBLIC_KEY[0], &output[0], 64, &hdr->signature[0], curve))
        return false;

    return true;
}