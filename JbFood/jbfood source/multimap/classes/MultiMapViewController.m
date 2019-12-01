#import "MultiMapViewController.h"
#import "MyLocation.h"
#import "MapLocation.h"
#import "Restaurant.h"
#import "RestaurantViewController.h"

@implementation MultiMapViewController


@synthesize delegate;
@synthesize mapView, slider, labelForDistance, activityIndicatorView, alertViewForDB;
@synthesize locationManager, reverseGeocoder;;
@synthesize myLocation;
@synthesize arrayRestaurantAnnotations;
@synthesize dbConnector;
@synthesize arrayAllRestaurants, firstCenterCoord, isNotFoundAddressFromReversegeocoder;
@synthesize startFlag;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		self.title = @"주변 검색";
		
		// 로케이션 매니저가 이용하능하다면
		if ([CLLocationManager locationServicesEnabled])
		{
			// 로케이션 매니저 생성
			CLLocationManager *locationManagerForMultiMap = [[CLLocationManager alloc] init];
			self.locationManager = locationManagerForMultiMap;
			[locationManagerForMultiMap release];
			
			// 로케이션 매니저가 생성되었으므로 맛집정보를 저장할 array초기화
			NSMutableArray *arrayForRestaurantAnnotations = [[NSMutableArray alloc] init];
			self.arrayRestaurantAnnotations = arrayForRestaurantAnnotations;
			[arrayForRestaurantAnnotations release];
			
			// 변수 초기화
			// 현재 좌표 초기화
			firstCenterCoord.latitude = 0;
			firstCenterCoord.longitude = 0;
			
			// 시작 플래그
			startFlag = NO;
			self.isNotFoundAddressFromReversegeocoder = NO;
			
		}
	}
	return self;
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
    
}


- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	// 로케이션 매니저가 이용하능하다면
	if ([CLLocationManager locationServicesEnabled])
	{
		// 맵뷰 설정
		self.mapView.scrollEnabled = NO;
		self.mapView.zoomEnabled = NO;
		self.mapView.delegate = self;
		
		// 슬라이더 초기화 - 알파값, 최소, 최대값, 초기값
		self.slider.alpha = kSliderAlphaValueMin;
		self.slider.minimumValue = kMapSpanValueMin;
		self.slider.maximumValue = kMapSpanValueMax;
		self.slider.value = kMapSpanValueMin;
		
		// 거리 레이블의 초기화
		[self drawDistanceLabel];
		
		// 거리 레이블의 폰트 설정
		self.labelForDistance.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.labelForDistance.font.pointSize];
		
		// 델리게이트 설정.
		self.locationManager.delegate = self;
		
		// 가장 정교한 정확도로 설정.
		self.locationManager.desiredAccuracy = kCLLocationAccuracyBestForNavigation;
		self.locationManager.distanceFilter = kMaximumDistanceFilterInMap;
	}
	else
	{
		UIAlertView *alertView = [[UIAlertView alloc]
								  initWithTitle:@"에러" 
								  message:@"GPS가 불가능한 장치입니다. 아이폰인지 확인하여 주세요.\n상위뷰로 이동합니다." 
								  delegate:self
								  cancelButtonTitle:@"확인"
								  otherButtonTitles:nil
								  ];
		[alertView show];
		[alertView release];
	}

}

- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.mapView = nil;
	self.slider = nil;
	self.labelForDistance = nil;
	self.activityIndicatorView = nil;
	self.alertViewForDB = nil;
	self.locationManager = nil;
	self.reverseGeocoder = nil;
	self.myLocation = nil;
	self.arrayRestaurantAnnotations = nil;
	self.dbConnector = nil;
	self.arrayAllRestaurants = nil;
}


