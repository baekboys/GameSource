#import "RestaurantAppDelegate.h"
#import "Restaurant.h"
#import "RestaurantViewController.h"
#import "RestaurantPreViewController.h"

@implementation RestaurantAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize previewController;
@synthesize naviController;

#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
    // Override point for customization after application launch.
	Restaurant *restaurant = [[Restaurant alloc] init];
	restaurant.id = 0;
	restaurant.name = @"명선이의 맛집";
	restaurant.address = @"전주시 완산구 서신동 신일아파트 106/1002";
	restaurant.phone = @"063-273-1905";
	restaurant.city = @"전주시";
	restaurant.intro = @"역시 집에서 먹는 밥이 최고라는 것!!!!!!하하하하하ㅏ하하ㅏㅎ우히히히히히히히히 나 원래 이런 사람이야.... 알아서 기어~~ 알았으면 뛰어... 등등등 테스트용도....아직도 더 넣어야 하는구나..ㅋㅋㅋㅋㅋㅋㅋ 멀기도 먼 아이폰 프로그래밍이네~~~~~ ㅠㅠㅠㅠㅠㅠㅠ";
	restaurant.latitude = 35.832314;
	restaurant.longitude = 127.11062;
	restaurant.etc1 = @"asdkljflkasjdlkfjlakjelrkjalksjflkjzlkcvjlkjalkdjlkajrl;kja;lrkjal;skdrjl;kasjrlkajsdlkjflaksjdflk";
	//restaurant.starCount = 1;
	//restaurant.starAverage = 4;
	//restaurant.starString = @"★★★★★";
	//restaurant.imageData1 = UIImagePNGRepresentation([UIImage imageNamed:@"jhs.png"]);
	
	//self.viewController = [[RestaurantViewController alloc] initWithNibName:@"RestaurantViewController" bundle:nil withRestaurant:restaurant];
	//self.viewController = [[RestaurantViewController alloc] initWithRestaurant:restaurant];
	self.previewController = [[RestaurantPreViewController alloc] initWithRestaurant:restaurant];
	self.naviController = [[UINavigationController alloc] initWithRootViewController:self.previewController];
	
	[restaurant release];
    
    [self.window makeKeyAndVisible];
	[self.window addSubview:naviController.view];
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
	[self.viewController release];
	[self.naviController release];
	[self.previewController release];
    [window release];
    [super dealloc];
}


@end
