#import <UIKit/UIKit.h>

@class SearchByNameViewController;

@interface SearchByNameAppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow *window;
	UINavigationController		*naviController;
    SearchByNameViewController	*viewController;
}

@property (nonatomic, retain) IBOutlet	UIWindow *window;
@property (nonatomic, retain)			UINavigationController		*naviController;
@property (nonatomic, retain)			SearchByNameViewController *viewController;

@end

