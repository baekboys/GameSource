//
//  FoodCulturePlazaAppDelegate.h
//  FoodCulturePlaza
//
//  Created by myungsun baek on 10. 12. 23..
//  Copyright 2010 UKG. All rights reserved.
//

#import <UIKit/UIKit.h>

@class FoodCulturePlazaViewController;

@interface FoodCulturePlazaAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    FoodCulturePlazaViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet FoodCulturePlazaViewController *viewController;

@end

