#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <mecab.h>

// Morphological analysis result structure
typedef struct {
    char* surface;      // Original text
    char* feature;      // Part-of-speech features
    char* reading;      // Reading (pronunciation)
    char* pronunciation; // Pronunciation
    int start_pos;      // Start position in original text
    int end_pos;        // End position in original text
} MorphNode;

// Analysis result container
typedef struct {
    MorphNode* nodes;
    int node_count;
    int capacity;
} MorphResult;

// Function prototypes
int mecab_init(void);
void mecab_cleanup(void);
MorphResult* analyze_text(const char* text);
void free_morph_result(MorphResult* result);
char* extract_reading(const char* feature);
char* extract_pronunciation(const char* feature);

#endif // MORPHOLOGY_H