#import <Foundation/Foundation.h>


@interface UserReview : NSObject 
{
	NSString		*phone;
	NSString		*review;
	NSUInteger		star;
}


@property (nonatomic, copy)		NSString		*phone;
@property (nonatomic, copy)		NSString		*review;
@property (nonatomic)			NSUInteger		star;


@end