- (void)dealloc 
{	
	self.mapView = nil;
	self.slider = nil;
	self.labelForDistance = nil;
	self.activityIndicatorView = nil;
	self.alertViewForDB = nil;
	self.locationManager = nil;
	self.reverseGeocoder = nil;
	self.myLocation = nil;
	self.arrayRestaurantAnnotations = nil;
	self.dbConnector = nil;
	self.arrayAllRestaurants = nil;
	
    [super dealloc];
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	
	// 로케이션 매니저의 위치, 방향 찾기 시작
	[self.locationManager startUpdatingLocation];
	[self.locationManager startUpdatingHeading];
	
	// 새로 시작하는 경우에만 액티비티 인디케이터 뷰 애니메이션 실행
	if ([self.arrayRestaurantAnnotations count] ==0)
	{
		[self.activityIndicatorView startAnimating];
	}
	
	
	[super viewDidAppear:animated];
	
	// 네비게이션바의 배경화면 설정
	UIImage *navigatinoBarBgImage = [UIImage imageNamed:JBFOOD_NAVIGATION_BAR_BG_FILENAME];
	CGRect bgRect = self.navigationController.navigationBar.bounds;
	UIImageView *navigatinoBarBgImageView = [[UIImageView alloc] initWithFrame:bgRect];
	navigatinoBarBgImageView.image = navigatinoBarBgImage;
	[self.navigationController.navigationBar insertSubview:navigatinoBarBgImageView atIndex:0];
	[navigatinoBarBgImageView release];
	
	
	// 네비게이션바의 타이틀뷰 설정
	UILabel *titleLabel = [[UILabel alloc] init];
	titleLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:JBFOOD_NAVIGATION_TITLE_FONT_SIZE];
	titleLabel.text = self.title;
	titleLabel.backgroundColor = [UIColor clearColor];
	[titleLabel sizeToFit];
	self.navigationItem.titleView = titleLabel;
	[titleLabel release];
}


- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
	
	// 로케이션 매니저의 위치, 방향 찾기 종료
	[self.locationManager stopUpdatingLocation];
	[self.locationManager stopUpdatingHeading];

	// 리버스 지오코더 업데이트 종료
	[self.reverseGeocoder cancel];
}


- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	if (self.dbConnector)
	{
		[self.dbConnector removeAllDelegateReferenceToNil];
	}
}


#pragma mark -
#pragma mark 어노테이션 관련 메소드
// 어노테이션 설정
- (void)setMyCenterLocation:(CLLocationCoordinate2D)newCoord
{
	// 현재 좌표 설정
	self.myLocation.coordinate = newCoord;

	
	// 현재 위도,경도로 부터 CLLocation객체 생성
	CLLocation *centerLocation = [[CLLocation alloc] initWithLatitude:newCoord.latitude longitude:newCoord.longitude];
	
	// array를 순회하면서 거리차를 계산한다.
	for (MapLocation *location in self.arrayRestaurantAnnotations)
	{
		CLLocation *thisLocation = [[CLLocation alloc] initWithLatitude:location.coordinate.latitude longitude:location.coordinate.longitude];
		
		location.radialDistance = [centerLocation distanceFromLocation:thisLocation];
		if (location.radialDistance <= 0)
		{
			location.address = @"거리계산중..";
		}
		else
		{
			location.address = [self getStringForDistance:location.radialDistance];
		}

		[thisLocation release];
	}
	
	[centerLocation release];
}

// 맛집데이터로부터 MapLocation 추가하기
- (void)addMapLocationWithRestaurant:(Restaurant*)newRestaurant
{
	// 현재 유저의 위치어노테이션 생성
	MapLocation *location = [[MapLocation alloc] init];
	
	// 정보 입력
	location.name = newRestaurant.name;
	location.coordinate = CLLocationCoordinate2DMake(newRestaurant.latitude, newRestaurant.longitude);

	// 현재 위치와 맛집어노테이션의 위치
	CLLocation *centerLocation = [[CLLocation alloc] initWithLatitude:self.myLocation.coordinate.latitude longitude:self.myLocation.coordinate.longitude];
	CLLocation *thisLocation = [[CLLocation alloc] initWithLatitude:location.coordinate.latitude longitude:location.coordinate.longitude];

	// 현재 위치와 거리차 계산
	location.radialDistance = [centerLocation distanceFromLocation:thisLocation];
	
	// 거리차를 서브타이틀 문자열로 설정.
	location.address =  [self getStringForDistance:location.radialDistance];
	
	// 맛집 데이터베이스검색에 사용되는 id값 설정.
	location.id = newRestaurant.id;
	
	[centerLocation release];
	[thisLocation release];
		
	
	// 맛집데이터를 가진 어노테이션 array에 추가
	[self.arrayRestaurantAnnotations addObject:location];
	
	// 릴리즈
	[location release];
}


