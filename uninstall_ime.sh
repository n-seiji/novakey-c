#!/bin/bash

# NovaKey IME Uninstallation Script
# This script removes NovaKey from the system Input Methods

set -e

echo "🗑️  Uninstalling NovaKey IME..."

# Define paths
IME_DIR="/Library/Input Methods"
APP_NAME="NovaKey.app"
INSTALLED_PATH="$IME_DIR/$APP_NAME"

# Check if we need sudo permissions
if [ ! -w "$IME_DIR" ]; then
    echo "🔐 Root permissions required for uninstallation..."
    sudo_cmd="sudo"
else
    sudo_cmd=""
fi

# Kill any running NovaKey processes
echo "🔄 Stopping NovaKey processes..."
killall -9 "NovaKey" 2>/dev/null || true
killall -9 "novakey" 2>/dev/null || true

# Remove the app bundle
if [ -d "$INSTALLED_PATH" ]; then
    echo "📁 Removing $INSTALLED_PATH..."
    $sudo_cmd rm -rf "$INSTALLED_PATH"
    echo "✅ NovaKey.app removed successfully"
else
    echo "ℹ️  NovaKey.app not found at $INSTALLED_PATH"
fi

# Refresh Input Method cache
echo "🔄 Refreshing Input Method cache..."
$sudo_cmd touch /System/Library/Caches/com.apple.IntlDataCache.le* 2>/dev/null || true

echo "✅ NovaKey IME uninstalled successfully!"
echo ""
echo "📋 You may need to:"
echo "1. Remove NovaKey from System Preferences > Keyboard > Input Sources"
echo "2. Log out and back in to completely refresh the input method list"