# Test configuration
add_executable(test_morphology test_morphology.c ../src/morphology/mecab_wrapper.c)

# Link MeCab for morphology tests
separate_arguments(MECAB_LIBS_LIST UNIX_COMMAND ${MECAB_LIBS})
target_link_libraries(test_morphology ${MECAB_LIBS_LIST})

# Include directories for tests
target_include_directories(test_morphology PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${MECAB_INCLUDE_DIR}
)

# Compiler flags for tests
separate_arguments(MECAB_CFLAGS_LIST UNIX_COMMAND ${MECAB_CFLAGS})
target_compile_options(test_morphology PRIVATE ${MECAB_CFLAGS_LIST})

# Add test
add_test(NAME morphology_test COMMAND test_morphology)