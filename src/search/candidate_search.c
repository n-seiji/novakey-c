#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../include/search.h"

Dictionary* create_fallback_dictionary(void);

SearchConfig* create_search_config(void) {
    SearchConfig* config = malloc(sizeof(SearchConfig));
    if (!config) {
        return NULL;
    }
    
    // Default configuration
    config->embedding_weight = 0.6f;
    config->phonetic_weight = 0.4f;
    config->max_candidates = 10;
    config->dictionary_path = strdup("resources/dictionary.txt");
    
    return config;
}

void free_search_config(SearchConfig* config) {
    if (config) {
        free(config->dictionary_path);
        free(config);
    }
}

Dictionary* load_dictionary(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Warning: Could not load dictionary from %s, using basic fallback\n", path);
        return create_fallback_dictionary();
    }
    
    Dictionary* dict = malloc(sizeof(Dictionary));
    if (!dict) {
        fclose(file);
        return NULL;
    }
    
    dict->capacity = 1000;
    dict->entry_count = 0;
    dict->entries = malloc(sizeof(DictionaryEntry) * dict->capacity);
    if (!dict->entries) {
        free(dict);
        fclose(file);
        return NULL;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file) && dict->entry_count < dict->capacity) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        // Parse line format: "kanji,hiragana,katakana,romaji,frequency"
        char* kanji = strtok(line, ",");
        char* hiragana = strtok(NULL, ",");
        char* katakana = strtok(NULL, ",");
        char* romaji = strtok(NULL, ",");
        char* freq_str = strtok(NULL, ",");
        
        if (kanji && hiragana && katakana && romaji && freq_str) {
            DictionaryEntry* entry = &dict->entries[dict->entry_count];
            entry->kanji = strdup(kanji);
            entry->hiragana = strdup(hiragana);
            entry->katakana = strdup(katakana);
            entry->romaji = strdup(romaji);
            entry->frequency = atof(freq_str);
            dict->entry_count++;
        }
    }
    
    fclose(file);
    printf("Loaded dictionary with %d entries\n", dict->entry_count);
    return dict;
}

Dictionary* create_fallback_dictionary(void) {
    Dictionary* dict = malloc(sizeof(Dictionary));
    if (!dict) return NULL;
    
    dict->capacity = 10;
    dict->entry_count = 5;
    dict->entries = malloc(sizeof(DictionaryEntry) * dict->capacity);
    if (!dict->entries) {
        free(dict);
        return NULL;
    }
    
    // Basic fallback entries
    const char* fallback_data[][5] = {
        {"こんにちは", "こんにちは", "コンニチハ", "konnichiwa", "1.0"},
        {"ありがとう", "ありがとう", "アリガトウ", "arigatou", "0.9"},
        {"さようなら", "さようなら", "サヨウナラ", "sayounara", "0.8"},
        {"おはよう", "おはよう", "オハヨウ", "ohayou", "0.85"},
        {"こんばんは", "こんばんは", "コンバンハ", "konbanwa", "0.75"}
    };
    
    for (int i = 0; i < dict->entry_count; i++) {
        DictionaryEntry* entry = &dict->entries[i];
        entry->kanji = strdup(fallback_data[i][0]);
        entry->hiragana = strdup(fallback_data[i][1]);
        entry->katakana = strdup(fallback_data[i][2]);
        entry->romaji = strdup(fallback_data[i][3]);
        entry->frequency = atof(fallback_data[i][4]);
    }
    
    printf("Created fallback dictionary with %d entries\n", dict->entry_count);
    return dict;
}

void free_dictionary(Dictionary* dict) {
    if (!dict) return;
    
    for (int i = 0; i < dict->entry_count; i++) {
        free(dict->entries[i].kanji);
        free(dict->entries[i].hiragana);
        free(dict->entries[i].katakana);
        free(dict->entries[i].romaji);
    }
    
    free(dict->entries);
    free(dict);
}

