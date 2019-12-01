#import <UIKit/UIKit.h>
#import "ARViewController.h"
#import "SQLiteDatabaseConnector.h"
#import "UserReviewViewController.h"
#import "DatabaseConnector.h"

// ----------------------------------------	
//	테이블뷰 섹션구성 열거자
//	맛집 뷰의 섹션은 열거자의 형태로 구성되어 있다.
//	세부사항은 각 섹션별로 cell단위로,
//	다시 작게 구성되어있다.
// ----------------------------------------
typedef enum
{
	TABLE_SECTION_ADDRESS,						// 주소 섹션
	TABLE_SECTION_RECOMMENDATION_MENU,			// 추천메뉴 섹션
	TABLE_SECTION_INFO,							// 정보 섹션
	TABLE_SECTION_ETC,							// 기타 섹션
	TABLE_SECTION_IMAGE_AND_REVIEW,				// 이미지와 리뷰 섹션
//	TABLE_SECTION_USER_REVIEW,					// 사용자 평가 섹션
	TABLE_SECTION_COUNT
}	TABLE_SECTION;


// 주소 섹션 안의 구성 열거자
typedef enum
{
	ADDRESS_SECTION_ADDRESS_FEILD,
	ADDRESS_SECTION_PHONE_FIELD,
	ADDRESS_SECTION_COUNT
}	ADDRESS_SECTION;


// 이미지와 리뷰섹션은 하나로 구성되어 있어서 구성열거자가 없음.!!


// 추천메뉴 섹션 안의 구성 열거자
typedef enum
{
	RECOMMENDATION_MENU_TITLE,					// 추천메뉴 타이틀 - (추천메뉴, 가격)
	RECOMMENDATION_MENU_FEILD,					// 실제 추천메뉴가 들어갈 공간
	RECOMMENDATION_MENU_COUNT
}	RECOMMENDATION_MENU_SECTION;


// 정보 섹션 안의 구성 열거자
typedef enum
{
	INFO_SECTION_TIME,							// 영업시간
	INFO_SECTION_ACCOMODATION,					// 좌석수
	INFO_SECTION_CAR_SUPPORT,					// 차량지원
	INFO_SECTION_RESERVATION,					// 예약여부
	INFO_SECTION_HOLYDAY,						// 공휴일
	INFO_SECTION_PARKING,						// 주차공간
	INFO_SECTION_COUNT
}	INFO_SECTION;


// 기타정보 섹션 안의 구성 열거자
typedef enum
{
	ETC_SECTION_1,								// 기타정보1
	ETC_SECTION_2,								// 기타정보2
	ETC_SECTION_COUNT
}	ETC_SECTION;


//// 맛집 평가 섹션 안의 구성 열거자
//typedef enum
//{
//	USER_REVIEW_STAR,							// 별점
//	USER_REVIEW_READ,							// 평가 리뷰 보기
//	USER_REVIEW_WRITE,							// 평가 쓰기
//	USER_REVIEW_COUNT
//}	USER_REVIEW_SECTION;

// 섹션 이름
#define TABLE_SECTION_NAME_ADDRESS				@"주소 및 전화번호"
#define TABLE_SECTION_NAME_RECOMMENDATION_MENU	@"추천메뉴"
#define TABLE_SECTION_NAME_IMAGE_REVIEW			@"사진 및 리뷰"
#define TABLE_SECTION_NAME_INFO					@"세부정보"
#define TABLE_SECTION_NAME_ETC					@"기타정보"
//#define TABLE_SECTION_NAME_USER_REVIEW			@"맛집톡톡"

// 추천메뉴 섹션의 row 이름
#define RECOMMENDATION_MENU_NAME_TITLE			@"메   뉴"
#define RECOMMENDATION_MENU_NAME_PRICE			@"가   격"


