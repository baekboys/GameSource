#import "ImageReviewCell.h"


@implementation ImageReviewCell


@synthesize restaurantImageView;
@synthesize reviewTextView;


- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier 
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
		
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];
    
}


- (void)dealloc 
{
	self.restaurantImageView = nil;
	self.reviewTextView = nil;
    [super dealloc];
}


@end
