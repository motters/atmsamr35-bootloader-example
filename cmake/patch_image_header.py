import argparse
import binascii
import struct
import hashlib
import subprocess
import OpenSSL
from OpenSSL import crypto
import base64
import ecdsa
import os
import struct
from codecs import decode, encode
import re
import codecs

"""
This script update the image section of the firmware file
    - See project credits for more information
"""


def patch_binary_payload(bin_filename, key_locations):
    """
    Patch crc, data_size & signature fields of image_hdr_t in place in binary
    Raise exception if binary is not a supported type
    """
    # Size of the header
    IMAGE_HDR_SIZE_BYTES = 128

    # Binary header marker to ensure we have the correct firmware
    IMAGE_HDR_MAGIC = 0xed9e

    # Open the bin file and read the content
    with open(bin_filename, "rb") as f:
        image_hdr = f.read(IMAGE_HDR_SIZE_BYTES)
        data = f.read()

    # Get the image marker if we have the correct bin file
    image_magic = struct.unpack("<H", image_hdr[0:2])

    # Is the image marker read the same as we would expect?
    if image_magic[0] != IMAGE_HDR_MAGIC:
        raise Exception(
            "Unsupported Binary Type. Expected 0x{:02x} Got 0x{:02x}".format(
                IMAGE_HDR_MAGIC, image_magic
            )
        )

    # Get the size of the firmware
    data_size = len(data)

    # Calculate the firmware CRC
    crc32 = binascii.crc32(data) & 0xffffffff

    # Create hash of firmware
    #data2 = "data"
    #data2_sha = hashlib.sha256(data2.encode('utf-8')).hexdigest()
    #print(data2_sha)
    #open("data.sha256", "w").write(data2_sha)

    # Sign  secp256r1 = prime256v1
    key_data = subprocess.run(["openssl", "dgst", "-sha256", "-sign", "../../keys/DEBUG/ec-priv.pem", "SAMR35-app.bin"], capture_output=True)
    print(key_data.stdout)
    open("SAMR35-app.sha256", "wb").write(key_data.stdout)
    open("SAMR35-app.sha256.hex", "w").write(codecs.getencoder('hex_codec')(key_data.stdout)[0].decode('utf-8'))

    # Verify
    verify_data = subprocess.run(["openssl", "dgst", "-sha256", "-verify", "../../keys/DEBUG/public.pem", "-signature", "SAMR35-app.sha256", "SAMR35-app.bin"], capture_output=True)
    print(verify_data.stdout)
    open("SAMR35-app.verify", "wb").write(verify_data.stdout)

    # Format sig for firmware; decode from DER format
    key_data_parse = subprocess.run(["openssl", "asn1parse", "-inform", "DER", "-in", "SAMR35-app.sha256"], capture_output=True)
    open("SAMR35-app.sha256.asn", "wb").write(key_data_parse.stdout)
    # Parse the decoded data
    ans_decoded_ascii = open("SAMR35-app.sha256.asn", "r").read()
    regex = r"INTEGER           :(\w*)"
    ints = re.findall(regex, ans_decoded_ascii, re.MULTILINE)
    hex_string = ints[0] + ints[1]
    n = 2
    # Put decode data into list
    #signature = [int(hex_string[i:i+n], 16) for i in range(0, len(hex_string), n)]
    #print(signature)
    #print(len(signature))

    # Sign the bin file
    #signature = [147, 118, 133, 98, 254, 90, 180, 160, 50, 37, 7, 47, 23, 108, 252, 183, 96, 145,
    #    12, 35, 199, 171, 143, 229, 139, 126, 242, 197, 115, 25, 68, 57, 211, 101, 66, 94, 122, 29, 125, 84, 7, 173,
    #    247, 19, 34, 48, 32, 41, 50, 247, 218, 132, 3, 19, 31, 171, 80, 137, 109, 109, 68, 81, 163, 77]
    signature = key_data.stdout.split(sep=None, maxsplit=1)[0]
    signature_size = len(signature)

    # Assemble the generated information
    image_hdr_crc_data_size = struct.pack("<LL{}BL".format(signature_size), crc32, data_size, *signature, signature_size)
    print(
        "Adding crc:{} data_size:{} & signature to '{}'".format(
            crc32, data_size, bin_filename
        )
    )

    # Open the binary file
    with open(bin_filename, "r+b") as f:
        # Seek to beginning of header "uint32_t crc" section
        f.seek(2)
        # Write the assembled information into the crc, data_size & signature position
        f.write(image_hdr_crc_data_size)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("bin", action="store")
    parser.add_argument("key_locations", action="store")
    args = parser.parse_args()

    patch_binary_payload(args.bin, args.key_locations)
