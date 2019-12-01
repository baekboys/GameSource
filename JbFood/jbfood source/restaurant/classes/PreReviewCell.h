#import <UIKit/UIKit.h>


#define kPreReviewCellHeight	60


@interface PreReviewCell : UITableViewCell 
{
	UILabel		*reviewLabel;
}


@property (nonatomic, retain)	IBOutlet	UILabel		*reviewLabel;


@end
