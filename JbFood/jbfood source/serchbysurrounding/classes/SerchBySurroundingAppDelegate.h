#import <UIKit/UIKit.h>

@class SearchBySurroundingRootController;

@interface SerchBySurroundingAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	UINavigationController *navigationController;
	SearchBySurroundingRootController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) UINavigationController *navigationController;
@property (nonatomic, retain) SearchBySurroundingRootController *viewController;

@end

