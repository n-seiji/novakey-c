#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "../../include/embedding.h"

// HTTP response callback
size_t write_callback(void* contents, size_t size, size_t nmemb, HttpResponse* response) {
    size_t real_size = size * nmemb;
    char* ptr = realloc(response->data, response->size + real_size + 1);
    
    if (!ptr) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, real_size);
    response->size += real_size;
    response->data[response->size] = '\0';
    
    return real_size;
}

HttpResponse* http_post_json(const char* url, const char* json_data) {
    CURL* curl;
    CURLcode res;
    HttpResponse* response = malloc(sizeof(HttpResponse));
    
    if (!response) {
        return NULL;
    }
    
    response->data = malloc(1);
    response->size = 0;
    
    curl = curl_easy_init();
    if (!curl) {
        free(response->data);
        free(response);
        return NULL;
    }
    
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(response->data);
        free(response);
        response = NULL;
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return response;
}

void free_http_response(HttpResponse* response) {
    if (response) {
        free(response->data);
        free(response);
    }
}

OllamaClient* ollama_client_create(const char* base_url, const char* model_name) {
    OllamaClient* client = malloc(sizeof(OllamaClient));
    if (!client) {
        return NULL;
    }
    
    // Initialize libcurl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    client->curl = curl_easy_init();
    if (!client->curl) {
        free(client);
        return NULL;
    }
    
    client->base_url = strdup(base_url ? base_url : "http://localhost:11434");
    client->model_name = strdup(model_name ? model_name : "nomic-embed-text");
    
    printf("Ollama client created (URL: %s, Model: %s)\n", 
           client->base_url, client->model_name);
    
    return client;
}

void ollama_client_destroy(OllamaClient* client) {
    if (!client) return;
    
    if (client->curl) {
        curl_easy_cleanup(client->curl);
    }
    
    free(client->base_url);
    free(client->model_name);
    free(client);
    
    curl_global_cleanup();
    printf("Ollama client destroyed\n");
}

EmbeddingVector* generate_embedding(OllamaClient* client, const char* text) {
    if (!client || !text) {
        return NULL;
    }
    
    // Create JSON request
    cJSON* json = cJSON_CreateObject();
    cJSON* model = cJSON_CreateString(client->model_name);
    cJSON* prompt = cJSON_CreateString(text);
    
    cJSON_AddItemToObject(json, "model", model);
    cJSON_AddItemToObject(json, "prompt", prompt);
    
    char* json_string = cJSON_Print(json);
    if (!json_string) {
        cJSON_Delete(json);
        return NULL;
    }
    
    // Build URL
    char url[512];
    snprintf(url, sizeof(url), "%s/api/embeddings", client->base_url);
    
    // Make HTTP request
    HttpResponse* response = http_post_json(url, json_string);
    
    free(json_string);
    cJSON_Delete(json);
    
    if (!response) {
        printf("Failed to get response from Ollama API\n");
        return NULL;
    }
    
    // Parse response JSON
    cJSON* response_json = cJSON_Parse(response->data);
    if (!response_json) {
        printf("Failed to parse response JSON\n");
        free_http_response(response);
        return NULL;
    }
    
    // Extract embedding array
    cJSON* embedding_json = cJSON_GetObjectItem(response_json, "embedding");
    if (!cJSON_IsArray(embedding_json)) {
        printf("No embedding array found in response\n");
        cJSON_Delete(response_json);
        free_http_response(response);
        return NULL;
    }
    
    int dimensions = cJSON_GetArraySize(embedding_json);
    if (dimensions <= 0) {
        printf("Empty embedding array\n");
        cJSON_Delete(response_json);
        free_http_response(response);
        return NULL;
    }
    
    // Create embedding vector
    EmbeddingVector* vector = malloc(sizeof(EmbeddingVector));
    if (!vector) {
        cJSON_Delete(response_json);
        free_http_response(response);
        return NULL;
    }
    
    vector->dimensions = dimensions;
    vector->values = malloc(sizeof(float) * dimensions);
    if (!vector->values) {
        free(vector);
        cJSON_Delete(response_json);
        free_http_response(response);
        return NULL;
    }
    
    // Copy values
    for (int i = 0; i < dimensions; i++) {
        cJSON* value = cJSON_GetArrayItem(embedding_json, i);
        if (cJSON_IsNumber(value)) {
            vector->values[i] = (float)cJSON_GetNumberValue(value);
        } else {
            vector->values[i] = 0.0f;
        }
    }
    
    cJSON_Delete(response_json);
    free_http_response(response);
    
    printf("Generated embedding with %d dimensions for text: '%.50s...'\n", 
           dimensions, text);
    
    return vector;
}

void free_embedding_vector(EmbeddingVector* vector) {
    if (vector) {
        free(vector->values);
        free(vector);
    }
}

float calculate_cosine_similarity(const EmbeddingVector* a, const EmbeddingVector* b) {
    if (!a || !b || a->dimensions != b->dimensions) {
        return 0.0f;
    }
    
    float dot_product = 0.0f;
    float norm_a = 0.0f;
    float norm_b = 0.0f;
    
    for (int i = 0; i < a->dimensions; i++) {
        dot_product += a->values[i] * b->values[i];
        norm_a += a->values[i] * a->values[i];
        norm_b += b->values[i] * b->values[i];
    }
    
    norm_a = sqrtf(norm_a);
    norm_b = sqrtf(norm_b);
    
    if (norm_a == 0.0f || norm_b == 0.0f) {
        return 0.0f;
    }
    
    return dot_product / (norm_a * norm_b);
}