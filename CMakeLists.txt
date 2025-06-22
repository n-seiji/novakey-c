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

# Create executable as App Bundle
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})

# Set App Bundle properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist
    MACOSX_BUNDLE_BUNDLE_NAME "NovaKey"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    MACOSX_BUNDLE_IDENTIFIER "com.novakey.inputmethod"
    OUTPUT_NAME "NovaKey"
)

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

# Code signing (optional - for development)
# Uncomment and modify the following lines if you have a developer certificate
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application: Your Name"
#     XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
# )

# Custom command to copy resources to app bundle
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources
    COMMENT "Copying resources to app bundle"
)

# Install target
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION "/Library/Input Methods"
)

# Test target
enable_testing()
add_subdirectory(tests)