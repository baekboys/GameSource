#import "SingleMapViewController.h"
#import "Restaurant.h"
#import "MapLocation.h"

@implementation SingleMapViewController

#pragma mark -
#pragma mark synthesize
@synthesize mapView;
@synthesize restaurant;
@synthesize button_myLocation;
@synthesize button_restaurantLocation;
@synthesize label_distance;
@synthesize locationManager;
@synthesize myCoordinate;


#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurant:(Restaurant*)newRestaurant
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		self.restaurant = newRestaurant;
    }
    return self;
}


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
    
}


- (void)viewDidLoad
{
	// ------------------------
	//		타이틀과 폰트 설정
	// ------------------------
	self.title = @"지도 보기";
	
	// 네비게이션바의 타이틀뷰 설정
	UILabel *titleLabel = [[UILabel alloc] init];
	titleLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:JBFOOD_NAVIGATION_TITLE_FONT_SIZE];
	titleLabel.text = self.title;
	titleLabel.backgroundColor = [UIColor clearColor];
	[titleLabel sizeToFit];
	self.navigationItem.titleView = titleLabel;
	[titleLabel release];
	
	
	// 거리 레이블의 폰트 설정
	self.label_distance.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.label_distance.font.pointSize];
	
	// 버튼의 폰트 설정
	self.button_myLocation.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_myLocation.font.pointSize];
	self.button_restaurantLocation.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_restaurantLocation.font.pointSize];
	
	
	// --------------------
	//		맵뷰 설정하기
	// --------------------
	
	// 맵뷰 델리게이트 설정
	self.mapView.delegate = self;

	// 위도, 경도 설정
	CLLocationCoordinate2D center;
	center.latitude = self.restaurant.latitude;
	center.longitude = self.restaurant.longitude;

	// 맵뷰에 좌표지역 설정하기
	MKCoordinateRegion viewRegion = MKCoordinateRegionMakeWithDistance(center, kSpanDistaneValueForMapView, kSpanDistaneValueForMapView);
	//MKCoordinateRegion adjustedRegion = [mapView regionThatFits:viewRegion];
    [mapView setRegion:viewRegion animated:YES];
	
	
	// ----------------------------------
	//		맵뷰에 맛집 어노테이션 표시 하기
	// ----------------------------------

	// 맛집의 위치어노테이션 생성
	MapLocation *restaurantlocation = [[MapLocation alloc] init];
	
	// 정보 입력
	restaurantlocation.name = self.restaurant.name;
	restaurantlocation.address = self.restaurant.address;
	CLLocationCoordinate2D coord;
	coord.latitude = self.restaurant.latitude;
	coord.longitude = self.restaurant.longitude;
	restaurantlocation.coordinate = coord;
	
	// 맵뷰에 어노테이션추가
	[self.mapView addAnnotation:restaurantlocation];
	
	// 릴리즈
	[restaurantlocation release];
	
	
	// -----------------------------------------------------------
	//	로케이션 매니저가 생성이 가능하다면 생성 - 자기 위치를 파악하기 위하여
	// -----------------------------------------------------------
	
	// UI를 숨겨놓는따. 위치파악이 된다면 차후에 UI를 화면에 보이게 한다.
	self.button_myLocation.hidden = YES;
	self.button_restaurantLocation.hidden = YES;
	self.label_distance.hidden = YES;
	
	if ([CLLocationManager locationServicesEnabled])
	{
		// 로케이션 매니저 생성.
		CLLocationManager *locationManagerForSigleMap = [[CLLocationManager alloc] init];
		self.locationManager = locationManagerForSigleMap;
		[locationManagerForSigleMap release];

		// 로케이션 매니저 설정.
		self.locationManager.delegate = self;
		self.locationManager.desiredAccuracy = kCLLocationAccuracyBest;
		self.locationManager.distanceFilter = kCLDistanceFilterNone;
		
		// 로케이션 업데이트 실시
		[self.locationManager startUpdatingLocation];
	}
}


