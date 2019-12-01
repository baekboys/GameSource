#import <UIKit/UIKit.h>

#define kUserReviewResultCellHeight 50

@interface UserReviewResultCell : UITableViewCell 
{
	UILabel *starString;
	UILabel *userReview;
}

@property (nonatomic, retain) IBOutlet UILabel *starString;
@property (nonatomic, retain) IBOutlet UILabel *userReview;

@end
