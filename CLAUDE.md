# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

NovaKey is a macOS-only Input Method Editor (IME) that provides intelligent Japanese text input using:
- English/Japanese input switching (Shift+Ctrl+Space)
- Morphological analysis via MeCab
- LLM embeddings via Ollama API
- Smart candidate ranking combining embedding similarity and phonetic matching
- RAG-like search architecture similar to Dify

## Build Commands

### Prerequisites
Install dependencies via Homebrew:
```bash
brew install mecab mecab-ipadic libcurl cjson cmake pkg-config
```

### Build Commands
```bash
# Configure and build
mkdir build && cd build
cmake ..
make

# Install IME to system
sudo make install

# Run tests
make test
```

### Development Build
```bash
# Debug build with verbose output
cmake -DCMAKE_BUILD_TYPE=Debug ..
make VERBOSE=1
```

## Architecture

### Core Components

**IME Layer (`src/ime/`)**
- `NovaKeyServer.m` - IMKServer implementation for system integration
- `NovaKeyController.m` - IMKInputController for input processing
- Handles macOS InputMethodKit integration and keyboard events

**Morphological Analysis (`src/morphology/`)**
- `mecab_wrapper.c` - MeCab integration for Japanese morphological analysis
- Tokenizes input for better conversion candidate generation

**Embedding Service (`src/embedding/`)**
- `ollama_client.c` - HTTP client for Ollama API communication
- `embedding_processor.c` - Processes text embeddings for semantic matching
- Uses libcurl for HTTP requests, cJSON for JSON parsing

**Search Engine (`src/search/`)**
- `candidate_search.c` - Combines phonetic and semantic matching
- `ranking_algorithm.c` - Scores candidates using embedding + phonetic weights
- Implements RAG-like retrieval and ranking

**Utilities (`src/utils/`)**
- `json_utils.c` - JSON parsing helpers
- `string_utils.c` - String manipulation utilities
- `config.c` - Configuration management

### Dependencies
- **InputMethodKit** - macOS IME framework
- **MeCab** - Japanese morphological analyzer (C library)
- **libcurl** - HTTP client for Ollama API
- **cJSON** - JSON parsing library
- **Foundation** - macOS system framework

### Key Files
- `CMakeLists.txt` - Build configuration with all required frameworks
- `include/` - Header files for all components
- `resources/` - Configuration files and dictionaries
- `tests/` - Unit tests for core functionality

## Development Notes

### IME Installation
IMEs must be installed to `/Library/Input Methods/` and require system permissions. The build system handles this via `sudo make install`.

### Ollama Integration
The IME expects Ollama to be running locally on `localhost:11434`. Configure embedding models in `resources/config.json`.

### Testing
Use `make test` to run unit tests. IME functionality requires manual testing with actual text input.

### Debugging
Use Console.app to view IME system logs. Enable debug logging in `src/utils/config.c`.