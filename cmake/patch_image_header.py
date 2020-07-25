import argparse
import binascii
import struct

"""
Modifed version of the below
See: https://github.com/memfault/interrupt/blob/master/example/fwup-architecture/patch_image_header.py
"""

def patch_binary_payload(bin_filename):
    """
    Patch crc & data_size fields of image_hdr_t in place in binary
    Raise exception if binary is not a supported type
    """
    IMAGE_HDR_SIZE_BYTES = 128
    IMAGE_HDR_MAGIC = 0xed9e

    with open(bin_filename, "rb") as f:
        image_hdr = f.read(IMAGE_HDR_SIZE_BYTES)
        data = f.read()

    image_magic = struct.unpack("<H", image_hdr[0:2])

    if image_magic[0] != IMAGE_HDR_MAGIC:
        raise Exception(
            "Unsupported Binary Type. Expected 0x{:02x} Got 0x{:02x}".format(
                IMAGE_HDR_MAGIC, image_magic
            )
        )

    data_size = len(data)
    crc32 = binascii.crc32(data) & 0xffffffff
    signature = [147, 118, 133, 98, 254, 90, 180, 160, 50, 37, 7, 47, 23, 108, 252, 183, 96, 145, 12, 35, 199, 171, 143,
                 229, 139, 126, 242, 197, 115, 25, 68, 57, 211, 101, 66, 94, 122, 29, 125, 84, 7, 173, 247, 19, 34, 48,
                 32, 41, 50, 247, 218, 132, 3, 19, 31, 171, 80, 137, 109, 109, 68, 81, 163, 77]


    image_hdr_crc_data_size = struct.pack("<LL{}B".format(len(signature)), crc32, data_size, *signature)
    print(
        "Adding crc:{} data_size:{} to '{}'".format(
            crc32, data_size, bin_filename
        )
    )

    with open(bin_filename, "r+b") as f:
        # Seek to beginning of "uint32_t crc"
        f.seek(2)
        # Write correct values into crc & data_size
        f.write(image_hdr_crc_data_size)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("bin", action="store")
    args = parser.parse_args()

    patch_binary_payload(args.bin)