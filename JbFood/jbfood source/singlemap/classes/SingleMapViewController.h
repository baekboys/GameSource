#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>

#pragma mark -
#pragma mark define
#define kSpanDistaneValueForMapView		350

@class Restaurant;

@interface SingleMapViewController : UIViewController <MKMapViewDelegate, CLLocationManagerDelegate, MKReverseGeocoderDelegate>
{
	// UI
	MKMapView	*mapView;
	UIButton	*button_myLocation;
	UIButton	*button_restaurantLocation;
	UILabel		*label_distance;
	
	// 맛집 데이터
	Restaurant	*restaurant;
	
	// 로케이션 매니저
	CLLocationManager *locationManager;
	
	// 내 위치
	CLLocationCoordinate2D myCoordinate;
}

#pragma mark -
#pragma mark property
@property (nonatomic, retain) IBOutlet	MKMapView	*mapView;
@property (nonatomic, retain) IBOutlet	UIButton	*button_myLocation;
@property (nonatomic, retain) IBOutlet	UIButton	*button_restaurantLocation;
@property (nonatomic, retain) IBOutlet	UILabel		*label_distance;
@property (nonatomic, retain)			Restaurant	*restaurant;
@property (nonatomic, retain)			CLLocationManager *locationManager;
@property (nonatomic)					CLLocationCoordinate2D myCoordinate;


#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurant:(Restaurant*)newRestaurant;

#pragma mark -
#pragma mark 지오코더로 부터 주소얻기 메소드
- (NSMutableString*)getAddressFromPlaceMark:(MKPlacemark *)placemark;

#pragma mark -
#pragma mark 버튼 이벤트 메소드
- (IBAction)touchButton:(id)sender;

@end
