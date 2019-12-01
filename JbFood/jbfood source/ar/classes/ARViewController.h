#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import <MapKit/MapKit.h>
#import "DatabaseConnector.h"
#import "ARRestaurantViewController.h"

#pragma mark -
#pragma mark define

// 디버그 모드
//#define DEBUG_MODE

// 모든맛집 검색 모드
//#define ALL_RESTAURANT_MODE

// 인터페이스빌더로 생성할 것인가?
//#define USE_INTERFACE_BUILDER

// ARViewController의 뷰 기본 크기
#define ARVIEW_WIDTH								320
#define ARVIEW_HEIGHT								480

// ARRootView의 뷰 크기
#define ARROOTVIEW_WIDTH							400
#define ARROOTVIEW_HEIGHT							600

// 증강현실 카메라의 화면 각
#define VIEWPORT_WIDTH_RADIANS						0.5
#define VIEWPORT_HEIGHT_RADIANS						0.75


// 뒤로가기 버튼의 사이즈와 위치
#define BUTTON_BACK_WIDTH							100
#define BUTTON_BACK_HEIGHT							50
#define BUTTON_BACK_POINT_X							10
#define BUTTON_BACK_POINT_Y							260


// 슬라이더 사이즈와 위치
#define SLIDER_WIDTH								300
#define SLIDER_HEIGHT								50
#define SLIDER_POINT_X								140
#define SLIDER_POINT_Y								260
// 슬라이더의 알파값
#define kARSliderAlphaValueMin						0.47
#define kARSliderAlphaValueMax						1.0


// 레이블 사이즈와 위치
#define LABEL_WIDTH									150
#define LABEL_HEIGHT								30
#define LABEL_POINT_X								((SLIDER_POINT_X + SLIDER_WIDTH + SLIDER_POINT_X) / 2) - (LABEL_WIDTH / 2)
#define LABEL_POINT_Y								SLIDER_POINT_Y - LABEL_HEIGHT
#define LABEL_FORMAT_FOR_METER						@"반경 : %dm"
#define LABEL_FORMAT_FOR_KILO_METER					@"반경 : %0.2fkm"
#define kLABEL_FONT_SIZE							20

// 각종 증강현실 제어 값
#define kMinimumScaleFactor							0.7			// 가장 멀리있는 맛집 증강현실뷰의 스케일값
#define kMaximumRotationAngle						M_PI / 4.0	// 좌우에 있을때의 맛집 증강현실뷰의 회전값
#define kRotateViewsBasedOnPerspective				NO			// 좌우회전을 할것인지 결정하는 플래그
#define kMaximumDistanceRange						5000		// 증강현실을 보여줄 최대 거리
#define kMinimumDistanceRange						200			// 증강현실을 보여줄 최소 거리

// 뒤로가기 버튼 이미지 파일 명
#define kBackButtonForArImageFileName				@"back_button.png"	// 버튼이미지 파일명
// 뒤로가기 버튼 사이즈
#define kBackButtonFontSize							15


// 주소정보를 찾지 못하였을 경우의 현재위치와 맛집위치와의 거리차 허용범위
#define kMaximumDistanceRangeForAllRestaurants		10000		// 증강현실 보여줄 최대거리보다 커야한다. - 10km
// 주소정보를 찾지 못하였을 경우의 ARResturantView를 새로 만들기위한 값변화
#define kMaximumDistanceFilterForAllRestaurants		1000		// fistCenterCoord가 1km이상 차이나면 ARRestaurantView를 새로 생성한다.

// 가장멀리있는 맛집증강현실뷰는 화면의 약간 위에 그려져야 한다. 따라서 얼만큼 위로 올려서 그릴지 결졍하는 값.
#define kMaximumDistanceFactorBasedOnPerspective	100

// 로케이션의 업데이트 허용치 (미터)
#define kMaximumDistanceFilterInAR					20.f

@class Restaurant;
@class ARRestaurantViewController;

@protocol ARViewControllerDelegate;

