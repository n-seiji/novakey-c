#!/bin/bash

# NovaKey IME User Installation Script
# This script installs NovaKey as a user Input Method in macOS (no sudo required)

set -e

echo "🚀 Installing NovaKey IME for current user..."

# Check if build exists
if [ ! -d "build/NovaKey.app" ]; then
    echo "❌ Error: NovaKey.app not found in build directory"
    echo "Please run 'cmake . && make' first"
    exit 1
fi

# Define installation paths (user space)
USER_IME_DIR="$HOME/Library/Input Methods"
APP_NAME="NovaKey.app"
INSTALLED_PATH="$USER_IME_DIR/$APP_NAME"

# Create user Input Methods directory if it doesn't exist
mkdir -p "$USER_IME_DIR"

echo "📁 Installing to: $INSTALLED_PATH"

# Remove existing installation if it exists
if [ -d "$INSTALLED_PATH" ]; then
    echo "🗑️  Removing existing installation..."
    rm -rf "$INSTALLED_PATH"
fi

# Copy the app bundle to user location
echo "📋 Copying NovaKey.app to user directory..."
cp -R "build/$APP_NAME" "$USER_IME_DIR/"

# Set correct permissions
echo "🔧 Setting permissions..."
chmod -R 755 "$INSTALLED_PATH"

# Kill existing IME processes
echo "🔄 Stopping any running NovaKey processes..."
killall -9 "NovaKey" 2>/dev/null || true
killall -9 "novakey" 2>/dev/null || true

# Refresh the input method cache
echo "🔄 Refreshing Input Method cache..."
touch ~/Library/Caches/com.apple.IntlDataCache.le* 2>/dev/null || true

echo "✅ NovaKey IME installed successfully in user space!"
echo ""
echo "📋 Next steps:"
echo "1. Go to System Preferences > Keyboard > Input Sources"
echo "2. Click the '+' button to add a new input source"
echo "3. Look for 'NovaKey' in the Japanese category"
echo "4. Add NovaKey to your input sources"
echo "5. Use Shift+Ctrl+Space to switch between English/Japanese modes"
echo ""
echo "🔍 If NovaKey doesn't appear, try:"
echo "   - Logging out and back in"
echo "   - Restarting your Mac"
echo "   - Running: ./uninstall_ime_user.sh && ./install_ime_user.sh"
echo ""
echo "📊 To test embedding functionality, make sure Ollama is running:"
echo "   ollama serve &"
echo "   ollama pull nomic-embed-text"
echo ""
echo "🧪 Run the test script to verify installation:"
echo "   ./test_ime.sh"