#!/bin/bash

# NovaKey IME Code Signing Script
# This script creates a properly signed IME bundle for macOS

set -e

echo "🔐 Creating signed NovaKey IME..."

# Check if build exists
if [ ! -d "build/NovaKey.app" ]; then
    echo "❌ Error: NovaKey.app not found in build directory"
    echo "Please run 'cmake . && make' first"
    exit 1
fi

# Create entitlements file
cat > build/entitlements.plist << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.app-sandbox</key>
    <false/>
    <key>com.apple.security.get-task-allow</key>
    <true/>
</dict>
</plist>
EOF

# Check for available signing identities
echo "🔍 Checking for available signing identities..."
AVAILABLE_IDENTITIES=$(security find-identity -v -p codesigning | grep "Developer ID Application" | head -1 | cut -d'"' -f2 || echo "")

if [ -n "$AVAILABLE_IDENTITIES" ]; then
    echo "✅ Found Developer ID: $AVAILABLE_IDENTITIES"
    SIGN_IDENTITY="$AVAILABLE_IDENTITIES"
elif security find-identity -v -p codesigning | grep -q "Mac Developer"; then
    SIGN_IDENTITY=$(security find-identity -v -p codesigning | grep "Mac Developer" | head -1 | cut -d'"' -f2)
    echo "✅ Found Mac Developer certificate: $SIGN_IDENTITY"
elif security find-identity -v -p codesigning | grep -q "Apple Development"; then
    SIGN_IDENTITY=$(security find-identity -v -p codesigning | grep "Apple Development" | head -1 | cut -d'"' -f2)
    echo "✅ Found Apple Development certificate: $SIGN_IDENTITY"
else
    echo "⚠️  No valid signing identity found. Creating ad-hoc signature..."
    SIGN_IDENTITY="-"
fi

# Remove existing signature if present
echo "🧹 Removing existing signatures..."
codesign --remove-signature build/NovaKey.app 2>/dev/null || true

# Sign the executable first
echo "🔏 Signing executable..."
codesign --force --sign "$SIGN_IDENTITY" \
    --entitlements build/entitlements.plist \
    --options runtime \
    build/NovaKey.app/Contents/MacOS/NovaKey

# Sign the entire bundle
echo "🔏 Signing app bundle..."
codesign --force --sign "$SIGN_IDENTITY" \
    --entitlements build/entitlements.plist \
    --options runtime \
    build/NovaKey.app

# Verify the signature
echo "✅ Verifying signature..."
codesign --verify --verbose=4 build/NovaKey.app

echo "🎉 NovaKey IME signed successfully!"
echo ""
echo "📋 Next steps:"
echo "1. Run ./install_ime.sh or ./install_ime_user.sh"
echo "2. Add NovaKey to Input Sources in System Preferences"
echo ""
echo "🔍 If NovaKey still doesn't appear in Input Sources:"
echo "   - The issue might be with the Info.plist configuration"
echo "   - Try logging out and back in"
echo "   - Check Console.app for any error messages" 