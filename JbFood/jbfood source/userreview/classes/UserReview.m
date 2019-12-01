#import "UserReview.h"


@implementation UserReview


@synthesize phone;
@synthesize review;
@synthesize star;


- (id)init
{
	if (self = [super init])
	{
		self.phone = @"";
		self.review = @"";
		self.star = 1;
	}
	
	return self;
}


- (void)dealloc
{
	self.phone = nil;
	self.review = nil;
	
	[super dealloc];
}


@end
