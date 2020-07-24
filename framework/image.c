#include "image.h"


/**
 * Get the header info and cast the struct
 *
 * @param slot
 * @return
 */
const image_hdr_t *image_get_header(image_slot_t slot)
{
    const image_hdr_t *hdr = NULL;

    switch (slot)
    {
        case IMAGE_SLOT_1:
            hdr = (const image_hdr_t *)&__approm_start__;
            break;
        default:
            break;
    }

    if (hdr && hdr->image_magic == IMAGE_MAGIC)
        return hdr;

    return NULL;
}