// 정보섹션의 row 이름
#define INFO_SECTION_NAME_TIME					@"영업시간"
#define INFO_SECTION_NAME_ACCOMODATION			@"좌 석 수"
#define INFO_SECTION_NAME_CAR_SUPPORT			@"차량지원"
#define INFO_SECTION_NAME_RESERVATION			@"예약가능"
#define INFO_SECTION_NAME_HOLYDAY				@"휴   일"
#define INFO_SECTION_NAME_PARKING				@"주차공간"


// 맛집뷰의 크기
#define RESTAURANT_MAINVIEW_WIDTH				320
#define RESTAURANT_MAINVIEW_HEIGHT				480


// --------------------------------------------
//  지도보기, 증강현실, 전화걸기 액션시트를 위한 태그값
// --------------------------------------------
typedef enum
{
	ACTIONSHEET_TAG_GEO,						// 지도보기, 증강현실
	ACTIONSHEET_TAG_CALL,						// 전화걸기
	ACTIONSHEET_TAG_JOIN,						// 단골맛집 추가
	ACTIONSHEET_TAG_JOIN_RESULT					// 단골맛집 추가한 결과
}	ACTIONSHEET_TAG;


// ---------------------------------------------
//					맛집클래스
// ---------------------------------------------
@class Restaurant;


@interface RestaurantViewController : UIViewController 
<
UIActionSheetDelegate, 
ARViewControllerDelegate, 
UITableViewDelegate, 
UITableViewDataSource,
userReviewWriteSuccess,
receiveRestaurantFromIdDelegate
>
{			
	// 맛집정보를 가진 객체
	Restaurant			*restaurant;
	
	// 뒤로가기 버튼을 생성할 것인가? - 증강현실을 위해서 필요
	BOOL				makeBackButtonForAR;
	
	// 단골맛집 버튼을 생성할 것인가?
	BOOL				makeJoinButtonForJoin;
	
	// UI
	UITableView			*restaurantTableView;
	UIActivityIndicatorView		*indicatorView;
	
	// 데이터베이스 커넥터
	DatabaseConnector	*dbConnector;
	
	
}


@property (nonatomic, retain)			Restaurant			*restaurant;
@property (nonatomic)					BOOL				makeBackButtonForAR;
@property (nonatomic)					BOOL				makeJoinButtonForJoin;
@property (nonatomic, retain) IBOutlet  UITableView			*restaurantTableView;
@property (nonatomic, retain)			UIActivityIndicatorView		*indicatorView;
@property (nonatomic, retain)			DatabaseConnector	*dbConnector;


#pragma mark -
#pragma mark 맛집뷰 초기화
- (id)initWithRestaurant:(Restaurant*)newRestaurant;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurant:(Restaurant*)newRestaurant;


#pragma mark -
#pragma mark 맛집뷰 이벤트
// 맛집의 위치를 보여줄 싱글맵뷰 만들기
- (void)makeMapView;
// 맛집의 증강현실 위치를 보여줄 증강현실뷰 만들기
- (void)makeARView;
// 뒤로가기버튼 만들기
- (void)makeBackButton;
// 뒤로가기버튼의 이벤트
- (void)touchBackButton:(id)sender;
// 단골맛집 버튼 만들기
- (void)makeJoinButton;
// 단골맛집 버튼의 이벤트
- (void)touchJoinButton:(id)sender;
// 단골맛집 추가하기
- (void)addEnjoyedRestaurant;


#pragma mark -
#pragma mark 액션시트 만들기
// 지도보기, 증강현실보기 액션시트 만들기
- (void)makeGeoActionSheet;
// 전화걸기 액션시트 만들기
- (void)makePhoneCallActionSheet;
// 단골맛집 액션시트 만들기
- (void)makeJoinActionSheet;
// 단골맛집 추가한 결과 액션시트 만들기
- (void)makeJoinResultActionSheet:(ADD_RETURN_TYPE)result;

#pragma mark -
#pragma mark 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;


@end
