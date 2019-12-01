#import <UIKit/UIKit.h>

@class DatabaseTestViewController;

@interface DataBaseAppDelegate : NSObject <UIApplicationDelegate> 
{
    UIWindow *window;
	DatabaseTestViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet DatabaseTestViewController *viewController;

@end

