#import "SearchByAreaTableCell.h"


@implementation SearchByAreaTableCell


@synthesize areaImageView;
@synthesize nameLabel;


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
	self.areaImageView = nil;
	self.nameLabel = nil;
    [super dealloc];
}


@end
