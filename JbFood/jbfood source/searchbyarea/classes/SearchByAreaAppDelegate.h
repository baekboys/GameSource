//
//  SearchByAreaAppDelegate.h
//  SearchByArea
//
//  Created by myungsun baek on 10. 12. 23..
//  Copyright 2010 UKG. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SearchByAreaTableViewController;

@interface SearchByAreaAppDelegate : NSObject <UIApplicationDelegate> {
    
    UIWindow *window;
	UINavigationController *navigationController;
    SearchByAreaTableViewController *tableViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) UINavigationController *navigationController;
@property (nonatomic, retain) SearchByAreaTableViewController *tableViewController;

@end

