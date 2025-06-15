#ifndef NOVAKEY_CORE_H
#define NOVAKEY_CORE_H

// Constants
#define OLLAMA_DEFAULT_URL "http://localhost:11434"

// Input modes
typedef enum {
    NovaKeyInputModeEnglish = 0,
    NovaKeyInputModeJapanese = 1
} NovaKeyInputMode;

// Candidate structure
typedef struct {
    char* text;
    char* reading;
    float embedding_score;
    float phonetic_score;
    float combined_score;
} NovaKeyCandidate;

#endif // NOVAKEY_CORE_H