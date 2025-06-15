#ifndef SEARCH_H
#define SEARCH_H

#include "morphology.h"
#include "embedding.h"
#include "novakey_core.h"

// Search configuration
typedef struct {
    float embedding_weight;    // Weight for embedding similarity (0.0 - 1.0)
    float phonetic_weight;     // Weight for phonetic similarity (0.0 - 1.0)
    int max_candidates;        // Maximum number of candidates to return
    char* dictionary_path;     // Path to candidate dictionary
} SearchConfig;

// Dictionary entry structure
typedef struct {
    char* kanji;              // Kanji representation
    char* hiragana;           // Hiragana reading
    char* katakana;           // Katakana reading
    char* romaji;             // Romaji reading
    float frequency;          // Usage frequency score
} DictionaryEntry;

// Dictionary container
typedef struct {
    DictionaryEntry* entries;
    int entry_count;
    int capacity;
} Dictionary;

// Candidate scoring result
typedef struct {
    NovaKeyCandidate* candidates;
    int candidate_count;
    int capacity;
} CandidateList;

// Function prototypes
SearchConfig* create_search_config(void);
void free_search_config(SearchConfig* config);

Dictionary* load_dictionary(const char* path);
void free_dictionary(Dictionary* dict);

CandidateList* search_candidates(const char* input_text, 
                                const MorphResult* morph_result,
                                const Dictionary* dict,
                                const SearchConfig* config,
                                OllamaClient* ollama_client);

void free_candidate_list(CandidateList* candidates);

// Scoring functions
float calculate_phonetic_similarity(const char* input_reading, const char* candidate_reading);
float calculate_edit_distance_score(const char* a, const char* b);
float calculate_combined_score(float embedding_score, float phonetic_score, 
                              float frequency_score, const SearchConfig* config);

// Utility functions
void sort_candidates_by_score(CandidateList* candidates);
char* romanize_hiragana(const char* hiragana);

#endif // SEARCH_H