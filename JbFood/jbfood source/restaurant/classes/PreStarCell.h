#import <UIKit/UIKit.h>


#define kPreStarCellHeight	26


@interface PreStarCell : UITableViewCell 
{
	UILabel		*starLabel;
}


@property (nonatomic, retain) IBOutlet	UILabel		*starLabel;


@end
