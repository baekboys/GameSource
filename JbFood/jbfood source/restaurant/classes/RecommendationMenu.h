#import <Foundation/Foundation.h>


@interface RecommendationMenu : NSObject 
{
	NSString *menu;
	NSString *price;
}

@property (nonatomic, copy) NSString *menu;
@property (nonatomic, copy) NSString *price;

@end
