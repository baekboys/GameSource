#import <UIKit/UIKit.h>


#define kPreInfoCellHeight	90


@interface PreInfoCell : UITableViewCell 
{
	// UI
	UILabel	*addressLabel;
	UILabel	*phoneLabel;
	UILabel	*menuLabel;
	
	UILabel	*addressNameLabel;
	UILabel	*phoneNameLabel;
	UILabel	*menuNameLabel;
}


@property (nonatomic, retain)	IBOutlet	UILabel	*addressLabel;
@property (nonatomic, retain)	IBOutlet	UILabel	*phoneLabel;
@property (nonatomic, retain)	IBOutlet	UILabel	*menuLabel;
@property (nonatomic, retain)	IBOutlet	UILabel	*addressNameLabel;
@property (nonatomic, retain)	IBOutlet	UILabel	*phoneNameLabel;
@property (nonatomic, retain)	IBOutlet	UILabel	*menuNameLabel;


@end
