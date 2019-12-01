//
//  SearchBySituationAppDelegate.h
//  SearchBySituation
//
//  Created by 백명선 on 11. 1. 12..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SearchBySituationViewController;

@interface SearchBySituationAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	UINavigationController			*naviContorller;
	SearchBySituationViewController *viewController;
}

@property (nonatomic, retain) IBOutlet	UIWindow *window;
@property (nonatomic, retain)			UINavigationController			*naviContorller;
@property (nonatomic, retain)			SearchBySituationViewController *viewController;

@end

