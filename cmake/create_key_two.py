import argparse
import binascii
import struct
import hashlib
import ecdsa
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.primitives.serialization import load_pem_private_key
from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature

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
    firmware_hash = hashlib.sha256(data)

    # Get private key
    curve = ec.SECP256R1()
    signature_algorithm = ec.ECDSA(hashes.SHA256())
    serialized_private_string = open(key_locations + "/ec-priv.pem", "r").read()
    with open(key_locations + "/ec-priv.pem", 'rb') as pem_in:
        serialized_private_string = pem_in.read()

    priv_key = load_pem_private_key(serialized_private_string, None, default_backend())

    signature_dss = priv_key.sign(data, signature_algorithm)
    result, signature = decode_dss_signature(signature_dss)

    #hex_data = signature
    signature_str = str(signature)
    print(signature)
    print(result)
    #AES_KEY = str(bytearray.fromhex(signature_str))
    #hex_data = bytes.fromhex(signature_str)

    #print(AES_KEY)
    #print('Signature: %s' % hex_data)

    # Sign the bin file
    signature = [147, 118, 133, 98, 254, 90, 180, 160, 50, 37, 7, 47, 23, 108, 252, 183, 96, 145,
                 12, 35, 199, 171, 143, 229, 139, 126, 242, 197, 115, 25, 68, 57, 211, 101, 66, 94, 122, 29, 125, 84, 7, 173,
                 247, 19, 34, 48, 32, 41, 50, 247, 218, 132, 3, 19, 31, 171, 80, 137, 109, 109, 68, 81, 163, 77]

    # Assemble the generated information
    image_hdr_crc_data_size = struct.pack("<LL{}B".format(len(signature)), crc32, data_size, *signature)
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
