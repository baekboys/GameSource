//
//  JBFoodAppDelegate.h
//  JBFood
//
//  Created by myungsun baek on 10. 12. 23..
//  Copyright 2010 UKG. All rights reserved.
//

#import <UIKit/UIKit.h>

@class JBFoodViewController;

@interface JBFoodAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    JBFoodViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet JBFoodViewController *viewController;

@end