// 모든 어노테이션뷰 삭제
- (void)removeAllMapLocationAndAnnotaionView
{
	// 어노테이션 뷰 삭제
	[self.mapView removeAnnotations:self.arrayRestaurantAnnotations];
	
	// Location array 삭제
	[self.arrayRestaurantAnnotations removeAllObjects];
}

// 어노테이션뷰의 버튼이벤트로부터 맛집뷰 보여주기
- (void)showRestaurantView:(id)sender
{
	// - (MKAnnotationView *) mapView:(MKMapView *)theMapView viewForAnnotation:(id <MKAnnotation>) annotation 
	// 위의 delegate구현 함수로부터 만들어진 어노테이션뷰
	// 
	//	else if ([annotation isKindOfClass:[MapLocation class]]) 
	//	{
	//	    static NSString *mapLocationIdentifier = @"MapLocationIdentifier";
	//		
	//		// plcaemarkIdentifier값을 통해서 어노테이션 뷰를 가져온다.
	//		MKAnnotationView *annotationView = (MKAnnotationView *)[theMapView dequeueReusableAnnotationViewWithIdentifier:mapLocationIdentifier];
	//		
	//		// 어노테이션뷰가 없다면 새로 생성
	//		if (annotationView == nil)  
	//		{
	//			annotationView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:mapLocationIdentifier] autorelease];
	//            annotationView.image = [UIImage imageNamed:@"point.png"];
	//			annotationView.canShowCallout = YES;
	//			
	//			UIButton* rightButton = [UIButton buttonWithType:UIButtonTypeDetailDisclosure];
	//-------->	rightButton.tag = (NSInteger)annotation;																		 <----------- (1)
	//-------->	[rightButton addTarget:self action:@selector(showRestaurantView:) forControlEvents:UIControlEventTouchUpInside]; <----------- (2)
	//			
	//			annotationView.rightCalloutAccessoryView = rightButton;
	//			
	//			
	//            return annotationView;
	//        }            
	//        else 
	//            annotationView.annotation = annotation;
	//        
	//        return annotationView;
	//    }
	// (1) 어노테이션의 id를 버튼의 tag에 저장한다.
	// (2) 이부분에서 만들어진 버튼이 눌러지면 지금의 함수를 호출한다.
	
	// 함수호출한 sender가 UIButton인지 확인
	if ([sender isKindOfClass:[UIButton class]])
	{
		// UIButton의 tag에 있는 값이 어노테이션의 id값이다
		id <MKAnnotation> annotation =  (id <MKAnnotation>) ((UIButton*)sender).tag;
		
		// MapLocation클래스인지 확인
		if ([annotation isKindOfClass:[MapLocation class]])
		{
			// 액티비티 인디케이터 뷰 애니메이션 시작
			[self.activityIndicatorView startAnimating];
			
			// MapLocation으로 형변환하여 id값을 알아낸다.
			MapLocation *location = (MapLocation*)annotation;
			
			// id값으로 맛집 데이터 찾기
			DatabaseConnector *connector = [[DatabaseConnector alloc] init];
			self.dbConnector = connector;
			[connector release];
			self.dbConnector.restaurantFromIdDelegate = self;
			[self.dbConnector startReceivingRestaurantFromId:location.id];
		}
	}
}


