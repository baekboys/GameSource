#import "PreInfoCell.h"


@implementation PreInfoCell


@synthesize addressLabel;
@synthesize phoneLabel;
@synthesize menuLabel;
@synthesize addressNameLabel;
@synthesize phoneNameLabel;
@synthesize menuNameLabel;


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
	self.addressLabel = nil;
	self.phoneLabel = nil;
	self.menuLabel = nil;
	self.addressNameLabel = nil;
	self.phoneNameLabel = nil;
	self.menuNameLabel = nil;
	
    [super dealloc];
}


@end
