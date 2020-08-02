PROJECT(boot LANGUAGES CXX)

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.12)

# setup library files
file(GLOB_RECURSE folder_header *.h)
file(GLOB_RECURSE folder_source *.c)

add_definitions(
        -DuECC_ASM=uECC_asm_small
        -DuECC_SQUARE_FUNC=0
        -DuECC_CURVE=uECC_secp256k1
        -DuECC_VLI_NATIVE_LITTLE_ENDIAN=0
        -DuECC_OPTIMIZATION_LEVEL=1
        -DuECC_PLATFORM=uECC_arm_thumb
        -DuECC_SUPPORTS_secp160r1=0
        -DuECC_SUPPORTS_secp192r1=0
        -DuECC_SUPPORTS_secp224r1=0
        -DuECC_SUPPORTS_secp256r1=0
        -DuECC_SUPPORT_COMPRESSED_POINT=0
)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g3 -flto") # -flto

include_directories(../vendor/micro-ecc ../keys/DEBUG)

add_library(${PROJECT_NAME} OBJECT $<TARGET_OBJECTS:ASF> $<TARGET_OBJECTS:framework> ../vendor/micro-ecc/uECC.h ../vendor/micro-ecc/uECC.c ../keys/DEBUG/keys.h ../keys/DEBUG/keys.c ${folder_header} ${folder_source})

target_link_libraries(${PROJECT_NAME} ASF framework)







