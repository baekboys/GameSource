#import "SearchByAreaResultTableCell.h"


@implementation SearchByAreaResultTableCell


@synthesize restaurantImageView;
@synthesize nameLabel;
@synthesize menuLabel;
@synthesize starLabel;

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super initWithCoder:aDecoder]) 
	{

	}
	return self;
}


- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier 
{    
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) 
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
	self.nameLabel = nil;
	self.menuLabel = nil;
	self.starLabel = nil;
    [super dealloc];
}


@end