#pragma mark -
#pragma mark 맵 델리게이트 메소드
- (MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation
{
	// MyLocatoin 클래스라면 내위치를 나타내는 어노테이션이다.
	if ([annotation isKindOfClass:[MyLocation class]])
	{
		static NSString *mapLocationIdentifier = @"MyLocationIdentifier";
		
		// plcaemarkIdentifier값을 통해서 어노테이션 뷰를 가져온다.
		MKAnnotationView *annotationView = (MKAnnotationView *)[self.mapView dequeueReusableAnnotationViewWithIdentifier:mapLocationIdentifier];
		
		// 어노테이션뷰가 없다면 새로 생성
		if (annotationView == nil)  
		{
			annotationView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:mapLocationIdentifier] autorelease];
            annotationView.image = [UIImage imageNamed:@"arrow.png"];
			annotationView.canShowCallout = YES;
			
            return annotationView;
        }            
        else
		{
            annotationView.annotation = annotation;
		}
        
        return annotationView;
	}

	// MapLocatoin 클래스라면 맛집데이터를 나타내는 어노테이션이다.
	else if ([annotation isKindOfClass:[MapLocation class]])
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
			
			UIButton* rightButton = [UIButton buttonWithType:UIButtonTypeDetailDisclosure];
			rightButton.tag = (NSInteger)annotation;
			[rightButton addTarget:self action:@selector(showRestaurantView:) forControlEvents:UIControlEventTouchUpInside];
			annotationView.rightCalloutAccessoryView = rightButton;
			
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


// 맛집데이터 array로부터 MapLocation 추가하기
- (void)addMapLocationWithArrayRestaurants:(NSMutableArray*)newArrayRestaurants
{
	for (Restaurant *restaurant in newArrayRestaurants)
	{
		[self addMapLocationWithRestaurant:restaurant];
	}
}


// 어노테이션뷰 업데이트
- (void)updatingMapLocation
{
	for (MapLocation *location in self.arrayRestaurantAnnotations)
	{
		// 슬라이더값/2(화면반경)보다 거리차가 크다면 어노테이션뷰에서 제거하고 아니면 어노테이션뷰에 추가한다.
		if (location.radialDistance > self.slider.value / 2)
		{
			[self.mapView removeAnnotation:location];
		}
		else 
		{
			[self.mapView addAnnotation:location];
		}
	}
}


// 거리 텍스트 얻기
- (NSString*)getStringForDistance:(double)distance
{
	NSString *distanceText;
	
	if (distance < 1000)
	{
		distanceText = [NSString stringWithFormat:@"%dm", (int)distance];
	}
	else
	{
		distanceText = [NSString stringWithFormat:@"%0.2fKm", distance/1000];
	}

	return distanceText;
}

#pragma mark -
#pragma mark 로케이션 매니저 관련 메소드
// 로케이션 매니저 업데이트 시작하기
- (void)startLocationManagerUpdating
{
	// 델리케이트 지정.
	self.locationManager.delegate = self;
	
	// 위치찾기 업데이트 시작
	[self.locationManager startUpdatingLocation];
	
	// 방향찾기 업데이트 시작
	[self.locationManager startUpdatingHeading];
}


// 로케이션 매니저 업데이트 종료하기
- (void)stopLocationManagerUpdating
{
	// 델리게이트 미지정.
	self.locationManager.delegate = nil;
	
	// 위치찾기 업데이트 시작
	[self.locationManager stopUpdatingLocation];
	
	// 방향찾기 업데이트 시작
	[self.locationManager stopUpdatingHeading];	
}


