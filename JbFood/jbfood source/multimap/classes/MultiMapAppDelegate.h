//
//  MultiMapAppDelegate.h
//  MultiMap
//
//  Created by 백명선 on 10. 12. 29..
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MultiMapViewController;

@interface MultiMapAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	MultiMapViewController *viewController;
	UINavigationController *naviController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MultiMapViewController *viewController;
@property (nonatomic, retain) UINavigationController *naviController;

@end

