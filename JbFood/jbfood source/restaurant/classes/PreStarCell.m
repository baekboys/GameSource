#import "PreStarCell.h"


@implementation PreStarCell

@synthesize starLabel;


- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier 
{
    
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code.
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{    
    [super setSelected:selected animated:animated];
	
}


- (void)dealloc 
{
	self.starLabel = nil;
    [super dealloc];
}


@end
