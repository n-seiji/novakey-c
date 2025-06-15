#import <Foundation/Foundation.h>
#import <InputMethodKit/InputMethodKit.h>
#import "../../include/novakey.h"

@interface NovaKeyController : IMKInputController {
    NovaKeyInputMode _currentMode;
    NSMutableString* _compositionBuffer;
    NSMutableArray* _candidates;
}

@property (nonatomic, assign) NovaKeyInputMode currentMode;
@property (nonatomic, strong) NSMutableString* compositionBuffer;
@property (nonatomic, strong) NSMutableArray* candidates;

@end

@implementation NovaKeyController

- (instancetype)initWithServer:(IMKServer*)server delegate:(id)delegate client:(id)inputClient {
    self = [super initWithServer:server delegate:delegate client:inputClient];
    if (self) {
        _currentMode = NovaKeyInputModeEnglish;
        _compositionBuffer = [[NSMutableString alloc] init];
        _candidates = [[NSMutableArray alloc] init];
        NSLog(@"NovaKey Controller initialized for client: %@", inputClient);
    }
    return self;
}

- (BOOL)inputText:(NSString*)string client:(id)sender {
    NSLog(@"Input text: %@ (mode: %ld)", string, (long)_currentMode);
    
    if (_currentMode == NovaKeyInputModeEnglish) {
        // Direct English input
        [sender insertText:string replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
        return YES;
    } else {
        // Japanese input - add to composition buffer
        [_compositionBuffer appendString:string];
        [self updateComposition:sender];
        return YES;
    }
}

- (BOOL)handleEvent:(NSEvent*)event client:(id)sender {
    NSLog(@"Handle event: %@ (keyCode: %d, modifiers: %lu)", 
          event.characters, event.keyCode, (unsigned long)event.modifierFlags);
    
    // Check for mode switch: Shift+Ctrl+Space
    if (event.type == NSEventTypeKeyDown && 
        event.keyCode == 49 && // Space key
        (event.modifierFlags & NSEventModifierFlagShift) &&
        (event.modifierFlags & NSEventModifierFlagControl)) {
        
        [self toggleInputMode];
        return YES;
    }
    
    // Handle other special keys
    switch (event.keyCode) {
        case 36: // Return
            if (_currentMode == NovaKeyInputModeJapanese && _compositionBuffer.length > 0) {
                [self commitComposition:sender];
                return YES;
            }
            break;
            
        case 53: // Escape
            if (_currentMode == NovaKeyInputModeJapanese && _compositionBuffer.length > 0) {
                [self cancelComposition:sender];
                return YES;
            }
            break;
            
        case 51: // Delete
            if (_currentMode == NovaKeyInputModeJapanese && _compositionBuffer.length > 0) {
                [_compositionBuffer deleteCharactersInRange:NSMakeRange(_compositionBuffer.length - 1, 1)];
                [self updateComposition:sender];
                return YES;
            }
            break;
    }
    
    return [super handleEvent:event client:sender];
}

- (void)toggleInputMode {
    _currentMode = (_currentMode == NovaKeyInputModeEnglish) ? 
                   NovaKeyInputModeJapanese : NovaKeyInputModeEnglish;
    
    NSString* modeString = (_currentMode == NovaKeyInputModeEnglish) ? @"English" : @"Japanese";
    NSLog(@"Input mode switched to: %@", modeString);
    
    // Clear composition buffer when switching modes
    [_compositionBuffer setString:@""];
    [_candidates removeAllObjects];
}

- (void)updateComposition:(id)sender {
    if (_compositionBuffer.length == 0) {
        [sender setMarkedText:@"" 
               selectionRange:NSMakeRange(0, 0) 
            replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
        return;
    }
    
    // TODO: Process composition with morphological analysis and embedding
    NSString* displayText = [NSString stringWithFormat:@"%@", _compositionBuffer];
    
    [sender setMarkedText:displayText
           selectionRange:NSMakeRange(displayText.length, 0)
        replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
    
    NSLog(@"Updated composition: %@", displayText);
}

- (void)commitComposition:(id)sender {
    if (_compositionBuffer.length > 0) {
        // TODO: Get best candidate from embedding/phonetic ranking
        NSString* commitText = [NSString stringWithString:_compositionBuffer];
        
        [sender insertText:commitText 
            replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
        
        [_compositionBuffer setString:@""];
        [_candidates removeAllObjects];
        
        NSLog(@"Committed text: %@", commitText);
    }
}

- (void)cancelComposition:(id)sender {
    [_compositionBuffer setString:@""];
    [_candidates removeAllObjects];
    
    [sender setMarkedText:@""
           selectionRange:NSMakeRange(0, 0)
        replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
    
    NSLog(@"Cancelled composition");
}

@end