#import <UIKit/UIKit.h>

// 셀 식별자
#define ImageReviewCellIdentifier	@"ImageReviewCellIdentifier"

// 셀의 높이
#define kImageReviewCellHeight		348


@interface ImageReviewCell : UITableViewCell 
{
	UIImageView		*restaurantImageView;
	UITextView		*reviewTextView;
}


@property (nonatomic, retain) IBOutlet	UIImageView		*restaurantImageView;
@property (nonatomic, retain) IBOutlet	UITextView		*reviewTextView;


@end
