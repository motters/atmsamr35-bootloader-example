import argparse
import binascii
import struct
import hashlib
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.exceptions import InvalidSignature
import os
import errno
import re

import subprocess

"""
This script update the image section of the firmware file
    - See project credits for more information
"""


def patch_binary_payload(key_type):
    """
    Create plain text keys
    """
    key_dir = 'keys/' + key_type

    # secp256r1 = prime256v1
    create_key = subprocess.run(
        ["openssl", "ecparam", "-name", "secp256r1", "-genkey", "-out", key_dir + "/ec-priv.pem"],
        capture_output=True, universal_newlines=True)
    public_key = subprocess.run(
        ["openssl", "ec", "-in", key_dir + "/ec-priv.pem", "-pubout", "-out", key_dir + "/public.pem"],
        capture_output=True, universal_newlines=True)  # -conv_form uncompressed
    private_key = subprocess.run(["openssl", "ec", "-in", key_dir + "/ec-priv.pem", "-out", key_dir + "/private.pem"],
                                 capture_output=True, universal_newlines=True)
    key_data = subprocess.run(["openssl", "ec", "-in", key_dir + "/ec-priv.pem", "-text", "-noout"],
                              capture_output=True, universal_newlines=True)

    # Get private key
    private_regex = r"priv:\n\s{4}([\w\w:]*)\n\s{4}([\w\w:]*)\n\s{4}([\w\w:]*)\n"
    private_matches = re.findall(private_regex, key_data.stdout, re.MULTILINE)
    private_key = private_matches[0][0] + private_matches[0][1] + private_matches[0][2]

    # Get public key
    public_regex = r"pub:\n\s{4}([\w\w:]*)\n\s{4}([\w\w:]*)\n\s{4}([\w\w:]*)\n\s{4}([\w\w:]*)\n\s{4}([\w\w:]*)\n"
    public_matches = re.findall(public_regex, key_data.stdout, re.MULTILINE)
    public_key = public_matches[0][0] + public_matches[0][1] + public_matches[0][2] + public_matches[0][3] + \
                 public_matches[0][4]
    print(key_data.stdout)
    print(len(public_key[3:].replace(":", "")))
    # Create a C array of the pubic key
    c_array_h = "#pragma once\n#include <stdint.h>\nextern uint8_t PUBLIC_KEY[64];\nextern uint8_t PUBLIC_KEY_LENGTH;"
    c_array_c = "#include \"keys.h\"\n"
    c_array_c += "uint8_t PUBLIC_KEY[64] = { 0x" + public_key[3:].replace(":", ", 0x") + " };\n"
    c_array_c += "uint8_t PUBLIC_KEY_LENGTH = 64;\n"

    # Save the C array public keys to a file
    open(key_dir + "/keys.c", "w").write(str(c_array_c))
    open(key_dir + "/keys.h", "w").write(str(c_array_h))

    # Success
    print("Successfully created key assets")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("key_type", action="store")
    args = parser.parse_args()

    patch_binary_payload(args.key_type)
