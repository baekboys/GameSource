#import "UserReviewResultCell.h"


@implementation UserReviewResultCell

@synthesize starString;
@synthesize userReview;

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
	self.starString = nil;
	self.userReview = nil;
    [super dealloc];
}


@end
