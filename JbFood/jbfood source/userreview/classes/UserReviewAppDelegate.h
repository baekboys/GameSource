//
//  UserReviewAppDelegate.h
//  UserReview
//
//  Created by 백명선 on 11. 1. 21..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class UserReviewViewController;
@class UserReviewResultTableViewController;

@interface UserReviewAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    UserReviewViewController *viewController;
	UserReviewResultTableViewController *resultViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UserReviewViewController *viewController;
@property (nonatomic, retain)			UserReviewResultTableViewController *resultViewController;
@end