#pragma mark -
#pragma mark 로케이션 매니저 델리게이트 메소드
- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation
{
    MKCoordinateRegion viewRegion = MKCoordinateRegionMakeWithDistance(newLocation.coordinate, self.slider.value, self.slider.value); 
    MKCoordinateRegion adjustedRegion = [mapView regionThatFits:viewRegion];
    [mapView setRegion:adjustedRegion animated:YES];
	
	if (myLocation == nil)
	{
		// 현재 유저의 위치어노테이션 생성
		MyLocation *myLocatoinForMultiMap = [[MyLocation alloc] init];
		self.myLocation = myLocatoinForMultiMap;
		[myLocatoinForMultiMap release];
		
		// 정보 입력
		myLocation.address = @"주소 추적중";
		myLocation.coordinate = newLocation.coordinate;
		
		// 거리 측정
		[self setMyCenterLocation:newLocation.coordinate];
		
		// 맵뷰에 어노테이션추가
		[mapView addAnnotation:myLocation];
	}
	else
	{
		[self setMyCenterLocation:newLocation.coordinate];
	}
	
	// ----------------------------------------------------------
	//	지도를 한개 보는것이 아니라면 주변검색을 하는것이다.
	//	따라서 리버스 지오코더를 이용하여 주소를 찾고,
	//	주소의 locality값을 이용하여 데이터베이스에서 맛집데이터를 가져온다.
	//	리버스 지오코더를 생성한다. - 한번만 생성한다!!!
	// ----------------------------------------------------------
	
	if (!self.startFlag)
	{
		MKReverseGeocoder *reverseGeocoderForMultiMap = [[MKReverseGeocoder alloc] initWithCoordinate:newLocation.coordinate];
		self.reverseGeocoder = reverseGeocoderForMultiMap;
		[reverseGeocoderForMultiMap release];
		reverseGeocoder.delegate = self;
		[reverseGeocoder start];

		// 시작플래그 활성화
		self.startFlag = YES;
	}
	
	
	// 주소정보를 얻지 못하였을 경우의 업데이트
	if (self.isNotFoundAddressFromReversegeocoder)
	{
		CLLocation *centerLocation = [[CLLocation alloc] initWithLatitude:self.myLocation.coordinate.latitude longitude:self.myLocation.coordinate.longitude];
		CLLocation *firstcenterLocation = [[CLLocation alloc] initWithLatitude:firstCenterCoord.latitude longitude:firstCenterCoord.longitude];
		
		// 처음 좌표와 1km이상 차이가 난다면 모든맛집 데이터로부터 다시 ARRestaurantView를 재 생성한다.
		if ([centerLocation distanceFromLocation:firstcenterLocation] > kMaximumDistanceFilterForAllRestaurants)
		{
			// firstCenterCood값 설정.
			firstCenterCoord = myLocation.coordinate;
			
			[self generateAnnotationViewFromArrayAllRestaurants];
		}
		
		[centerLocation release];
		[firstcenterLocation release];
	}	
}


- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{	
	[mapView viewForAnnotation:self.myLocation].transform = CGAffineTransformMakeRotation (M_PI * (newHeading.trueHeading) / 180.0f);
	
//	// 나침반(방향)만큼 맵뷰를 회전시킨다.
//	self.mapView.transform = CGAffineTransformMakeRotation (M_PI * (360 - newHeading.trueHeading) / 180.0f);
//	
//	
//	// 맵뷰의 서브뷰들. 즉 어노테이션뷰는 아이폰의 머리방향으로 향해 있어야 하므로, 다시 회전시킨다.
//	for (id<MKAnnotation> annotation in self.mapView.annotations)
//	{		
//		[mapView viewForAnnotation:annotation].transform = CGAffineTransformMakeRotation (M_PI * (newHeading.trueHeading) / 180.0f);
//	}
}


#pragma mark -
#pragma mark 리버스 지오코더 관련 메소드
// 지오코더 제거
- (void)removeReverseGeocoder
{
	reverseGeocoder.delegate = nil;
	[reverseGeocoder cancel];
	self.reverseGeocoder = nil;	
}


//지오코더로 부터 주소얻기 메소드
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
#pragma mark 리버스 지오코더 델리게이트 메소드
- (void)reverseGeocoder:(MKReverseGeocoder *)geocoder didFindPlacemark:(MKPlacemark *)placemark
{	
	// 지오코더 제거
	[self removeReverseGeocoder];
		
	// 현재위치의 주소 설정
	myLocation.address = [self getAddressFromPlaceMark:placemark];

	// 맛집데이터를 가진 어노테이션 array가 0이면, 즉 데이터가 없다면 지역명(placemark.locality)으로 검색하여 맛집 데이터 가져온다.
	if ([arrayRestaurantAnnotations count] == 0)
	{
		// 지오코더로부터 얻은 주소로부터 데이터베이스 검색
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		self.dbConnector = connector;
		[connector release];
		self.dbConnector.arrayRestaurantsFromAreaDelegate = self;
		[self.dbConnector startReceivingArrayRestaurantsFromArea:placemark.locality];		
	}
}


- (void)reverseGeocoder:(MKReverseGeocoder *)geocoder didFailWithError:(NSError *)error
{	
	// 지오코더 삭제
	[self removeReverseGeocoder];
	
	// 모든데이터 가져오기 시작
	[self startAllRestaurnatDataForNotFoundAddress];
}


