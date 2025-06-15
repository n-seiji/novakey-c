cmake_minimum_required(VERSION 3.20)
project(novakey VERSION 1.0.0 LANGUAGES C CXX OBJC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)

# Find MeCab using mecab-config
find_program(MECAB_CONFIG mecab-config)
if(NOT MECAB_CONFIG)
    message(FATAL_ERROR "mecab-config not found")
endif()

execute_process(COMMAND ${MECAB_CONFIG} --cflags
                OUTPUT_VARIABLE MECAB_CFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${MECAB_CONFIG} --libs
                OUTPUT_VARIABLE MECAB_LIBS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${MECAB_CONFIG} --inc-dir
                OUTPUT_VARIABLE MECAB_INCLUDE_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Find cJSON
find_path(CJSON_INCLUDE_DIR cjson/cJSON.h)
find_library(CJSON_LIBRARY cjson)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${MECAB_INCLUDE_DIR}
    ${CJSON_INCLUDE_DIR}
)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.c"
    "src/*.cpp"
    "src/*.m"
    "src/*.mm"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${MECAB_LIBS}
    ${CURL_LIBRARIES}
    ${CJSON_LIBRARY}
    "-framework Foundation"
    "-framework InputMethodKit"
    "-framework Cocoa"
)

# Compiler flags
separate_arguments(MECAB_CFLAGS_LIST UNIX_COMMAND ${MECAB_CFLAGS})
target_compile_options(${PROJECT_NAME} PRIVATE
    ${MECAB_CFLAGS_LIST}
    -Wall
    -Wextra
)

# Install target
install(TARGETS ${PROJECT_NAME}
    DESTINATION "/Library/Input Methods"
)

# Test target
enable_testing()
add_subdirectory(tests)