//
//  ARAppDelegate.h
//  AR
//
//  Created by myungsun baek on 10. 12. 13..
//  Copyright 2010 UKG. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ARViewController;

@interface ARAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    ARViewController *viewController;
	
	NSMutableArray *restaurantArray;
}

@property (nonatomic, retain) IBOutlet	UIWindow *window;
@property (nonatomic, retain) IBOutlet	ARViewController *viewController;

@property (nonatomic, retain)			NSMutableArray *restaurantArray;

@end

