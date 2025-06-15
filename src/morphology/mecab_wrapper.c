#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mecab.h>
#include "../../include/morphology.h"

static mecab_t* mecab = NULL;

int mecab_init(void) {
    // Initialize MeCab with default settings
    mecab = mecab_new2("");
    if (!mecab) {
        fprintf(stderr, "Failed to initialize MeCab: %s\n", mecab_strerror(NULL));
        return -1;
    }
    
    printf("MeCab initialized successfully\n");
    return 0;
}

void mecab_cleanup(void) {
    if (mecab) {
        mecab_destroy(mecab);
        mecab = NULL;
        printf("MeCab cleaned up\n");
    }
}

MorphResult* analyze_text(const char* text) {
    if (!mecab || !text) {
        return NULL;
    }
    
    // Parse text with MeCab
    const char* result = mecab_sparse_tostr(mecab, text);
    if (!result) {
        fprintf(stderr, "MeCab parsing failed: %s\n", mecab_strerror(mecab));
        return NULL;
    }
    
    // Create result structure
    MorphResult* morph_result = malloc(sizeof(MorphResult));
    if (!morph_result) {
        return NULL;
    }
    
    morph_result->capacity = 32;
    morph_result->node_count = 0;
    morph_result->nodes = malloc(sizeof(MorphNode) * morph_result->capacity);
    if (!morph_result->nodes) {
        free(morph_result);
        return NULL;
    }
    
    // Parse MeCab output line by line
    char* result_copy = strdup(result);
    char* line = strtok(result_copy, "\n");
    int pos = 0;
    
    while (line && strcmp(line, "EOS") != 0) {
        // Expand array if needed
        if (morph_result->node_count >= morph_result->capacity) {
            morph_result->capacity *= 2;
            morph_result->nodes = realloc(morph_result->nodes, 
                                        sizeof(MorphNode) * morph_result->capacity);
            if (!morph_result->nodes) {
                free(result_copy);
                free_morph_result(morph_result);
                return NULL;
            }
        }
        
        // Parse line: "surface\tfeature"
        char* tab_pos = strchr(line, '\t');
        if (tab_pos) {
            *tab_pos = '\0';
            char* surface = line;
            char* feature = tab_pos + 1;
            
            MorphNode* node = &morph_result->nodes[morph_result->node_count];
            
            // Copy surface
            node->surface = strdup(surface);
            node->feature = strdup(feature);
            
            // Extract reading and pronunciation from feature
            node->reading = extract_reading(feature);
            node->pronunciation = extract_pronunciation(feature);
            
            // Set positions
            node->start_pos = pos;
            node->end_pos = pos + strlen(surface);
            pos = node->end_pos;
            
            morph_result->node_count++;
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(result_copy);
    return morph_result;
}

void free_morph_result(MorphResult* result) {
    if (!result) return;
    
    for (int i = 0; i < result->node_count; i++) {
        free(result->nodes[i].surface);
        free(result->nodes[i].feature);
        free(result->nodes[i].reading);
        free(result->nodes[i].pronunciation);
    }
    
    free(result->nodes);
    free(result);
}

char* extract_reading(const char* feature) {
    if (!feature) return NULL;
    
    // Feature format: "品詞,品詞細分類1,品詞細分類2,品詞細分類3,活用型,活用形,原形,読み,発音"
    // Reading is the 8th field (index 7)
    char* feature_copy = strdup(feature);
    char* field = strtok(feature_copy, ",");
    int field_index = 0;
    
    while (field && field_index < 7) {
        field = strtok(NULL, ",");
        field_index++;
    }
    
    char* reading = NULL;
    if (field && strcmp(field, "*") != 0) {
        reading = strdup(field);
    }
    
    free(feature_copy);
    return reading;
}

char* extract_pronunciation(const char* feature) {
    if (!feature) return NULL;
    
    // Pronunciation is the 9th field (index 8)
    char* feature_copy = strdup(feature);
    char* field = strtok(feature_copy, ",");
    int field_index = 0;
    
    while (field && field_index < 8) {
        field = strtok(NULL, ",");
        field_index++;
    }
    
    char* pronunciation = NULL;
    if (field && strcmp(field, "*") != 0) {
        pronunciation = strdup(field);
    }
    
    free(feature_copy);
    return pronunciation;
}