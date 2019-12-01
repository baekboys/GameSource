#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#pragma mark -
#pragma mark define
// 1라디안 = 180도 / PI
#define degreesToRadians(x) (M_PI * x / 180.0)	// 각 -> 라디안
#define radiansToDegrees(x) (x * (180.0/M_PI))	// 라디안 -> 각

// 각 컴포넌트들을 프로그래밍으로 생성하기 위하여 정의
#define kButtonWidth			120								// 버튼의 넓이
#define kButtonHeight			120								// 버튼의 높이
#define kButtonFontSize			20								// 버튼의 폰트 사이즈
#define kButtonImageFileName	@"ar.png"						// 버튼이미지 파일명
#define kLabelWidth				kButtonWidth					// 레이블의 널이
#define kLabelHeight			20								// 레이블의 높이
#define kLabelFontSize			25								// 레이블의 폰트 사이즈
#define kTotalViewWidth			kButtonWidth					// 뷰의 총 넓이 - 버튼의 넓이와 동일
#define kTotalViewHeight		kButtonHeight + kLabelHeight	// 븅의 총 높이 - 버튼의 높이 + 레이블의 높이


@class Restaurant;

@protocol ARRestaurantViewControllerTouchDelegate;

@interface ARRestaurantViewController : UIViewController 
{
	// 맛집이미지를 배경하는 하는 버튼
	UIButton	*imageButton;

	// 현재위치와 맛집의 거리차를 알려주는 레이블
	UILabel		*distanceLabel;
	
	// 인터페이스빌더를 이용하였는지 판단하는 플래그
	BOOL		isUseInterfaceBuilder;
	
	// 맛집 데이터
	Restaurant	*restaurantData;
	
	// 증강현실을 위한 데이터
	NSNumber	*radialDistance;		// 현재 위치로부터 이 맛집과의 거리
	double		azimuth;				// 현재 위치로부터 이 맛집의 방위각
	
	// 터치하였을경우 델이게이트
	id <ARRestaurantViewControllerTouchDelegate>	touchDelegate;
}

@property (nonatomic, retain)	IBOutlet	UIButton	*imageButton;
@property (nonatomic, retain)	IBOutlet	UILabel		*distanceLabel;
@property (nonatomic)						BOOL		isUseInterfaceBuilder;
@property (nonatomic, retain)				Restaurant	*restaurantData;

@property (nonatomic, retain)				NSNumber	*radialDistance;
@property (nonatomic)						double		azimuth;

@property (nonatomic, assign)	id <ARRestaurantViewControllerTouchDelegate>	touchDelegate;

#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithRestaurant:(Restaurant*)restaurant;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurant:(Restaurant*)restaurant;
- (void)makeButtonAndLabel;

#pragma mark -
#pragma mark 증강현실을 위한 메소드
// 아이폰위치로부터 이 맛집 증강현실뷰의 방위각 구하기
- (float)angleFromCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate;
// 아이폰위치로부터 이 맛집 증간현실뷰의 직선거리 구하기
- (void)calibrateFromCenterCoordinate:(CLLocationCoordinate2D)coordinate;
// 직선거리를 레이블에 표시
- (void)drawLabelFromDistance;

#pragma mark -
#pragma mark 터치 이벤트
- (IBAction)touchtButton:(id)sender;
@end


#pragma mark -
#pragma mark 맛집 증강현실뷰 터치 프로토콜 델리게이트
@protocol ARRestaurantViewControllerTouchDelegate
- (void)ARRestaurantViewControllerDidTouched:(ARRestaurantViewController*)controller;
@end
