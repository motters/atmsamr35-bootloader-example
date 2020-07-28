# SAMR35 Bootloader Example

This mini-projects show how to create a bootloader that loads an application in flash.

## Setup

### System Requirements

Arm GNU compiler, CMake;

Python 3 with the following

```shell script
pip install ecdsa cryptography asn1 future pyasn1
```

### Project Setup

#### Create Key

A python tool is provided to create the public & private keys. You can create different keys from DEBUG & RELEASE.

```shell script
python cmake/create_key.py DEBUG
```

# Credit
  
  * https://github.com/memfault/interrupt
  * https://github.com/armmbed/mbed-os-example-lorawan-fuota
  * https://github.com/B-Con/crypto-algorithms
  * https://github.com/kmackay/micro-ecc
  * http://ww1.microchip.com/downloads/en/AppNotes/Atmel-42141-SAM-AT02333-Safe-and-Secure-Bootloader-Implementation-for-SAM3-4_Application-Note.pdf