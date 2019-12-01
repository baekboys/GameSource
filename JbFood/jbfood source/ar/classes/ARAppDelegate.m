//
//  ARAppDelegate.m
//  AR
//
//  Created by myungsun baek on 10. 12. 13..
//  Copyright 2010 UKG. All rights reserved.
//

#import "ARAppDelegate.h"
#import "ARViewController.h"

#import "Restaurant.h"
#import "DatabaseConnector.h"

@implementation ARAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize restaurantArray;


#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
    // Override point for customization after application launch.
	//self.restaurantArray = [[NSMutableArray alloc] init];
	
	// 맛집 추가-------------------------------------------------
	//{
//		Restaurant *restaurant = [[Restaurant alloc] init];
//		restaurant.name = @"명선이 집";
//		restaurant.phone = @"063-273-1905";
//		restaurant.address = @"전북 전주시 완산구 서신동 신일@ 106/1002";
//		restaurant.latitude = 35.832314;
//		restaurant.longitude = 127.11062;
//		restaurant.city = @"전주시";
//		[self.restaurantArray addObject:restaurant];
//		[restaurant release];		
//	}

	
	// 맛집 추가-------------------------------------------------
	//{
//		Restaurant *restaurant = [[Restaurant alloc] init];
//		restaurant.name = @"시청";
//		restaurant.phone = @"063-273-1905";
//		restaurant.address = @"전북 전주시 완산구 서신동 신일@ 106/1002";
//		restaurant.latitude = 35.824416;
//		restaurant.longitude = 127.147994;
//		restaurant.city = @"전주시";
//		[self.restaurantArray addObject:restaurant];
//		[restaurant release];
//	}


	// 맛집 추가-------------------------------------------------
	//{
//		Restaurant *restaurant = [[Restaurant alloc] init];
//		restaurant.name = @"전북대학교";
//		restaurant.phone = @"063-273-1905";
//		restaurant.address = @"전북 전주시 완산구 서신동 신일@ 106/1002";
//		restaurant.latitude = 35.847248;
//		restaurant.longitude = 127.129412;
//		restaurant.city = @"전주시";
//		[self.restaurantArray addObject:restaurant];
//		[restaurant release];
//	}
	
	//[self.viewController addARRestaurantViewWithRestaurantArrays:self.restaurantArray];
	
	
	//NSMutableArray *array = [DatabaseConnector getRestaurantsArrayFromArea:@"전주시"];
	//[self.viewController addARRestaurantViewWithRestaurantArrays:array];
	
    // Add the view controller's view to the window and display.
    [self.window addSubview:viewController.view];
    [self.window makeKeyAndVisible];

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
	[self.restaurantArray release];
    [viewController release];
    [window release];
    [super dealloc];
}


@end
