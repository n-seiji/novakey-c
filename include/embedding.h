#ifndef EMBEDDING_H
#define EMBEDDING_H

#include <curl/curl.h>
#include <cjson/cJSON.h>

// HTTP response structure
typedef struct {
    char* data;
    size_t size;
} HttpResponse;

// Embedding vector structure
typedef struct {
    float* values;
    int dimensions;
} EmbeddingVector;

// Ollama API client structure
typedef struct {
    CURL* curl;
    char* base_url;
    char* model_name;
} OllamaClient;

// Function prototypes
OllamaClient* ollama_client_create(const char* base_url, const char* model_name);
void ollama_client_destroy(OllamaClient* client);

EmbeddingVector* generate_embedding(OllamaClient* client, const char* text);
void free_embedding_vector(EmbeddingVector* vector);

float calculate_cosine_similarity(const EmbeddingVector* a, const EmbeddingVector* b);

// HTTP utility functions
size_t write_callback(void* contents, size_t size, size_t nmemb, HttpResponse* response);
HttpResponse* http_post_json(const char* url, const char* json_data);
void free_http_response(HttpResponse* response);

#endif // EMBEDDING_H