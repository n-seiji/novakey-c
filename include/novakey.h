#ifndef NOVAKEY_H
#define NOVAKEY_H

#include <Foundation/Foundation.h>
#include <InputMethodKit/InputMethodKit.h>
#include "novakey_core.h"

// Forward declarations
@class NovaKeyServer;
@class NovaKeyController;

// Constants
#define NOVAKEY_BUNDLE_ID @"com.novakey.inputmethod"
#define NOVAKEY_CONNECTION_NAME @"NovaKey_1_Connection"

// Function prototypes
int novakey_main(int argc, char* argv[]);

#endif // NOVAKEY_H