#pragma mark -
#pragma mark interface
@interface ARViewController : UIViewController 
<UIAccelerometerDelegate, CLLocationManagerDelegate, MKReverseGeocoderDelegate, 
UIAlertViewDelegate, 
receiveArrayRestaurantsFromAreaDelegate, receiveRestaurantFromIdDelegate, receiveAllRestaurantsDelegate,
ARRestaurantViewControllerTouchDelegate
>
{
	// -------------------
	//	카메라 및 증강현실 뷰 
	// -------------------
	
	// 이미지 피커 컨트롤러
	UIImagePickerController			*cameraController;

	// 카메라뷰의 서브뷰로 쓰일 오버레이뷰 - 이곳에 증강현실뷰를 추가
	UIView							*cameraOverlayView;
	
	// 뒤로가기 버튼
	UIButton						*button_back;
		
	// 증강현실 최소거리를 위한 슬라이더
	UISlider						*distanceSlider;
	
	// 증강현실 최소거리를 나타내는 레이블
	UILabel							*distanceLabel;
	
	// 최상단 증강현실 뷰
	UIView							*arRootView;
	
	// 액티비티 인디케이터 뷰
	UIActivityIndicatorView			*activityIndicatorView;
	
	// 증강현실뷰를 저장할 array
	NSMutableArray					*arrayARRestaurantViews;
	
	// 종료 델리게이트
	id <ARViewControllerDelegate>	delegate;
	
	// 종료 플래그
	BOOL							wantStop;
	
	// 알람뷰
	UIAlertView						*alertViewForDB;

	
		
#ifdef DEBUG_MODE
	// 디버깅 레이블
	UILabel							*debugLabel;
#endif
	
	// ------------------------
	//	증강현실 구현에 필요한 변수
	// ------------------------
	
	// ar_coordinates에 있는 ARCoordinate 인스턴스들 중에서 가장 먼 거리
	double							maximumScaleDistance;
	
	// 최소 스케일
	double							minimumScaleFactor;
	
	// 최대 회전각
	double							maximumRotationAngle;
		
	// 뷰를 회전시킬것인가?
	BOOL							rotateViewsBasedOnPerspective;
	
	// 현재의 위도경도를 나타내는 변수
	CLLocationCoordinate2D			centerCoord;
	
	// 현재 경사각을 나타내는 변수
	double							centerInclination;
	
	// 현재 방위각을 나타내는 변수
	double							centerAzimuth;
	
	// 증강현실 보여줄 범위
	double							maximumDistanceRange;
	
	// 맛집증강현실뷰를 한개만 이용할 것인지 판단하는 플래그
	BOOL							isSingleAR;
	
	// ---------------------
	//	코어로케이션과 가속센서
	// ---------------------

	// 로케이션 매니저
	CLLocationManager				*locationManager;
	
	// 리버스 지오코더
	MKReverseGeocoder				*reverseGeocoder;
		
	// 가속센서
	UIAccelerometer					*accelerometerManager;
	
	
	// -----------------
	//	데이터베이스 커넥터
	// -----------------
	DatabaseConnector			*dbConnector;
	
	
	// ---------------------------------------------------
	//	주소정보를 가져오지 못하였을 경우 모든 맛집 정보를 가져온다.
	// ---------------------------------------------------
	NSMutableArray				*arrayAllRestaurants;

	// ------------------------------------------------------------------------------------------------------
	//	주소정보를 얻어오지 못한다면 전체의 맛집데이터베이스를 가져와서 현재위치와 맛집의 위치의 거리차를 이용하여 계산하여야 한다.
	//	한번에 10km이상은 이동하지 못한다는 가정을하고, 모든 데이터베이스에서 10km이하의 거리만 ARRestaurnatView로 만든다.
	//	거리차가 1km이상 발생하였을경우에만 모든 맛집데이터에서 다시 ARRestaurnatView를 새로 만든다.
	// ------------------------------------------------------------------------------------------------------
	CLLocationCoordinate2D		firstCenterCoord;
	BOOL						isNotFoundAddressFromReversegeocoder;

	// 최초업데이트 플래그
	BOOL						startFlag;
}


#pragma mark -
#pragma mark property
@property (nonatomic, retain)	UIImagePickerController			*cameraController;
@property (nonatomic, retain)	UIView							*cameraOverlayView;
@property (nonatomic, retain)	UIButton						*button_back;
@property (nonatomic, retain)	UISlider						*distanceSlider;
@property (nonatomic, retain)	UILabel							*distanceLabel;
@property (nonatomic, retain)	UIView							*arRootView;
@property (nonatomic, retain)	UIActivityIndicatorView			*activityIndicatorView;
@property (nonatomic, retain)	UIAlertView						*alertViewForDB;
@property (nonatomic, retain)	NSMutableArray					*arrayARRestaurantViews;
@property (nonatomic, assign)	id <ARViewControllerDelegate>	delegate;
@property (nonatomic)			BOOL							wantStop;
@property (nonatomic)			double							maximumScaleDistance;
@property (nonatomic)			double							minimumScaleFactor;
@property (nonatomic)			double							maximumRotationAngle;
@property (nonatomic)			BOOL							rotateViewsBasedOnPerspective;
@property (nonatomic)			CLLocationCoordinate2D			centerCoord;
@property (nonatomic)			double							centerInclination;
@property (nonatomic)			double							centerAzimuth;
@property (nonatomic)			double							maximumDistanceRange;
@property (nonatomic)			BOOL							isSingleAR;
@property (nonatomic, retain)	CLLocationManager				*locationManager;
@property (nonatomic, retain)	MKReverseGeocoder				*reverseGeocoder;
@property (nonatomic, retain)	UIAccelerometer					*accelerometerManager;
@property (nonatomic, retain)	DatabaseConnector				*dbConnector;

