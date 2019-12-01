#import <UIKit/UIKit.h>

@class RestaurantInfoTabbarController;

@interface RestaurantInfoAppDelegate : NSObject <UIApplicationDelegate, UITabBarControllerDelegate> {
    UIWindow *window;
    RestaurantInfoTabbarController *tabBarController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) RestaurantInfoTabbarController *tabBarController;

@end
