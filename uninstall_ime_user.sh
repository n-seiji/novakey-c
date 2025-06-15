#!/bin/bash

# NovaKey IME User Uninstallation Script
# This script removes NovaKey from the user's Input Methods

set -e

echo "🗑️  Uninstalling NovaKey IME from user space..."

# Define paths
USER_IME_DIR="$HOME/Library/Input Methods"
APP_NAME="NovaKey.app"
INSTALLED_PATH="$USER_IME_DIR/$APP_NAME"

# Kill any running NovaKey processes
echo "🔄 Stopping NovaKey processes..."
killall -9 "NovaKey" 2>/dev/null || true
killall -9 "novakey" 2>/dev/null || true

# Remove the app bundle
if [ -d "$INSTALLED_PATH" ]; then
    echo "📁 Removing $INSTALLED_PATH..."
    rm -rf "$INSTALLED_PATH"
    echo "✅ NovaKey.app removed successfully"
else
    echo "ℹ️  NovaKey.app not found at $INSTALLED_PATH"
fi

# Refresh Input Method cache
echo "🔄 Refreshing Input Method cache..."
touch ~/Library/Caches/com.apple.IntlDataCache.le* 2>/dev/null || true

echo "✅ NovaKey IME uninstalled successfully!"
echo ""
echo "📋 You may need to:"
echo "1. Remove NovaKey from System Preferences > Keyboard > Input Sources"
echo "2. Log out and back in to completely refresh the input method list"