@property (nonatomic, retain)	NSMutableArray					*arrayAllRestaurants;
@property (nonatomic)			CLLocationCoordinate2D			firstCenterCoord;
@property (nonatomic)			BOOL							isNotFoundAddressFromReversegeocoder;
@property (nonatomic)			BOOL							startFlag;

#ifdef DEBUG_MODE
@property (nonatomic, retain)	UILabel							*debugLabel;
#endif

#pragma mark -
#pragma mark 각종 매니저와 뷰 관련 메소드
// 생성 - 맛집증강현실뷰를 한개로만 이용할 것지 결정
- (id)initWithUsingSingleAR:(BOOL)single;
// 인터페이스빌더로 생성
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withUsingSingleAR:(BOOL)single;
// 초기화 하기
- (void)initAllInstance;
// 이미지피커와 오버레이뷰 만들기
- (void)makeImagePickerAndOverlayView;
// 로케이션 매니저와 가속센서 매니저 만들기
- (void)makeLocationAndAccelManager;
// 로케이션 매니저 업데이트 시작 및 종료
- (void)startLocationManagerUpdating;
- (void)stopLocationManagerUpdating;

#pragma mark -
#pragma mark 증강현실뷰 추가 삭제
// centerLocation setter 메소드
- (void)setMyCenterLocation:(CLLocationCoordinate2D)newCoord;
// 증강현실뷰 추가 - 맛집 데이터를 한개씩 추가
- (void)addARRestaurantViewWithRestaurant:(Restaurant *)newRestaurant;
// 증강현실뷰 추가 - 맛집 array로 부터 추가
- (void)addARRestaurantViewWithRestaurantArrays:(NSMutableArray *)restaurantsArray;
// 증강현실뷰 모두 삭제
- (void)removeAllARRestaurantViews;


#pragma mark -
#pragma mark 증강현실관련 메소드
// 증강현실뷰 업데이트
- (void)updateARRestaurantViews;
// 맛집 증강현실뷰가 뷰포트에 속해있는지 확인하는 메소드
- (BOOL)isContainsInViewPort:(ARRestaurantViewController *)controller;
// ARRoot뷰안의 맛집 증강현실뷰 위치
- (CGPoint)pointInARRootView:(ARRestaurantViewController *)controller;
// 증강현실뷰 정렬 메소드 - radialDistance를 기준으로 하여 정렬
- (void)sortingARRestaurantViewByRadialDistance;
// 맛집뷰에서 되돌아오기위한 네비게이션바의 뒤로가기 버튼 이벤트
- (void)touchButton_backFromRestaurantView:(id)sender;
// 증강현실뷰 슬라이더 값 변화
- (void)changeSliderValue:(id)sender;
// 슬라이더 터치 이벤트
- (void)touchDownSlider:(id)sender;
- (void)touchUpSlider:(id)sender;
// 레이블 값 그리기
- (void)drawDistanceLabel;


#pragma mark -
#pragma mark 리버스 지오코더 관련 메소드
// 지오코더 제거
- (void)removeReverseGeocoder;

#pragma mark -
#pragma mark 종료 메소드
- (void)viewControllerDidFinish;
// 종료버튼 이벤트
- (void)touchButton_back:(id)sender;

#pragma mark -
#pragma mark 알람뷰 메소드
// 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;


#pragma mark -
#pragma mark 주소정보 받지 못할 경우 필요한 메소드
// 주소정보를 받지 못해 모든 데이터로 시작해야하는 경우 발생하는 메소드
- (void)startAllRestaurnatDataAR;
// 주소정보를 받지 못한경우 arrayAllRestaurants로부터 ARRestaurantView 재 생성
- (void)generateARRestaurantViewFromArrayAllRestaurants;
// arrayAllRestaurants로부터 거리차 구하여 restaurant 얻기
- (NSMutableArray*)getRestaurantsFromArryAllRestaurant;


@end

#pragma mark -
#pragma mark 증강현실뷰 종료 프로트콜
@protocol ARViewControllerDelegate
- (void)ARViewControllerDidFinish:(ARViewController *)controller;
@end