- (void)viewDidUnload 
{
    [super viewDidUnload];
	
//	self.mapView = nil;
//	self.button_myLocation = nil;
//	self.button_restaurantLocation = nil;
//	self.label_distance = nil;

	self.mapView = nil;
	self.button_myLocation = nil;
	self.button_restaurantLocation = nil;
	self.label_distance = nil;
	self.restaurant = nil;
	self.locationManager = nil;
}


- (void)dealloc
{
	self.mapView = nil;
	self.button_myLocation = nil;
	self.button_restaurantLocation = nil;
	self.label_distance = nil;
	self.restaurant = nil;
	self.locationManager = nil;
	
    [super dealloc];
}


#pragma mark -
#pragma mark 맵 델리게이트 메소드
- (MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation
{
	if ([annotation isKindOfClass:[MapLocation class]]) 
	{
	    static NSString *mapLocationIdentifier = @"MapLocationIdentifier";
		
		// plcaemarkIdentifier값을 통해서 어노테이션 뷰를 가져온다.
		MKAnnotationView *annotationView = (MKAnnotationView *)[self.mapView dequeueReusableAnnotationViewWithIdentifier:mapLocationIdentifier];
		
		// 어노테이션뷰가 없다면 새로 생성
		if (annotationView == nil)  
		{
			annotationView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:mapLocationIdentifier] autorelease];
            annotationView.image = [UIImage imageNamed:@"annotation.png"];
			annotationView.canShowCallout = YES;
			
            return annotationView;
        }            
        else
		{
            annotationView.annotation = annotation;
		}
        
        return annotationView;
    }
	
	
    return nil;
}


#pragma mark -
#pragma mark 로케이션 매니저 델리게이트 메소드
- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation 
{	
	// 로케이션매니저 업데이트 중지
	[manager stopUpdatingLocation];
	manager.delegate = nil;
	
	// 위치 저장
	self.myCoordinate = newLocation.coordinate;
	
	// 리버스 지오코더 생성
	MKReverseGeocoder *geoCoder = [[MKReverseGeocoder alloc] initWithCoordinate:newLocation.coordinate];
	geoCoder.delegate = self;
	[geoCoder start];
}


#pragma mark -
#pragma mark 리버스 지오코더(주소찾기) 델리게이트 메소드
- (void)reverseGeocoder:(MKReverseGeocoder *)geocoder didFindPlacemark:(MKPlacemark *)placemark
{
	// --------------
	//	지오코더 릴리즈
	// --------------
	[geocoder cancel];
	[geocoder release];
	
	// -------------------------
	//	현재 위치의 어노테이션 생성
	// -------------------------
	MapLocation *mylocation = [[MapLocation alloc] init];
	
	// 정보 입력
	mylocation.name = @"현재 위치";
	mylocation.address = [self getAddressFromPlaceMark:placemark];
	CLLocationCoordinate2D coord;
	coord.latitude = self.myCoordinate.latitude;
	coord.longitude = self.myCoordinate.longitude;
	mylocation.coordinate = coord;
	
	// 맵뷰에 어노테이션추가
	[self.mapView addAnnotation:mylocation];
	
	// 릴리즈
	[mylocation release];
	
	
	// -----------
	//	버튼 활성화
	// -----------
	self.button_myLocation.hidden = NO;
	self.button_restaurantLocation.hidden = NO;
	self.label_distance.hidden = NO;
	
	// ---------
	//	거리 계산
	// ---------
	
	// 거리계산을 위하여 맛집과 현재위치의 로케이션 생성
	CLLocation *restaurantLocation = [[CLLocation alloc] initWithLatitude:restaurant.latitude longitude:restaurant.longitude];
	CLLocation *myLocation = [[CLLocation alloc] initWithLatitude:self.myCoordinate.latitude longitude:self.myCoordinate.longitude];
	
	// 거리계산
	double distance = [restaurantLocation distanceFromLocation:myLocation];
	
	NSString *label_text = nil;
	
	// 거리가 1km 이상이면 km단위로 표시
	if (distance >= 1000)
	{
		label_text = [NSString stringWithFormat:@"거리차 : %.1fkm", distance / 1000.f];
	}
	// m단위로 표시
	else
	{
		label_text = [NSString stringWithFormat:@"거리차 : %dm", (int)distance];
	}

	// 레이블 텍스트 설정
	self.label_distance.text = label_text;
	
	// 릴리즈
	[restaurantLocation release];
	[myLocation release];
}


