#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/morphology.h"

void test_mecab_init() {
    printf("Testing MeCab initialization...\n");
    int result = mecab_init();
    assert(result == 0);
    printf("✓ MeCab initialization successful\n");
}

void test_basic_analysis() {
    printf("Testing basic morphological analysis...\n");
    
    const char* test_text = "こんにちは";
    MorphResult* result = analyze_text(test_text);
    
    assert(result != NULL);
    assert(result->node_count > 0);
    
    printf("✓ Analysis completed: %d nodes found\n", result->node_count);
    
    for (int i = 0; i < result->node_count; i++) {
        printf("  Node %d: '%s' (feature: %s)\n", 
               i, result->nodes[i].surface, result->nodes[i].feature);
    }
    
    free_morph_result(result);
}

void test_cleanup() {
    printf("Testing MeCab cleanup...\n");
    mecab_cleanup();
    printf("✓ MeCab cleanup completed\n");
}

int main() {
    printf("=== NovaKey Morphology Tests ===\n\n");
    
    test_mecab_init();
    test_basic_analysis();
    test_cleanup();
    
    printf("\n✓ All morphology tests passed!\n");
    return 0;
}