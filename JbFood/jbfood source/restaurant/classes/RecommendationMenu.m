#import "RecommendationMenu.h"


@implementation RecommendationMenu

@synthesize menu, price;

- (id)init
{
	if (self = [super init])
	{
		self.menu = nil;
		self.price = nil;
	}
	
	return self;
}

- (void)dealloc
{
	self.menu = nil;
	self.price = nil;
	[super dealloc];
}

@end
