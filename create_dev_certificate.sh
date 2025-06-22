#!/bin/bash

# Create a self-signed certificate for development using Keychain Access
# This is NOT the same as Apple Developer Certificate but may help with IME recognition

set -e

echo "🔐 Creating development certificate for NovaKey IME..."

CERT_NAME="NovaKey IME Development"

# Check if certificate already exists
if security find-identity -v -p codesigning | grep -q "$CERT_NAME"; then
    echo "✅ Certificate '$CERT_NAME' already exists"
    exit 0
fi

echo "📝 Instructions to create a development certificate:"
echo ""
echo "1. Open Keychain Access (キーチェーンアクセス)"
echo "2. Go to Keychain Access > Certificate Assistant > Create a Certificate..."
echo "3. Set the following:"
echo "   - Name: $CERT_NAME"
echo "   - Identity Type: Self Signed Root"
echo "   - Certificate Type: Code Signing"
echo "   - Check 'Let me override defaults'"
echo "4. Click Continue through all steps (accept defaults)"
echo "5. Make sure it's saved to 'login' keychain"
echo ""
echo "Or run this command to open Keychain Access:"

# Open Keychain Access
open -a "Keychain Access"

echo ""
echo "After creating the certificate, run:"
echo "  security find-identity -v -p codesigning"
echo "  ./create_signed_ime.sh"
echo ""
echo "⚠️  Note: This is a development certificate, not an Apple Developer Certificate"
echo "   For production use, you need to join Apple Developer Program ($99/year)" 