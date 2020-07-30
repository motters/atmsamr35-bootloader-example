PROJECT(boot LANGUAGES CXX)

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.12)

# setup library files
file(GLOB_RECURSE folder_header *.h)
file(GLOB_RECURSE folder_source *.c)

set(MBED_LOCATION_LIBS ../vendor/mbedtls/library)
set(src_crypto
       # ${MBED_LOCATION_LIBS}/aes.c
        #${MBED_LOCATION_LIBS}/aesni.c
       # ${MBED_LOCATION_LIBS}/arc4.c
       # ${MBED_LOCATION_LIBS}/aria.c
        ${MBED_LOCATION_LIBS}/asn1parse.c
        ${MBED_LOCATION_LIBS}/asn1write.c
        ${MBED_LOCATION_LIBS}/base64.c
        ${MBED_LOCATION_LIBS}/bignum.c
       # ${MBED_LOCATION_LIBS}/blowfish.c
        #${MBED_LOCATION_LIBS}/camellia.c
       # ${MBED_LOCATION_LIBS}/ccm.c
        #${MBED_LOCATION_LIBS}/chacha20.c
        ##${MBED_LOCATION_LIBS}/chachapoly.c
        #${MBED_LOCATION_LIBS}/cipher.c
        #${MBED_LOCATION_LIBS}/cipher_wrap.c
       # ${MBED_LOCATION_LIBS}/cmac.c
        #${MBED_LOCATION_LIBS}/ctr_drbg.c
       # ${MBED_LOCATION_LIBS}/des.c
       # ${MBED_LOCATION_LIBS}/dhm.c
        ${MBED_LOCATION_LIBS}/ecdh.c
        ${MBED_LOCATION_LIBS}/ecdsa.c
        #${MBED_LOCATION_LIBS}/ecjpake.c
        ${MBED_LOCATION_LIBS}/ecp.c
        ${MBED_LOCATION_LIBS}/ecp_curves.c
        ${MBED_LOCATION_LIBS}/entropy.c
        ${MBED_LOCATION_LIBS}/entropy_poll.c
        #${MBED_LOCATION_LIBS}/error.c
       # ${MBED_LOCATION_LIBS}/gcm.c
        #${MBED_LOCATION_LIBS}/havege.c
        #${MBED_LOCATION_LIBS}/hkdf.c
        #${MBED_LOCATION_LIBS}/hmac_drbg.c
        ${MBED_LOCATION_LIBS}/md.c
       # ${MBED_LOCATION_LIBS}/md2.c
       # ${MBED_LOCATION_LIBS}/md4.c
       # ${MBED_LOCATION_LIBS}/md5.c
        ${MBED_LOCATION_LIBS}/memory_buffer_alloc.c
       # ${MBED_LOCATION_LIBS}/nist_kw.c
        ${MBED_LOCATION_LIBS}/oid.c
       # ${MBED_LOCATION_LIBS}/padlock.c
        ${MBED_LOCATION_LIBS}/pem.c
        ${MBED_LOCATION_LIBS}/pk.c
        ${MBED_LOCATION_LIBS}/pk_wrap.c
        ${MBED_LOCATION_LIBS}/pkcs12.c
       # ${MBED_LOCATION_LIBS}/pkcs5.c
        ${MBED_LOCATION_LIBS}/pkparse.c
        ${MBED_LOCATION_LIBS}/pkwrite.c
        ${MBED_LOCATION_LIBS}/platform.c
        ${MBED_LOCATION_LIBS}/platform_util.c
       # ${MBED_LOCATION_LIBS}/poly1305.c
       # ${MBED_LOCATION_LIBS}/psa_crypto.c
       # ${MBED_LOCATION_LIBS}/psa_crypto_se.c
       # ${MBED_LOCATION_LIBS}/psa_crypto_slot_management.c
       # ${MBED_LOCATION_LIBS}/psa_crypto_storage.c
        #${MBED_LOCATION_LIBS}/psa_its_file.c
        #${MBED_LOCATION_LIBS}/ripemd160.c
       # ${MBED_LOCATION_LIBS}/rsa.c
       # ${MBED_LOCATION_LIBS}/rsa_internal.c
        ${MBED_LOCATION_LIBS}/sha1.c
        ${MBED_LOCATION_LIBS}/sha256.c
        )

add_definitions(
        -DuECC_ASM=uECC_asm_small
        -DECC_SQUARE_FUNC=0
        -DuECC_CURVE=uECC_secp256r1
        -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
        -DuECC_OPTIMIZATION_LEVEL=3
        -DMBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
)

#include_directories(../vendor/micro-ecc ../keys/DEBUG)
include_directories(../vendor/mbedtls/include ../keys/DEBUG ../)

#add_library(${PROJECT_NAME} OBJECT $<TARGET_OBJECTS:ASF> $<TARGET_OBJECTS:framework> ../vendor/micro-ecc/uECC.h ../vendor/micro-ecc/uECC.c ../keys/DEBUG/keys.h ../keys/DEBUG/keys.c ${folder_header} ${folder_source})

set(MBEDTLS_DIR ../vendor/mbedtls)

# $<TARGET_OBJECTS:mbedcrypto>
add_library(${PROJECT_NAME} OBJECT $<TARGET_OBJECTS:ASF> $<TARGET_OBJECTS:framework>
        ${src_crypto}
        ../mbedtls_config.h ../keys/DEBUG/keys.h ../keys/DEBUG/keys.c ${folder_header} ${folder_source})

#target_link_libraries(${PROJECT_NAME} ASF framework)
target_link_libraries(${PROJECT_NAME} ASF framework) # mbedtls mbedcrypto mbedx509







