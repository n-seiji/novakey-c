#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

typedef struct NovaKeyConfig {
    char* ollama_url;
    char* embedding_model;
    char* dictionary_path;
    float embedding_weight;
    float phonetic_weight;
    int max_candidates;
    int debug_logging;
} NovaKeyConfig;

// Forward declarations
NovaKeyConfig* create_default_config(void);

static NovaKeyConfig* global_config = NULL;

NovaKeyConfig* load_config(const char* config_path) {
    FILE* file = fopen(config_path, "r");
    if (!file) {
        printf("Warning: Could not load config from %s, using defaults\n", config_path);
        return create_default_config();
    }
    
    // Read file content
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return create_default_config();
    }
    
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    // Parse JSON
    cJSON* json = cJSON_Parse(content);
    free(content);
    
    if (!json) {
        printf("Warning: Invalid JSON in config file, using defaults\n");
        return create_default_config();
    }
    
    NovaKeyConfig* config = malloc(sizeof(NovaKeyConfig));
    if (!config) {
        cJSON_Delete(json);
        return create_default_config();
    }
    
    // Extract configuration values
    cJSON* ollama_url = cJSON_GetObjectItem(json, "ollama_url");
    config->ollama_url = strdup(cJSON_IsString(ollama_url) ? 
                                cJSON_GetStringValue(ollama_url) : "http://localhost:11434");
    
    cJSON* embedding_model = cJSON_GetObjectItem(json, "embedding_model");
    config->embedding_model = strdup(cJSON_IsString(embedding_model) ? 
                                     cJSON_GetStringValue(embedding_model) : "nomic-embed-text");
    
    cJSON* dictionary_path = cJSON_GetObjectItem(json, "dictionary_path");
    config->dictionary_path = strdup(cJSON_IsString(dictionary_path) ? 
                                     cJSON_GetStringValue(dictionary_path) : "resources/dictionary.txt");
    
    cJSON* embedding_weight = cJSON_GetObjectItem(json, "embedding_weight");
    config->embedding_weight = cJSON_IsNumber(embedding_weight) ? 
                               (float)cJSON_GetNumberValue(embedding_weight) : 0.6f;
    
    cJSON* phonetic_weight = cJSON_GetObjectItem(json, "phonetic_weight");
    config->phonetic_weight = cJSON_IsNumber(phonetic_weight) ? 
                              (float)cJSON_GetNumberValue(phonetic_weight) : 0.4f;
    
    cJSON* max_candidates = cJSON_GetObjectItem(json, "max_candidates");
    config->max_candidates = cJSON_IsNumber(max_candidates) ? 
                             cJSON_GetNumberValue(max_candidates) : 10;
    
    cJSON* debug_logging = cJSON_GetObjectItem(json, "debug_logging");
    config->debug_logging = cJSON_IsBool(debug_logging) ? 
                            cJSON_IsTrue(debug_logging) : 1;
    
    cJSON_Delete(json);
    printf("Loaded configuration from %s\n", config_path);
    return config;
}

NovaKeyConfig* create_default_config(void) {
    NovaKeyConfig* config = malloc(sizeof(NovaKeyConfig));
    if (!config) return NULL;
    
    config->ollama_url = strdup("http://localhost:11434");
    config->embedding_model = strdup("nomic-embed-text");
    config->dictionary_path = strdup("resources/dictionary.txt");
    config->embedding_weight = 0.6f;
    config->phonetic_weight = 0.4f;
    config->max_candidates = 10;
    config->debug_logging = 1;
    
    printf("Created default configuration\n");
    return config;
}

void free_config(NovaKeyConfig* config) {
    if (!config) return;
    
    free(config->ollama_url);
    free(config->embedding_model);
    free(config->dictionary_path);
    free(config);
}

NovaKeyConfig* get_global_config(void) {
    if (!global_config) {
        global_config = load_config("resources/config.json");
    }
    return global_config;
}

void cleanup_global_config(void) {
    if (global_config) {
        free_config(global_config);
        global_config = NULL;
    }
}