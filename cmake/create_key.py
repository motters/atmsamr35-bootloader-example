import argparse
import ecdsa
from ecdsa import SigningKey, SECP256k1

import subprocess

"""
This script update the image section of the firmware file
    - See project credits for more information
"""


def patch_binary_payload(key_type):
    """
    Create plain text keys
    """
    # Define location to store core
    key_dir = 'keys/' + key_type

    # Create a key
    sk = ecdsa.SigningKey.generate(curve=ecdsa.SECP256k1, hashfunc=None)
    vk = sk.get_verifying_key()

    # Private key
    sklst = []
    for e in bytearray(sk.to_string()):
        sklst.append(e)

    # Public key
    vklst = []
    for e in bytearray(vk.to_string()):
        vklst.append(e)

    # Create a C array of the pubic key
    c_array_h = "#pragma once\n#include <stdint.h>\nextern uint8_t PUBLIC_KEY[64];extern uint8_t PRIVATE_KEY[32];\nextern uint8_t PUBLIC_KEY_LENGTH;"
    c_array_c = "#include \"keys.h\"\n"
    c_array_c += "uint8_t PUBLIC_KEY[64] = { " + ','.join(map(str, vklst)) + " };\n"
    c_array_c += "uint8_t PRIVATE_KEY[32] = { " + ','.join(map(str, sklst)) + " };\n"
    c_array_c += "uint8_t PUBLIC_KEY_LENGTH = 64;\n"

    # Save the C array public keys to a file
    open(key_dir + "/keys.c", "w").write(str(c_array_c))
    open(key_dir + "/keys.h", "w").write(str(c_array_h))
    open(key_dir + "/private.bin", "wb").write(sk.to_string())
    open(key_dir + "/public.txt", "w").write(str(vk.to_string()))

    # Success
    print("Successfully created key assets")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("key_type", action="store")
    args = parser.parse_args()

    patch_binary_payload(args.key_type)
