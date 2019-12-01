#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import "DatabaseConnector.h"

#pragma mark -
#pragma mark define

// 슬라이더의 알파값
#define kSliderAlphaValueMin		0.47
#define kSliderAlphaValueMax		1.0

// 화면에 보이질 맵의 디테일값(미터단위)
#define kMapSpanValueMin			200
#define kMapSpanValueMax			8000

// 로케이션의 업데이트 허용치 (미터)
#define kMaximumDistanceFilterInMap	5.f

// 주소정보를 찾지 못하였을 경우의 현재위치와 맛집위치와의 거리차 허용범위
#define kMaximumDistanceRangeForAllRestaurantsForMap		10000		// 맵을 보여줄 최대거리보다 커야한다. - 10km
// 주소정보를 찾지 못하였을 경우의 MapLocation을 새로 만들기위한 값변화
#define kMaximumDistanceFilterForAllRestaurantsForMap		1000

@class MyLocation;
@class Restaurant;

@protocol MultiMapViewControllerDelegate;

@interface MultiMapViewController : UIViewController
<MKMapViewDelegate, 
CLLocationManagerDelegate, MKReverseGeocoderDelegate, 
UIAlertViewDelegate, 
receiveArrayRestaurantsFromAreaDelegate, receiveRestaurantFromIdDelegate, receiveAllRestaurantsDelegate
>
{
	// 종료 델리게이트
	id <MultiMapViewControllerDelegate>	delegate;
	
	// ---------------------
	//			UI
	// ---------------------
	
	// 맵뷰
	MKMapView					*mapView;
	
	// 슬라이더
	UISlider					*slider;
	
	// 맵뷰의 거리를 나타낼 레이블
	UILabel						*labelForDistance;
	
	// 액티비티 인디케이터 뷰
	UIActivityIndicatorView		*activityIndicatorView;
	
	// 알람뷰
	UIAlertView					*alertViewForDB;
	
	
	// -----------------
	//		로케이션
	// -----------------
	
	// 로케이션 매니저
	CLLocationManager			*locationManager;
	
	// 리버스 지오코더
	MKReverseGeocoder			*reverseGeocoder;

	// 현재 위치를 표시할 어노테이션
	MyLocation					*myLocation;
	
	
	// -------------------------
	//	맛집 정보를 담고 있는 array
	// -------------------------
	NSMutableArray				*arrayRestaurantAnnotations;

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
@property (nonatomic, assign)			id <MultiMapViewControllerDelegate>	delegate;
@property (nonatomic, retain) IBOutlet	MKMapView					*mapView;
@property (nonatomic, retain) IBOutlet	UISlider					*slider;
@property (nonatomic, retain) IBOutlet	UILabel						*labelForDistance;
@property (nonatomic, retain) IBOutlet	UIActivityIndicatorView		*activityIndicatorView;
@property (nonatomic, retain)			UIAlertView					*alertViewForDB;
@property (nonatomic, retain)			CLLocationManager			*locationManager;
@property (nonatomic, retain)			MKReverseGeocoder			*reverseGeocoder;
@property (nonatomic, retain)			MyLocation					*myLocation;
@property (nonatomic, retain)			NSMutableArray				*arrayRestaurantAnnotations;
@property (nonatomic, retain)			DatabaseConnector			*dbConnector;

@property (nonatomic, retain)	NSMutableArray					*arrayAllRestaurants;
@property (nonatomic)			CLLocationCoordinate2D			firstCenterCoord;
@property (nonatomic)			BOOL							isNotFoundAddressFromReversegeocoder;
@property (nonatomic)			BOOL							startFlag;


#pragma mark -
#pragma mark 어노테이션 관련 메소드
// 어노테이션 설정
- (void)setMyCenterLocation:(CLLocationCoordinate2D)newCoord;
// 맛집데이터로부터 MapLocation 추가하기
- (void)addMapLocationWithRestaurant:(Restaurant*)newRestaurant;
// 맛집데이터 array로부터 MapLocation 추가하기
- (void)addMapLocationWithArrayRestaurants:(NSMutableArray*)newArrayRestaurants;
// 모든 어노테이션뷰 삭제
- (void)removeAllMapLocationAndAnnotaionView;
// 어노테이션뷰의 버튼이벤트로부터 맛집뷰 보여주기
- (void)showRestaurantView:(id)sender;
// 어노테이션뷰 업데이트
- (void)updatingMapLocation;
// 거리 텍스트 얻기
- (NSString*)getStringForDistance:(double)distance;

#pragma mark -
#pragma mark 로케이션 매니저 관련 메소드
// 로케이션 매니저 업데이트 시작 및 종료
- (void)startLocationManagerUpdating;
- (void)stopLocationManagerUpdating;


#pragma mark -
#pragma mark 리버스 지오코더 관련 메소드
// 지오코더 제거
- (void)removeReverseGeocoder;
//지오코더로 부터 주소얻기 메소드
- (NSMutableString*)getAddressFromPlaceMark:(MKPlacemark *)placemark;


#pragma mark -
#pragma mark 알람뷰 메소드
// 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;

#pragma mark -
#pragma mark 슬라이더 이벤트
- (IBAction)changeSliderValue:(id)sender;
- (IBAction)touchDownSlider:(id)sender;
- (IBAction)touchUpSlider:(id)sender;
- (void)drawDistanceLabel;

#pragma mark -
#pragma mark 종료 메소드
- (void)MultiMapviewControllerDidFinish;


#pragma mark -
#pragma mark 주소정보 받지 못할 경우 필요한 메소드
// 주소정보를 받지 못해 모든 데이터로 시작해야하는 경우 발생하는 메소드
- (void)startAllRestaurnatDataForNotFoundAddress;
// 알람뷰 생성하기 - 리버스지오코더 실패
//- (void)makeAlertViewForGeocoder;
// 주소정보를 받지 못한경우 arrayAllRestaurants로부터 ARRestaurantView 재 생성
- (void)generateAnnotationViewFromArrayAllRestaurants;
// arrayAllRestaurants로부터 거리차 구하여 restaurant 얻기
- (NSMutableArray*)getRestaurantsFromArryAllRestaurant;


@end

#pragma mark -
#pragma mark 주변검색 맵뷰 종료 프로트콜
@protocol MultiMapViewControllerDelegate
- (void)MultiMapViewControllerDidFinish:(MultiMapViewController *)controller;
@end
