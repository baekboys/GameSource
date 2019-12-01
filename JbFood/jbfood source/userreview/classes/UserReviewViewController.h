#import <UIKit/UIKit.h>
#import "DatabaseConnector.h"

//@class Restaurant;

@protocol userReviewWriteSuccess;


typedef enum
{
	ALERT_VIEW_TAG_TYPE_NON_REVIEW,	// 한줄평이 써있지 않는 상태
	ALERT_VIEW_TAG_TYPE_POSTED		// 이미 한줄평이 작성된 상태
}	ALERT_VIEW_TAG_TYPE;


@interface UserReviewViewController : UIViewController 
<
UIAlertViewDelegate,
sendUserReviewDelegate
>
{
	// UI
	UILabel		*label_star;
	UILabel		*label_review;
	
	UIButton	*button_star0;
	UIButton	*button_star1;
	UIButton	*button_star2;
	UIButton	*button_star3;
	UIButton	*button_star4;
	
	UILabel		*label_star0;
	UILabel		*label_star1;
	UILabel		*label_star2;
	UILabel		*label_star3;
	UILabel		*label_star4;
	
	UITextField *textField_review;
	
	UIActivityIndicatorView		*indicatorView;
	
	UIButton	*button_send;
	
	NSArray		*arrayLabels;
	
	// 별점
	NSUInteger	starValue;
	
	// 맛집데이터베이스의 id
	NSUInteger	restaurantId;
	
	// 데이터베이스 커넥터
	DatabaseConnector *dbConnector;
	
	// 성공적인 한줄평 알림 프로토콜 델리게이트
	id<userReviewWriteSuccess> userReviewWriteDelegate;
}


@property (nonatomic, retain) IBOutlet UILabel		*label_star;
@property (nonatomic, retain) IBOutlet UILabel		*label_review;
@property (nonatomic, retain) IBOutlet UIButton		*button_star0;
@property (nonatomic, retain) IBOutlet UIButton		*button_star1;
@property (nonatomic, retain) IBOutlet UIButton		*button_star2;
@property (nonatomic, retain) IBOutlet UIButton		*button_star3;
@property (nonatomic, retain) IBOutlet UIButton		*button_star4;
@property (nonatomic, retain) IBOutlet UILabel		*label_star0;
@property (nonatomic, retain) IBOutlet UILabel		*label_star1;
@property (nonatomic, retain) IBOutlet UILabel		*label_star2;
@property (nonatomic, retain) IBOutlet UILabel		*label_star3;
@property (nonatomic, retain) IBOutlet UILabel		*label_star4;
@property (nonatomic, retain)			UIActivityIndicatorView		*indicatorView;

@property (nonatomic, retain) IBOutlet UITextField	*textField_review;
@property (nonatomic, retain) IBOutlet UIButton		*button_send;

@property (nonatomic, retain)			NSArray		*arrayLabels;

@property (nonatomic)					NSUInteger	starValue;
@property (nonatomic)					NSUInteger	restaurantId;

@property (nonatomic, retain)			DatabaseConnector *dbConnector;

@property (nonatomic, assign)			id<userReviewWriteSuccess> userReviewWriteDelegate;

// 초기화 코드
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurantId:(NSUInteger)newRestaurantId;

// 별 버튼 이벤트
- (IBAction)touchStarButton:(id)sender;
- (void)drawActiveStar:(NSUInteger)number;

// 평가 버튼 이벤트
- (IBAction)touchReviewButton:(id)sender;

@end


@protocol userReviewWriteSuccess
- (void) userReviewWriteDidSuccess;
@end
