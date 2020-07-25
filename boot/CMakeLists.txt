PROJECT(boot LANGUAGES CXX)

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.12)

# setup library files
file(GLOB_RECURSE folder_header *.h)
file(GLOB_RECURSE folder_source *.c)

add_definitions(
        -DuECC_ASM=uECC_asm_small
        -DECC_SQUARE_FUNC=0
        -DuECC_CURVE=uECC_secp256r1
)

include_directories(../vendor/micro-ecc)

add_library(${PROJECT_NAME} OBJECT $<TARGET_OBJECTS:ASF> $<TARGET_OBJECTS:framework> ../vendor/micro-ecc/uECC.h ../vendor/micro-ecc/uECC.c ${folder_header} ${folder_source})

target_link_libraries(${PROJECT_NAME} ASF framework)







