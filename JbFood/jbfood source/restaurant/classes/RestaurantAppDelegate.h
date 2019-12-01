#import <UIKit/UIKit.h>

@class Restaurant;
@class RestaurantViewController;
@class RestaurantPreViewController;

@interface RestaurantAppDelegate : NSObject <UIApplicationDelegate> 
{
    UIWindow *window;
	RestaurantViewController *viewController;
	RestaurantPreViewController *previewController;
	UINavigationController *naviController;
}

@property (nonatomic, retain) IBOutlet	UIWindow *window;
@property (nonatomic, retain)			RestaurantViewController *viewController;
@property (nonatomic, retain)			RestaurantPreViewController *previewController;
@property (nonatomic, retain)			UINavigationController *naviController;
@end

