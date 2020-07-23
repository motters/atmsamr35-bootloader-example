PROJECT(app LANGUAGES CXX)

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.12)

# setup library files
file(GLOB_RECURSE folder_header *.h *.hpp)
file(GLOB_RECURSE folder_source *.c *.cpp)

add_library(${PROJECT_NAME} OBJECT $<TARGET_OBJECTS:ASF> $<TARGET_OBJECTS:framework> ${folder_header} ${folder_source})

target_link_libraries(${PROJECT_NAME} ASF framework)







