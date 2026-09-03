file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")
cmake_minimum_required(VERSION 3.10)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(image_morph)

include(FetchContent)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb/
    GIT_TAG master
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendors/stb/
)
FetchContent_Populate(stb)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/)
add_executable(image_morph)

target_sources(image_morph PRIVATE ${SOURCES})
target_include_directories(image_morph PRIVATE vendors/stb/)
