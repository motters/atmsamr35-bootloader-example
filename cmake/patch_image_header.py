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
import hashlib
from ecdsa import SigningKey, VerifyingKey
from ecdsa.util import sigencode_der, sigdecode_der


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
        data_byes = bytearray(data)


    # Get the image marker if we have the correct bin file
    image_magic = struct.unpack("<H", image_hdr[0:2])

    # Is the image marker read the same as we would expect?
    if image_magic[0] != IMAGE_HDR_MAGIC:
        raise Exception(
            "Unsupported Binary Type. Expected 0x{:02x} Got 0x{:02x}".format(
                IMAGE_HDR_MAGIC, image_magic
            )
        )

    m = hashlib.sha256()
    #for number in data: # [:2]
    #    #print(number.to_bytes(1, 'little', signed=False).hex())
     #   m.update(number.to_bytes(1, 'little', signed=False))
    m.update(data)
    hash = m.digest()
    print(binascii.hexlify(hash, '-'))
    print("")

    # Get the size of the firmware
    data_size = len(data)

    # Calculate the firmware CRC
    crc32 = binascii.crc32(data) & 0xffffffff

    # Sign  secp256r1 = prime256v1
    with open("../../keys/DEBUG/private.txt", "rb") as f:
        private_key = f.read()
        #private_key_hex = private_key.to_bytes(1, 'little', signed=False).hex()
        print(private_key)
        sk = ecdsa.SigningKey.from_string(private_key, curve=ecdsa.SECP256k1, hashfunc=hashlib.sha256)
        #signed = sk.sign(data, hashfunc=hashlib.sha256, sigencode=ecdsa.util.sigencode_string)
        signed = sk.sign_digest(hash)
        print(binascii.hexlify(signed, '-'))
        n = 2
        signature = [int(signed.hex()[i:i+n], 16) for i in range(0, len(signed.hex()), n)]
        print(signature)
        print(len(signature))

    # Assemble the generated information 64->len(signature)
    image_hdr_crc_data_size = struct.pack("<LL{}B".format(64), crc32, data_size, *signature)
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
