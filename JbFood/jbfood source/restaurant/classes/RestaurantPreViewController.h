#import <UIKit/UIKit.h>
#import "ButtonCell.h"
#import "ARViewController.h"
#import "UserReviewViewController.h"
#import "DatabaseConnector.h"

@class Restaurant;

// 테이블뷰의 메인 섹션 열거자
typedef enum
{
	PREVIEW_SECTION_MAININFO,
	PREVIEW_SECTION_BUTTON,
	PREVIEW_SECTION_USERREVIEW,
	PREVIEW_SECTION_TOTALCOUNT
}	PREVIEW_SECTION;


// 정보섹션의 열거자
typedef enum
{
	MAININFO_SECTION_STAR,
	MAININFO_SECTION_INFO,
	MAININFO_SECTION_REVIEW,
	MAININFO_SECTION_COUNT
}	MAININFO_SECTION;


// 사용자 한줄평 열거자
typedef enum
{
	USERREVIEW_SECTION_READ,
	USERREVIEW_SECTION_WRITE,
	USERREVIEW_SECTION_COUNT
}	USERREVIEW_SECTION;


@interface RestaurantPreViewController : UIViewController 
<
UITableViewDelegate, 
UITableViewDataSource,
buttonDidTouch,
UIActionSheetDelegate,
ARViewControllerDelegate,
userReviewWriteSuccess,
receiveRestaurantFromIdDelegate
>
{
	// UI
	UITableView		*previewTableView;	
	UIActivityIndicatorView		*indicatorView;
	
	// 뒤로가기 버튼을 생성할 것인가? - 증강현실을 위해서 필요
	BOOL				makeBackButtonForAR;
	
	// 단골맛집 버튼을 생성할 것인가?
	BOOL				makeJoinButtonForJoin;
	
	
	// 데이터베이스 커넥터
	DatabaseConnector	*dbConnector;
	
	
	// 맛집 데이터
	Restaurant		*restaurant;
}

@property (nonatomic, retain)		UITableView					*previewTableView;
@property (nonatomic, retain)		UIActivityIndicatorView		*indicatorView;
@property (nonatomic, retain)		DatabaseConnector			*dbConnector;
@property (nonatomic, retain)		Restaurant					*restaurant;
@property (nonatomic)				BOOL						makeBackButtonForAR;
@property (nonatomic)				BOOL						makeJoinButtonForJoin;


#pragma mark -
#pragma mark 맛집뷰 초기화
- (id)initWithRestaurant:(Restaurant*)newRestaurant;

#pragma mark -
#pragma mark 전화걸기 액션시트 만들기
- (void)makePhoneCallActionSheet;


#pragma mark -
#pragma mark 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;

#pragma mark -
#pragma mark 뒤로가기버튼 생성
// 뒤로가기버튼 만들기
- (void)makeBackButton;
// 뒤로가기버튼의 이벤트
- (void)touchBackButton:(id)sender;


@end
