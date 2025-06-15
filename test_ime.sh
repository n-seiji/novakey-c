#!/bin/bash

# NovaKey IME Testing Script
# This script helps test the NovaKey IME functionality

echo "🧪 NovaKey IME Testing Guide"
echo "=============================="
echo ""

# Check if NovaKey is installed (system or user location)
SYSTEM_IME_PATH="/Library/Input Methods/NovaKey.app"
USER_IME_PATH="$HOME/Library/Input Methods/NovaKey.app"

if [ -d "$SYSTEM_IME_PATH" ]; then
    echo "✅ NovaKey is installed at: $SYSTEM_IME_PATH (system-wide)"
    IME_PATH="$SYSTEM_IME_PATH"
elif [ -d "$USER_IME_PATH" ]; then
    echo "✅ NovaKey is installed at: $USER_IME_PATH (user-space)"
    IME_PATH="$USER_IME_PATH"
else
    echo "❌ NovaKey is not installed. Run ./install_ime.sh or ./install_ime_user.sh first"
    exit 1
fi

# Check if Ollama is running
if pgrep -x "ollama" > /dev/null; then
    echo "✅ Ollama server is running"
else
    echo "⚠️  Ollama server is not running"
    echo "   Start it with: ollama serve &"
fi

# Check if nomic-embed-text model is available
if ollama list | grep -q "nomic-embed-text"; then
    echo "✅ nomic-embed-text model is available"
else
    echo "⚠️  nomic-embed-text model not found"
    echo "   Install it with: ollama pull nomic-embed-text"
fi

echo ""
echo "🔍 Manual Testing Steps:"
echo "========================"
echo ""
echo "1. System Setup:"
echo "   • Open System Preferences > Keyboard > Input Sources"
echo "   • Add 'NovaKey' from the Japanese section"
echo "   • Ensure NovaKey appears in your input source menu"
echo ""
echo "2. Basic IME Testing:"
echo "   • Open TextEdit or any text application"
echo "   • Switch to NovaKey input method from menu bar"
echo "   • Press Shift+Ctrl+Space to toggle English/Japanese mode"
echo "   • Try typing in both modes"
echo ""
echo "3. Japanese Input Testing:"
echo "   • Switch to Japanese mode (Shift+Ctrl+Space)"
echo "   • Type: 'konnichiwa' (should show composition)"
echo "   • Press Enter to commit"
echo "   • Press Escape to cancel composition"
echo ""
echo "4. Check Console Logs:"
echo "   • Open Console.app"
echo "   • Filter for 'NovaKey' to see debug messages"
echo "   • Look for initialization and input events"
echo ""
echo "5. Advanced Testing (with Ollama):"
echo "   • Ensure Ollama is running: ollama serve"
echo "   • Type Japanese text and observe embedding processing"
echo "   • Check logs for embedding API calls"
echo ""
echo "🐛 Troubleshooting:"
echo "=================="
echo ""
echo "If NovaKey doesn't appear in Input Sources:"
echo "• Try logging out and back in"
echo "• Check permissions: ls -la '$IME_PATH'"
echo "• Reinstall: ./uninstall_ime.sh && ./install_ime.sh"
echo ""
echo "If NovaKey crashes or doesn't respond:"
echo "• Check Console.app for error messages"
echo "• Verify all dependencies are installed"
echo "• Test individual components: ./build/test_morphology"
echo ""
echo "If embedding features don't work:"
echo "• Verify Ollama is running: pgrep ollama"
echo "• Test embedding manually: ./build/tests/test_embedding"
echo "• Check Ollama model: ollama list"

# Function to run a simple embedding test
run_embedding_test() {
    echo ""
    echo "🧪 Running embedding test..."
    
    if [ -f "./build/tests/test_embedding" ]; then
        ./build/tests/test_embedding
    else
        echo "❌ test_embedding binary not found. Build it first with: make"
    fi
}

# Ask if user wants to run embedding test
echo ""
read -p "🤔 Would you like to run the embedding test now? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    run_embedding_test
fi

echo ""
echo "📝 Remember to check /Users/seiji-nakayama/ghq/github.com/n-seiji/novakey-c/CLAUDE.md"
echo "   for additional development and debugging information!"