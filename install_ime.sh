#!/bin/bash

# NovaKey IME Installation Script
# This script installs NovaKey as a system Input Method in macOS

set -e

echo "🚀 Installing NovaKey IME..."

# Check if build exists
if [ ! -d "build/NovaKey.app" ]; then
    echo "❌ Error: NovaKey.app not found in build directory"
    echo "Please run 'cmake . && make' first"
    exit 1
fi

# Define installation paths
IME_DIR="/Library/Input Methods"
APP_NAME="NovaKey.app"
INSTALLED_PATH="$IME_DIR/$APP_NAME"

echo "📁 Installing to: $INSTALLED_PATH"

# Check if we need sudo permissions
if [ ! -w "$IME_DIR" ]; then
    echo "🔐 Root permissions required for installation..."
    sudo_cmd="sudo"
else
    sudo_cmd=""
fi

# Remove existing installation if it exists
if [ -d "$INSTALLED_PATH" ]; then
    echo "🗑️  Removing existing installation..."
    $sudo_cmd rm -rf "$INSTALLED_PATH"
fi

# Copy the app bundle to system location
echo "📋 Copying NovaKey.app to system directory..."
$sudo_cmd cp -R "build/$APP_NAME" "$IME_DIR/"

# Set correct permissions
echo "🔧 Setting permissions..."
$sudo_cmd chown -R root:wheel "$INSTALLED_PATH"
$sudo_cmd chmod -R 755 "$INSTALLED_PATH"

# Kill existing IME processes
echo "🔄 Restarting Input Method processes..."
killall -9 "NovaKey" 2>/dev/null || true

# Register the IME with the system
echo "📝 Registering IME with system..."

# Touch the InputMethods cache to force refresh
$sudo_cmd touch /System/Library/Caches/com.apple.IntlDataCache.le*

# Use the TIS framework to register (if available)
if command -v "/System/Library/Frameworks/Carbon.framework/Versions/A/Support/TISCreateInputSourceList" &> /dev/null; then
    echo "🔄 Refreshing Input Source list..."
    /System/Library/Frameworks/Carbon.framework/Versions/A/Support/TISCreateInputSourceList
fi

echo "✅ NovaKey IME installed successfully!"
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
echo "   - Running: ./uninstall_ime.sh && ./install_ime.sh"
echo ""
echo "📊 To test embedding functionality, make sure Ollama is running:"
echo "   ollama serve &"
echo "   ollama pull nomic-embed-text"