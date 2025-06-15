# Test configuration

# Morphology tests
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

# Embedding tests
add_executable(test_embedding test_embedding.c ../src/embedding/ollama_client.c)

# Link libraries for embedding tests
target_link_libraries(test_embedding
    ${CURL_LIBRARIES}
    ${CJSON_LIBRARY}
    "-framework Foundation"
)

# Include directories for embedding tests
target_include_directories(test_embedding PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CJSON_INCLUDE_DIR}
)

# Integration tests
add_executable(test_integration test_integration.c 
    ../src/morphology/mecab_wrapper.c
    ../src/embedding/ollama_client.c
    ../src/search/candidate_search.c
    ../src/utils/config.c
)

# Link libraries for integration tests
separate_arguments(MECAB_LIBS_LIST UNIX_COMMAND ${MECAB_LIBS})
target_link_libraries(test_integration
    ${MECAB_LIBS_LIST}
    ${CURL_LIBRARIES}
    ${CJSON_LIBRARY}
    "-framework Foundation"
)

# Include directories for integration tests
target_include_directories(test_integration PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${MECAB_INCLUDE_DIR}
    ${CJSON_INCLUDE_DIR}
)

# Compiler flags for integration tests
separate_arguments(MECAB_CFLAGS_LIST UNIX_COMMAND ${MECAB_CFLAGS})
target_compile_options(test_integration PRIVATE ${MECAB_CFLAGS_LIST})

# Add tests
add_test(NAME morphology_test COMMAND test_morphology)
add_test(NAME embedding_test COMMAND test_embedding)
add_test(NAME integration_test COMMAND test_integration)