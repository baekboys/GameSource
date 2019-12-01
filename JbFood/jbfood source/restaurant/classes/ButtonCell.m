#import "ButtonCell.h"


@implementation ButtonCell


@synthesize callButton;
@synthesize mapButton;
@synthesize arButton;
@synthesize buttonDelegate;


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
    
    // Configure the view for the selected state.
}


- (void)dealloc
{
	self.callButton = nil;
	self.mapButton = nil;
	self.arButton = nil;
    [super dealloc];
}


-(IBAction)touchButton:(id)sender
{
	BUTTONCELL_TYPE button_event;
	
	if (sender == self.callButton) 
	{
		button_event = BUTTONCELL_TYPE_CALL;
	}
	else if (sender == self.mapButton)
	{
		button_event = BUTTONCELL_TYPE_MAP;
	}
	else if (sender == self.arButton)
	{
		button_event = BUTTONCELL_TYPE_AR;
	}
	
	[self.buttonDelegate buttonDidTouchEvent:button_event];
}


@end