CandidateList* search_candidates(const char* input_text,
                                const MorphResult* morph_result,
                                const Dictionary* dict,
                                const SearchConfig* config,
                                OllamaClient* ollama_client) {
    if (!input_text || !dict || !config) {
        return NULL;
    }
    
    CandidateList* candidates = malloc(sizeof(CandidateList));
    if (!candidates) return NULL;
    
    candidates->capacity = config->max_candidates;
    candidates->candidate_count = 0;
    candidates->candidates = malloc(sizeof(NovaKeyCandidate) * candidates->capacity);
    if (!candidates->candidates) {
        free(candidates);
        return NULL;
    }
    
    // Generate embedding for input text
    EmbeddingVector* input_embedding = NULL;
    if (ollama_client) {
        input_embedding = generate_embedding(ollama_client, input_text);
    }
    
    // Search through dictionary entries
    for (int i = 0; i < dict->entry_count && candidates->candidate_count < config->max_candidates; i++) {
        DictionaryEntry* entry = &dict->entries[i];
        
        // Calculate phonetic similarity
        float phonetic_score = calculate_phonetic_similarity(input_text, entry->hiragana);
        
        // Calculate embedding similarity
        float embedding_score = 0.0f;
        if (input_embedding && ollama_client) {
            EmbeddingVector* entry_embedding = generate_embedding(ollama_client, entry->kanji);
            if (entry_embedding) {
                embedding_score = calculate_cosine_similarity(input_embedding, entry_embedding);
                free_embedding_vector(entry_embedding);
            }
        }
        
        // Calculate combined score
        float combined_score = calculate_combined_score(embedding_score, phonetic_score, 
                                                       entry->frequency, config);
        
        // Only add candidates with reasonable scores
        if (combined_score > 0.1f) {
            NovaKeyCandidate* candidate = &candidates->candidates[candidates->candidate_count];
            candidate->text = strdup(entry->kanji);
            candidate->reading = strdup(entry->hiragana);
            candidate->embedding_score = embedding_score;
            candidate->phonetic_score = phonetic_score;
            candidate->combined_score = combined_score;
            
            candidates->candidate_count++;
        }
    }
    
    // Sort candidates by combined score
    sort_candidates_by_score(candidates);
    
    if (input_embedding) {
        free_embedding_vector(input_embedding);
    }
    
    printf("Found %d candidates for input: %s\n", candidates->candidate_count, input_text);
    return candidates;
}

void free_candidate_list(CandidateList* candidates) {
    if (!candidates) return;
    
    for (int i = 0; i < candidates->candidate_count; i++) {
        free(candidates->candidates[i].text);
        free(candidates->candidates[i].reading);
    }
    
    free(candidates->candidates);
    free(candidates);
}

float calculate_phonetic_similarity(const char* input_reading, const char* candidate_reading) {
    if (!input_reading || !candidate_reading) {
        return 0.0f;
    }
    
    // Use edit distance for phonetic similarity
    return calculate_edit_distance_score(input_reading, candidate_reading);
}

float calculate_edit_distance_score(const char* a, const char* b) {
    int len_a = strlen(a);
    int len_b = strlen(b);
    
    if (len_a == 0) return len_b == 0 ? 1.0f : 0.0f;
    if (len_b == 0) return 0.0f;
    
    // Simple implementation - can be optimized with dynamic programming
    int max_len = (len_a > len_b) ? len_a : len_b;
    int common = 0;
    
    // Count common characters (simplified)
    for (int i = 0; i < len_a && i < len_b; i++) {
        if (a[i] == b[i]) {
            common++;
        }
    }
    
    return (float)common / (float)max_len;
}

float calculate_combined_score(float embedding_score, float phonetic_score,
                              float frequency_score, const SearchConfig* config) {
    float weighted_embedding = embedding_score * config->embedding_weight;
    float weighted_phonetic = phonetic_score * config->phonetic_weight;
    float frequency_factor = 1.0f + (frequency_score * 0.1f); // Small frequency boost
    
    return (weighted_embedding + weighted_phonetic) * frequency_factor;
}

void sort_candidates_by_score(CandidateList* candidates) {
    if (!candidates || candidates->candidate_count <= 1) {
        return;
    }
    
    // Simple bubble sort - can be optimized
    for (int i = 0; i < candidates->candidate_count - 1; i++) {
        for (int j = 0; j < candidates->candidate_count - 1 - i; j++) {
            if (candidates->candidates[j].combined_score < 
                candidates->candidates[j + 1].combined_score) {
                
                // Swap candidates
                NovaKeyCandidate temp = candidates->candidates[j];
                candidates->candidates[j] = candidates->candidates[j + 1];
                candidates->candidates[j + 1] = temp;
            }
        }
    }
}