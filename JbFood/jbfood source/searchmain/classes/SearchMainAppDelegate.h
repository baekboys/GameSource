//
//  SearchMainAppDelegate.h
//  SearchMain
//
//  Created by 백명선 on 11. 1. 12..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SearchMainViewController;

@interface SearchMainAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	SearchMainViewController	*viewController;
	UINavigationController		*naviController;
}

@property (nonatomic, retain) IBOutlet	UIWindow *window;
@property (nonatomic, retain)			SearchMainViewController	*viewController;
@property (nonatomic, retain)			UINavigationController		*naviController;

@end