- (void)reverseGeocoder:(MKReverseGeocoder *)geocoder didFailWithError:(NSError *)error
{
	// 지오코더 릴리즈
	[geocoder cancel];
	[geocoder release];
	
	// -------------------------
	//	현재 위치의 어노테이션 생성
	// -------------------------
	MapLocation *mylocation = [[MapLocation alloc] init];
	
	// 정보 입력
	mylocation.name = @"현재 위치";
	mylocation.address = @"주소정보를 찾을수 없습니다.";
	CLLocationCoordinate2D coord;
	coord.latitude = self.myCoordinate.latitude;
	coord.longitude = self.myCoordinate.longitude;
	mylocation.coordinate = coord;
	
	// 맵뷰에 어노테이션추가
	[self.mapView addAnnotation:mylocation];
	
	// 릴리즈
	[mylocation release];
	
	
	// -----------
	//	버튼 활성화
	// -----------
	self.button_myLocation.hidden = NO;
	self.button_restaurantLocation.hidden = NO;
	self.label_distance.hidden = NO;
	
	// ---------
	//	거리 계산
	// ---------
	
	// 거리계산을 위하여 맛집과 현재위치의 로케이션 생성
	CLLocation *restaurantLocation = [[CLLocation alloc] initWithLatitude:restaurant.latitude longitude:restaurant.longitude];
	CLLocation *myLocation = [[CLLocation alloc] initWithLatitude:self.myCoordinate.latitude longitude:self.myCoordinate.longitude];
	
	// 거리계산
	double distance = [restaurantLocation distanceFromLocation:myLocation];
	
	NSString *label_text = nil;
	
	// 거리가 1km 이상이면 km단위로 표시
	if (distance >= 1000)
	{
		label_text = [NSString stringWithFormat:@"거리차 : %.1fkm", distance / 1000.f];
	}
	// m단위로 표시
	else
	{
		label_text = [NSString stringWithFormat:@"거리차 : %dm", (int)distance];
	}
	
	// 레이블 텍스트 설정
	self.label_distance.text = label_text;
	
	// 릴리즈
	[restaurantLocation release];
	[myLocation release];
	
}


#pragma mark -
#pragma mark 지오코더로 부터 주소얻기 메소드
- (NSMutableString*)getAddressFromPlaceMark:(MKPlacemark *)placemark
{
	NSMutableString *address = [NSMutableString stringWithString:@""];
	
	if (placemark.administrativeArea)
	{
		[address appendString:placemark.administrativeArea];
	}
	
	if (placemark.subAdministrativeArea)
	{
		[address appendFormat:@" %@", placemark.subAdministrativeArea];
	}
	
	if (placemark.locality)
	{
		[address appendFormat:@" %@", placemark.locality];
	}
	
	if (placemark.subLocality)
	{
		[address appendFormat:@" %@", placemark.subLocality];
	}
	
	if (placemark.thoroughfare)
	{
		[address appendFormat:@" %@", placemark.thoroughfare];
	}

	if (placemark.subThoroughfare)
	{
		[address appendFormat:@" %@", placemark.subThoroughfare];
	}
	
	return address;
}


#pragma mark -
#pragma mark 버튼 이벤트 메소드
- (IBAction)touchButton:(id)sender
{
	// 위도, 경도 설정
	CLLocationCoordinate2D center;
	
	// 내위치 버튼을 눌렸다면
	if (sender == self.button_myLocation)
	{
		center.latitude = self.myCoordinate.latitude;
		center.longitude = self.myCoordinate.longitude;
	}
	// 맛집 버튼을 눌렀다면
	else if (sender == self.button_restaurantLocation)
	{
		center.latitude = self.restaurant.latitude;
		center.longitude = self.restaurant.longitude;
	}
		
	// 맵뷰에 중앙으로 설정. 기존의 축적값을 이용한다.
	MKCoordinateRegion viewRegion = MKCoordinateRegionMake(center, self.mapView.region.span);
    [mapView setRegion:viewRegion animated:YES];
}


@end
