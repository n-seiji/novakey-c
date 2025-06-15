#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/morphology.h"
#include "../include/embedding.h"
#include "../include/search.h"

void test_end_to_end_search() {
    printf("Testing end-to-end candidate search...\n");
    
    // Initialize MeCab
    int mecab_result = mecab_init();
    if (mecab_result != 0) {
        printf("⚠ MeCab initialization failed, skipping test\n");
        return;
    }
    
    // Create Ollama client
    OllamaClient* ollama_client = ollama_client_create("http://localhost:11434", "nomic-embed-text");
    if (!ollama_client) {
        printf("⚠ Could not create Ollama client, skipping test\n");
        mecab_cleanup();
        return;
    }
    
    // Create search configuration
    SearchConfig* config = create_search_config();
    assert(config != NULL);
    printf("✓ Search configuration created\n");
    
    // Load dictionary (will use fallback if file doesn't exist)
    Dictionary* dict = load_dictionary(config->dictionary_path);
    assert(dict != NULL);
    printf("✓ Dictionary loaded with %d entries\n", dict->entry_count);
    
    // Test input
    const char* input_text = "こんにちは";
    
    // Perform morphological analysis
    MorphResult* morph_result = analyze_text(input_text);
    if (morph_result) {
        printf("✓ Morphological analysis completed: %d nodes\n", morph_result->node_count);
    }
    
    // Search for candidates
    CandidateList* candidates = search_candidates(input_text, morph_result, dict, config, ollama_client);
    
    if (candidates && candidates->candidate_count > 0) {
        printf("✓ Found %d candidates for '%s':\n", candidates->candidate_count, input_text);
        
        for (int i = 0; i < candidates->candidate_count && i < 5; i++) {
            NovaKeyCandidate* candidate = &candidates->candidates[i];
            printf("  %d. %s (reading: %s, combined_score: %.3f)\n", 
                   i + 1, candidate->text, candidate->reading, candidate->combined_score);
            printf("     embedding: %.3f, phonetic: %.3f\n",
                   candidate->embedding_score, candidate->phonetic_score);
        }
        
        free_candidate_list(candidates);
    } else {
        printf("⚠ No candidates found or search failed\n");
    }
    
    // Cleanup
    if (morph_result) {
        free_morph_result(morph_result);
    }
    free_dictionary(dict);
    free_search_config(config);
    ollama_client_destroy(ollama_client);
    mecab_cleanup();
    
    printf("✓ End-to-end test completed\n");
}

void test_multiple_inputs() {
    printf("Testing multiple input patterns...\n");
    
    // Initialize systems
    int mecab_result = mecab_init();
    if (mecab_result != 0) {
        printf("⚠ MeCab initialization failed, skipping test\n");
        return;
    }
    
    OllamaClient* ollama_client = ollama_client_create("http://localhost:11434", "nomic-embed-text");
    if (!ollama_client) {
        printf("⚠ Could not create Ollama client, skipping test\n");
        mecab_cleanup();
        return;
    }
    
    SearchConfig* config = create_search_config();
    Dictionary* dict = load_dictionary(config->dictionary_path);
    
    // Test different inputs
    const char* test_inputs[] = {
        "こんにちは",
        "ありがとう", 
        "おはよう",
        "konnichiwa"  // Test romaji input
    };
    
    int num_tests = sizeof(test_inputs) / sizeof(test_inputs[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("\n--- Testing input: '%s' ---\n", test_inputs[i]);
        
        MorphResult* morph_result = analyze_text(test_inputs[i]);
        CandidateList* candidates = search_candidates(test_inputs[i], morph_result, 
                                                    dict, config, ollama_client);
        
        if (candidates && candidates->candidate_count > 0) {
            printf("Found %d candidates:\n", candidates->candidate_count);
            
            // Show top 3 candidates
            for (int j = 0; j < candidates->candidate_count && j < 3; j++) {
                NovaKeyCandidate* candidate = &candidates->candidates[j];
                printf("  %d. %s (score: %.3f)\n", 
                       j + 1, candidate->text, candidate->combined_score);
            }
            
            free_candidate_list(candidates);
        } else {
            printf("No candidates found\n");
        }
        
        if (morph_result) {
            free_morph_result(morph_result);
        }
    }
    
    // Cleanup
    free_dictionary(dict);
    free_search_config(config);
    ollama_client_destroy(ollama_client);
    mecab_cleanup();
    
    printf("\n✓ Multiple input test completed\n");
}

void test_config_weights() {
    printf("Testing different weight configurations...\n");
    
    // Test with different embedding vs phonetic weights
    SearchConfig* config1 = create_search_config();
    config1->embedding_weight = 0.8f;
    config1->phonetic_weight = 0.2f;
    
    SearchConfig* config2 = create_search_config();
    config2->embedding_weight = 0.2f;
    config2->phonetic_weight = 0.8f;
    
    printf("✓ Config 1: embedding=%.1f, phonetic=%.1f\n", 
           config1->embedding_weight, config1->phonetic_weight);
    printf("✓ Config 2: embedding=%.1f, phonetic=%.1f\n", 
           config2->embedding_weight, config2->phonetic_weight);
    
    free_search_config(config1);
    free_search_config(config2);
    
    printf("✓ Weight configuration test completed\n");
}

int main() {
    printf("=== NovaKey Integration Tests ===\n\n");
    
    test_config_weights();
    test_end_to_end_search();
    test_multiple_inputs();
    
    printf("\n✓ All integration tests completed!\n");
    return 0;
}