PROJECT(app LANGUAGES CXX)

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.12)

# setup library files
file(GLOB_RECURSE folder_header *.h *.hpp)
file(GLOB_RECURSE folder_source *.c *.cpp)

# Find git sha
execute_process (COMMAND GIT rev-parse --short HEAD
        WORKING_DIRECTORY ../${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

ADD_DEFINITIONS(
        -DGIT_SHA="${VERSION}"
)

add_library(${PROJECT_NAME} OBJECT $<TARGET_OBJECTS:ASF> $<TARGET_OBJECTS:framework> ${folder_header} ${folder_source})

target_compile_definitions(${PROJECT_NAME}
        INTERFACE
        GIT_SHA="${VERSION}"
        )

target_link_libraries(${PROJECT_NAME} ASF framework)







