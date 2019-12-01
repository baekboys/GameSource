#import <UIKit/UIKit.h>

@class UserReview;

@interface UserReviewDetailViewController : UIViewController 
{
	// UI
	UILabel		*starLabel;
	UITextView	*textView;
	
	// UserReview
	UserReview	*userReview;
}


@property (nonatomic, retain)	IBOutlet	UILabel		*starLabel;
@property (nonatomic, retain)	IBOutlet	UITextView	*textView;
@property (nonatomic, retain)				UserReview	*userReview;


// 초기화
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withUserReview:(UserReview*)newUserReview;

// 평균 별점으로부터 문자열 별점으로 가져오기
- (NSString*)getStarStringFromStar:(NSUInteger)newStarAverage;


@end
