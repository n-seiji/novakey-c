#import <Foundation/Foundation.h>
#import <InputMethodKit/InputMethodKit.h>
#import <objc/message.h>

// This compatibility shim re-introduces the classic two-parameter
// -[IMKServer initWithName:bundleIdentifier:] initialiser that disappeared
// on macOS 15+/Sequoia.  If the original selector still exists it will be
// called, otherwise we forward to the new three-parameter variant
// -initWithName:bundleIdentifier:controllerClassName: passing the provided
// NovaKey controller class.  This prevents a runtime "unrecognised selector"
// crash on newer macOS versions while remaining compatible with older ones.

@interface IMKServer (NovaKeyCompatibility)
- (instancetype)nk_initWithName:(NSString *)name bundleIdentifier:(NSString *)bundleIdentifier controllerClassName:(NSString *)controller;
@end

@implementation IMKServer (NovaKeyCompatibility)

// Add missing selector at runtime if necessary
- (instancetype)initWithName:(NSString *)name bundleIdentifier:(NSString *)bundleIdentifier
{
    // If the modern selector exists, call it dynamically
    SEL modernSel = sel_getUid("initWithName:bundleIdentifier:controllerClassName:");
    if ([self respondsToSelector:modernSel]) {
        typedef id (*InitModernIMP)(id, SEL, NSString *, NSString *, NSString *);
        InitModernIMP imp = (InitModernIMP)objc_msgSend;
        return imp(self, modernSel, name, bundleIdentifier, @"NovaKeyController");
    }

    // Fallback: if the original selector is still valid on this OS, let the
    // runtime dispatch to the superclass implementation (just call super).
    struct objc_super sup = { .receiver = self, .super_class = [IMKServer class] };
    typedef id (*SuperInitIMP)(struct objc_super *, SEL, NSString *, NSString *);
    SuperInitIMP superImp = (SuperInitIMP)objc_msgSendSuper;
    return superImp(&sup, _cmd, name, bundleIdentifier);
}

@end 