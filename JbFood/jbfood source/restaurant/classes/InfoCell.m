#import "InfoCell.h"


@implementation InfoCell


//@synthesize titleImageView;
@synthesize label_title;
@synthesize label_contents;

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
	//[self.titleImageView release];
	self.label_title = nil;
	self.label_contents = nil;
    [super dealloc];
}


@end
