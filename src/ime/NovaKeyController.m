#import <Foundation/Foundation.h>
#import <InputMethodKit/InputMethodKit.h>
#import "../../include/novakey.h"
#import "../../include/search.h"
#import "../../include/embedding.h"
#import "../../include/morphology.h"

@interface NovaKeyController : IMKInputController {
    NovaKeyInputMode _currentMode;
    NSMutableString* _compositionBuffer;
    NSMutableArray* _candidates;
    SearchConfig* _searchConfig;
    Dictionary* _dictionary;
    OllamaClient* _ollamaClient;
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
        
        // Initialize search components
        _searchConfig = create_search_config();
        _dictionary = load_dictionary("resources/dictionary.txt");
        _ollamaClient = ollama_client_create("http://localhost:11434", "nomic-embed-text");
        
        NSLog(@"NovaKey Controller initialized for client: %@", inputClient);
        NSLog(@"Dictionary loaded with %d entries", _dictionary ? _dictionary->entry_count : 0);
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
    
    // Generate candidates for current composition
    [self generateCandidates];
    
    NSString* displayText = [NSString stringWithFormat:@"%@", _compositionBuffer];
    
    [sender setMarkedText:displayText
           selectionRange:NSMakeRange(displayText.length, 0)
        replacementRange:NSMakeRange(NSNotFound, NSNotFound)];
    
    NSLog(@"Updated composition: %@ (candidates: %lu)", displayText, (unsigned long)_candidates.count);
}

- (void)commitComposition:(id)sender {
    if (_compositionBuffer.length > 0) {
        NSString* commitText;
        
        // Use first candidate if available, otherwise use raw input
        if (_candidates.count > 0) {
            commitText = [_candidates objectAtIndex:0];
        } else {
            commitText = [NSString stringWithString:_compositionBuffer];
        }
        
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

- (void)generateCandidates {
    if (!_dictionary || _compositionBuffer.length == 0) {
        [_candidates removeAllObjects];
        return;
    }
    
    const char* inputText = [_compositionBuffer UTF8String];
    
    // Generate morphological analysis
    MorphResult* morphResult = analyze_text(inputText);
    
    // Search for candidates
    CandidateList* candidateList = search_candidates(inputText, morphResult, _dictionary, _searchConfig, _ollamaClient);
    
    // Convert C candidates to NSArray
    [_candidates removeAllObjects];
    if (candidateList) {
        for (int i = 0; i < candidateList->candidate_count; i++) {
            NSString* candidateText = [NSString stringWithUTF8String:candidateList->candidates[i].text];
            [_candidates addObject:candidateText];
        }
        free_candidate_list(candidateList);
    }
    
    if (morphResult) {
        free_morph_result(morphResult);
    }
    
    NSLog(@"Generated %lu candidates for input: %@", (unsigned long)_candidates.count, _compositionBuffer);
}

- (void)dealloc {
    if (_searchConfig) {
        free_search_config(_searchConfig);
    }
    if (_dictionary) {
        free_dictionary(_dictionary);
    }
    if (_ollamaClient) {
        ollama_client_destroy(_ollamaClient);
    }
    [super dealloc];
}

@end