# NovaKey IME - Installation & Testing Guide

## 🎯 Overview

NovaKey is a macOS Input Method Editor (IME) that combines traditional Japanese input with modern AI-powered features including:

- **Morphological Analysis**: Using MeCab for Japanese text processing
- **LLM Embeddings**: Via Ollama API for semantic matching
- **Smart Candidate Ranking**: Combining embedding similarity and phonetic scores
- **Mode Switching**: Shift+Ctrl+Space to toggle English/Japanese input

## 📦 Installation

### Prerequisites

1. **Dependencies** (install via Homebrew):
   ```bash
   brew install cmake mecab mecab-ipadic cjson curl
   ```

2. **Ollama** (for embedding features):
   ```bash
   brew install ollama
   ollama serve &
   ollama pull nomic-embed-text
   ```

### Build & Install

1. **Build the project**:
   ```bash
   cmake .
   make
   ```

2. **Install IME** (choose one):
   
   **Option A: User installation** (recommended, no sudo required):
   ```bash
   ./install_ime_user.sh
   ```
   
   **Option B: System-wide installation** (requires admin):
   ```bash
   ./install_ime.sh
   ```

3. **Verify installation**:
   ```bash
   ./test_ime.sh
   ```

## 🧪 Testing

### Automated Tests

Run comprehensive functionality tests:

```bash
# Test morphological analysis
./build/test_morphology

# Test embedding functionality
./build/tests/test_embedding

# Test integration
./build/tests/test_integration

# Complete IME test guide
./test_ime.sh
```

### Manual Testing

1. **System Setup**:
   - Open System Preferences > Keyboard > Input Sources
   - Click '+' to add new input source
   - Look for 'NovaKey' in Japanese category
   - Add to your input sources

2. **Basic Functionality**:
   - Open TextEdit or any text application
   - Switch to NovaKey from input source menu
   - Press `Shift+Ctrl+Space` to toggle English/Japanese modes
   - Type text in both modes

3. **Japanese Input**:
   - Switch to Japanese mode
   - Type romaji (e.g., "konnichiwa")
   - See composition buffer update
   - Press Enter to commit, Escape to cancel

4. **Advanced Features** (with Ollama):
   - Ensure Ollama server is running
   - Type Japanese text and observe embedding processing
   - Check Console.app for debug logs

## 🔧 Troubleshooting

### IME Not Appearing in Input Sources

- Log out and back in to refresh input method list
- Check installation: `ls -la ~/Library/Input\ Methods/NovaKey.app`
- Reinstall: `./uninstall_ime_user.sh && ./install_ime_user.sh`
- Try system reboot if needed

### IME Crashes or Doesn't Respond

- Check Console.app for error messages (filter for "NovaKey")
- Verify dependencies: `brew list mecab cjson curl`
- Test components individually: `./build/test_morphology`
- Check permissions: `chmod -R 755 ~/Library/Input\ Methods/NovaKey.app`

### Embedding Features Not Working

- Verify Ollama: `pgrep ollama` and `ollama list`
- Test embedding: `./build/tests/test_embedding`
- Check model: `ollama pull nomic-embed-text`
- Review Ollama logs for API errors

## 📊 System Status

The following components should be working:

✅ **Core IME Framework**: Input handling, mode switching, composition
✅ **Morphological Analysis**: MeCab integration for Japanese text processing  
✅ **Embedding Generation**: Ollama API integration with nomic-embed-text model
✅ **Candidate Ranking**: Cosine similarity and phonetic scoring
✅ **macOS Integration**: App bundle, Info.plist, Input Method registration

## 🗂️ File Structure

```
NovaKey.app/
├── Contents/
│   ├── Info.plist          # IME registration metadata
│   ├── MacOS/
│   │   └── novakey         # Main executable
│   └── Resources/          # (empty, for future icons/assets)
```

## 🚀 Next Steps

1. **Add NovaKey to Input Sources** in System Preferences
2. **Test basic input switching** with Shift+Ctrl+Space
3. **Try Japanese composition** and candidate selection
4. **Verify embedding features** with Ollama running
5. **Check debug logs** in Console.app for detailed behavior

For development details, see `CLAUDE.md` in the project root.