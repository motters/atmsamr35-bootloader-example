import argparse
import binascii
import sys
import subprocess

"""
This script converts the app bin to a C array for the updater to use
    - See project credits for more information
"""


def make_sublist_group(lst: list, grp: int) -> list:
    """
    Group list elements into sublists.
    make_sublist_group([1, 2, 3, 4, 5, 6, 7], 3) = [[1, 2, 3], [4, 5, 6], 7]
    """
    return [lst[i:i+grp] for i in range(0, len(lst), grp)]


def do_convension(content: bytes, to_uppercase: bool=False) -> str:
    hexstr = binascii.hexlify(content).decode("UTF-8")
    if to_uppercase:
        hexstr = hexstr.upper()
    element_prefix = '0x'
    element_suffix = ''
    separator_string = ', '
    linebreak_string = '\n'
    linebreak = True
    array = [element_prefix + hexstr[i:i + 2] + element_suffix for i in range(0, len(hexstr), 2)]
    if linebreak:
        array = make_sublist_group(array, 15)
    else:
        array = [array,]

    return linebreak_string.join(["    " + separator_string.join(e) + separator_string for e in array])


def create_array(input, output):
    """
    Create plain text keys
    """
    with open(input, 'rb') as f:
        file_content = f.read()

    size = str(len(file_content))
    ret = "#pragma once\n\n"
    ret += "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n#include <stdint.h>\n\n"
    ret += "uint8_t firmware_upgrade[] = {\n" + do_convension(file_content) + " \n};\nuint32_t firmware_upgrade_size = " + size + ";\n\n"
    ret += "#ifdef __cplusplus\n}\n#endif"

    with open(output, 'w') as f:
        f.write(ret)

    # Success
    print("Successfully created C array")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("input", action="store")
    parser.add_argument("output", action="store")
    args = parser.parse_args()

    create_array(args.input, args.output)
