#import <UIKit/UIKit.h>

#define kButtonCellHeight	40


typedef enum
{
	BUTTONCELL_TYPE_CALL,
	BUTTONCELL_TYPE_MAP,
	BUTTONCELL_TYPE_AR,
	BUTTONCELL_TYPE_COUNT
}	BUTTONCELL_TYPE;

@protocol buttonDidTouch;


@interface ButtonCell : UITableViewCell 
{
	UIButton		*callButton;
	UIButton		*mapButton;
	UIButton		*arButton;
	
	id<buttonDidTouch>	buttonDelegate;
}


@property (nonatomic, retain)	IBOutlet	UIButton		*callButton;
@property (nonatomic, retain)	IBOutlet	UIButton		*mapButton;
@property (nonatomic, retain)	IBOutlet	UIButton		*arButton;
@property (nonatomic, assign)				id<buttonDidTouch>	buttonDelegate;

-(IBAction)touchButton:(id)sender;

@end


@protocol buttonDidTouch
- (void) buttonDidTouchEvent:(BUTTONCELL_TYPE)button;
@end
