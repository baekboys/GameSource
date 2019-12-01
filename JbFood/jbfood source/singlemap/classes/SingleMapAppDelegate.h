//
//  SingleMapAppDelegate.h
//  SingleMap
//
//  Created by myungsun baek on 10. 12. 28..
//  Copyright 2010 UKG. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SingleMapViewController;

@interface SingleMapAppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow *window;
	UINavigationController *naviController;
	SingleMapViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) UINavigationController *naviController;
@property (nonatomic, retain) SingleMapViewController *viewController;

@end