#pragma mark -
#pragma mark 알람뷰 메소드
- (void)makeAlertView:(NSString*)title
{
	if (self.alertViewForDB)
	{
		self.alertViewForDB = nil;
	}
	
	UIAlertView *alertView = [[UIAlertView alloc]
							  initWithTitle:@"알림" 
							  message:title
							  delegate:self
							  cancelButtonTitle:@"확인"
							  otherButtonTitles:nil
							  ];
	self.alertViewForDB = alertView;
	[alertView show];
	[alertView release];
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{	

}


#pragma mark -
#pragma mark 슬라이더 이벤트
- (IBAction)changeSliderValue:(id)sender
{	
	// 슬라이더 값이 변한하였으므로 거리 레이블을 설정한다.
	[self drawDistanceLabel];
	
	// 슬라이더 값이 변한하였으므로 지도를 줌 인/아웃 시킨다.
	MKCoordinateRegion viewRegion = MKCoordinateRegionMakeWithDistance(mapView.region.center, self.slider.value, self.slider.value); 
	MKCoordinateRegion adjustedRegion = [mapView regionThatFits:viewRegion];
    [mapView setRegion:adjustedRegion animated:YES];
	
	// 슬라이더 값이 변하였으므로 맛집데이터를 가진 어노테이션을 업데이트 시킨다.
	[self updatingMapLocation];
}


- (IBAction)touchDownSlider:(id)sender
{
	self.slider.alpha = kSliderAlphaValueMax;
}


- (IBAction)touchUpSlider:(id)sender
{
	self.slider.alpha = kSliderAlphaValueMin;
}


- (void)drawDistanceLabel
{
	NSString *distanceText;
	
	double distance = self.slider.value/2;
	if (distance < 1000)
	{
		distanceText = [NSString stringWithFormat:@"반경 %dm", (int)distance];
	}
	else
	{
		distanceText = [NSString stringWithFormat:@"반경 %0.2fKm", distance/1000];
	}

	self.labelForDistance.text = distanceText;
}


#pragma mark -
#pragma mark 종료 메소드
- (void)MultiMapviewControllerDidFinish
{
	[self removeReverseGeocoder];
	[self stopLocationManagerUpdating];
	[self.delegate MultiMapViewControllerDidFinish:self];
}


#pragma mark -
#pragma mark 데이터베이스 연결 델리게이트 - arrayRestaurantsFromAreaDelegate
- (void)arrayDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{

}


- (void)arrayDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{	
	// 데이터베이스 커넥터 삭제
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;

	// 액티비티 인디케이터 애니메이션 종료
	[activityIndicatorView stopAnimating];
	
	// 알람 뷰 생성
	[self makeAlertView:@"연결에 실패하였습니다."];
}


- (void)arrayDidFinish:(DatabaseConnector *)connector withArrayRestaurants:(NSMutableArray *)array
{
	// 데이터베이스 커넥터 삭제
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인디케이터 애니메이션 종료
	[activityIndicatorView stopAnimating];
			
	// 검색결과가 존재한다면
	if ([array count] > 0)
	{
		[self addMapLocationWithArrayRestaurants:array];
	}
	else
	{
		// 결과가 존재하지 않는다면 모든 맛집 데이터를 받아서 사용
		[self startAllRestaurnatDataForNotFoundAddress];
	}
}


#pragma mark -
#pragma mark 데이터베이스 연결 델리게이트 - restaurantFromIdDelegate
- (void)restaurantDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{

}


- (void)restaurantDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{
	// 데이터베이스 커넥터 삭제
	connector.restaurantFromIdDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인디케이터 애니메이션 종료
	[activityIndicatorView stopAnimating];
	
	// 알람 뷰 생성
	[self makeAlertView:@"연결에 실패하였습니다."];
}


- (void)restaurantDidFinish:(DatabaseConnector *)connector withRestaurant:(Restaurant *)newRestaurant
{
	// 데이터베이스 커넥터 삭제
	connector.restaurantFromIdDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인디케이터 애니메이션 종료
	[activityIndicatorView stopAnimating];

	
	// 검색결과가 존재한다면
	if (newRestaurant)
	{
		// 맛집뷰 생성
		RestaurantViewController *viewController = [[RestaurantViewController alloc] initWithRestaurant:newRestaurant];
		
		// 생성한 맛집뷰를 push
		[self.navigationController pushViewController:viewController animated:YES];
		
		// 생성한 테이블뷰 릴리즈
		[viewController release];
	}
	// 검색결과가 존재하지 않는다면
	else
	{
		// 알람 뷰 생성
		[self makeAlertView:@"맛집데이터가 존재하지 않습니다."];
	}
}


#pragma mark -
#pragma mark 주소정보 받지 못할 경우 필요한 메소드
// 주소정보를 받지 못해 모든 데이터로 시작해야하는 경우 발생하는 메소드
- (void)startAllRestaurnatDataForNotFoundAddress
{	
	// 플래그 설정
	self.isNotFoundAddressFromReversegeocoder = YES;
	
	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = connector;
	[connector release];
	self.dbConnector.allRestaurantsDelegate = self;
	[self.dbConnector startReceivingAllRestaurants];
}

// 주소정보를 받지 못한경우 arrayAllRestaurants로부터 ARRestaurantView 재 생성
- (void)generateAnnotationViewFromArrayAllRestaurants
{
	// 증강현실뷰가 있다면 삭제한다.
	[self removeAllMapLocationAndAnnotaionView];
	
	// 모든 맛집데이터로부터 적절한 거리에 있는 맛집데이터 array를 얻어온다.
	NSMutableArray *arrayForRestaurant = [self getRestaurantsFromArryAllRestaurant];
	
	// 증강현실뷰를 만든다.
	[self addMapLocationWithArrayRestaurants:arrayForRestaurant];

	// 증강현실뷰 거리계산 업데이트 실시
	[self setMyCenterLocation:myLocation.coordinate];
}


// arrayAllRestaurants로부터 거리차 구하여 restaurant 얻기
- (NSMutableArray*)getRestaurantsFromArryAllRestaurant
{
	NSMutableArray *array = [NSMutableArray array];
	
	CLLocation *centerLocation = [[CLLocation alloc] initWithLatitude:myLocation.coordinate.latitude longitude:myLocation.coordinate.longitude];
	for (Restaurant *newRestaurant in self.arrayAllRestaurants)
	{
		CLLocation *restaurantLocation = [[CLLocation alloc] initWithLatitude:newRestaurant.latitude longitude:newRestaurant.longitude];
		
		// 현재 좌표와 맛집의 좌표 차이가 kMaximumDistanceRangeForAllRestaurants 이하라면 추가한다.
		if ([centerLocation distanceFromLocation:restaurantLocation] <= kMaximumDistanceRangeForAllRestaurants)
		{
			[array addObject:newRestaurant];
		}
		[restaurantLocation release];
	}
	[centerLocation release];
	
	return array;
}


#pragma mark -
#pragma mark 주소정보 받지 못할 경우 필요한 메소드 - allRestaurantsDelegate
- (void)allRestaurantsDidFail:(DatabaseConnector*)connector withError:(NSError*)data
{
	// 데이터베이스 커넥터 삭제
	connector.allRestaurantsDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인디케이터 뷰 애니메이션 종료
	[self.activityIndicatorView stopAnimating];
	
	// 알람뷰 생성
	[self makeAlertView:@"연결에 실패하였습니다."];
}

- (void)allRestaurantsDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
{
	
}

- (void)allRestaurantsDidFinish:(DatabaseConnector*)connector withArray:(NSMutableArray*)array
{
	// 데이터베이스 커넥터 삭제
	connector.allRestaurantsDelegate = nil;
	self.dbConnector = nil;
	
	// 모든 맛집 데이터 저장
	self.arrayAllRestaurants = array;
	
	// 맛집 어노테이션 뷰 생성
	[self generateAnnotationViewFromArrayAllRestaurants];
	
	
	// 인디케이터 뷰 애니메이션 멈춤
	[self.activityIndicatorView stopAnimating];
}

@end
