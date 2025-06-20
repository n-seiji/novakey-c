#import <Foundation/Foundation.h>
#import <InputMethodKit/InputMethodKit.h>
#import "../../include/novakey.h"

@interface NovaKeyServer : IMKServer
@end

@implementation NovaKeyServer

- (instancetype)init {
    // Use the modern initialization method
    NSString* connectionName = NOVAKEY_CONNECTION_NAME;
    NSBundle* bundle = [NSBundle mainBundle];
    
    self = [super initWithName:connectionName bundleIdentifier:bundle.bundleIdentifier];
    if (self) {
        NSLog(@"NovaKey IME Server initialized with bundle: %@", bundle.bundleIdentifier);
    }
    return self;
}

- (void)dealloc {
    NSLog(@"NovaKey IME Server deallocated");
}

@end

static NovaKeyServer* server = nil;

int novakey_main(int argc, char* argv[]) {
    @autoreleasepool {
        // Initialize the server
        server = [[NovaKeyServer alloc] init];
        if (!server) {
            NSLog(@"Failed to initialize NovaKey IME Server");
            return 1;
        }
        
        // Run the main event loop
        [[NSRunLoop currentRunLoop] run];
        
        return 0;
    }
}