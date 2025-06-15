#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/embedding.h"

void test_ollama_client_create() {
    printf("Testing Ollama client creation...\n");
    
    OllamaClient* client = ollama_client_create("http://localhost:11434", "nomic-embed-text");
    assert(client != NULL);
    assert(client->base_url != NULL);
    assert(client->model_name != NULL);
    assert(strcmp(client->base_url, "http://localhost:11434") == 0);
    assert(strcmp(client->model_name, "nomic-embed-text") == 0);
    
    printf("✓ Ollama client created successfully\n");
    
    ollama_client_destroy(client);
    printf("✓ Ollama client destroyed successfully\n");
}

void test_embedding_generation() {
    printf("Testing embedding generation...\n");
    
    OllamaClient* client = ollama_client_create("http://localhost:11434", "nomic-embed-text");
    assert(client != NULL);
    
    // Test with Japanese text
    const char* japanese_text = "こんにちは";
    EmbeddingVector* vector = generate_embedding(client, japanese_text);
    
    if (vector != NULL) {
        printf("✓ Japanese embedding generated: %d dimensions\n", vector->dimensions);
        assert(vector->dimensions > 0);
        assert(vector->values != NULL);
        
        // Print first few values for verification
        printf("  First 5 values: ");
        for (int i = 0; i < 5 && i < vector->dimensions; i++) {
            printf("%.3f ", vector->values[i]);
        }
        printf("\n");
        
        free_embedding_vector(vector);
    } else {
        printf("⚠ Embedding generation failed (possibly Ollama not running)\n");
    }
    
    ollama_client_destroy(client);
}

void test_cosine_similarity() {
    printf("Testing cosine similarity calculation...\n");
    
    // Create test vectors
    EmbeddingVector vec1 = {0};
    EmbeddingVector vec2 = {0};
    
    vec1.dimensions = 3;
    vec1.values = malloc(sizeof(float) * 3);
    vec1.values[0] = 1.0f;
    vec1.values[1] = 0.0f;
    vec1.values[2] = 0.0f;
    
    vec2.dimensions = 3;
    vec2.values = malloc(sizeof(float) * 3);
    vec2.values[0] = 1.0f;
    vec2.values[1] = 0.0f;
    vec2.values[2] = 0.0f;
    
    // Identical vectors should have similarity 1.0
    float similarity = calculate_cosine_similarity(&vec1, &vec2);
    printf("✓ Identical vectors similarity: %.3f\n", similarity);
    assert(similarity > 0.99f);
    
    // Orthogonal vectors
    vec2.values[0] = 0.0f;
    vec2.values[1] = 1.0f;
    vec2.values[2] = 0.0f;
    
    similarity = calculate_cosine_similarity(&vec1, &vec2);
    printf("✓ Orthogonal vectors similarity: %.3f\n", similarity);
    assert(similarity < 0.01f && similarity > -0.01f);
    
    free(vec1.values);
    free(vec2.values);
}

void test_embedding_comparison() {
    printf("Testing embedding comparison with real data...\n");
    
    OllamaClient* client = ollama_client_create("http://localhost:11434", "nomic-embed-text");
    if (!client) {
        printf("⚠ Cannot create Ollama client\n");
        return;
    }
    
    // Generate embeddings for similar and different text
    EmbeddingVector* hello1 = generate_embedding(client, "こんにちは");
    EmbeddingVector* hello2 = generate_embedding(client, "おはよう");
    EmbeddingVector* different = generate_embedding(client, "さようなら");
    
    if (hello1 && hello2 && different) {
        float sim_greetings = calculate_cosine_similarity(hello1, hello2);
        float sim_different = calculate_cosine_similarity(hello1, different);
        
        printf("✓ こんにちは vs おはよう similarity: %.3f\n", sim_greetings);
        printf("✓ こんにちは vs さようなら similarity: %.3f\n", sim_different);
        
        // Greetings should be more similar than greeting vs farewell
        if (sim_greetings > sim_different) {
            printf("✓ Semantic similarity working as expected\n");
        } else {
            printf("⚠ Unexpected similarity results\n");
        }
        
        free_embedding_vector(hello1);
        free_embedding_vector(hello2);
        free_embedding_vector(different);
    } else {
        printf("⚠ Could not generate embeddings for comparison\n");
    }
    
    ollama_client_destroy(client);
}

int main() {
    printf("=== NovaKey Embedding Tests ===\n\n");
    
    test_ollama_client_create();
    test_cosine_similarity();
    test_embedding_generation();
    test_embedding_comparison();
    
    printf("\n✓ All embedding tests completed!\n");
    return 0;
}