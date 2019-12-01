#import "ARViewController.h"
#import "Restaurant.h"
//#import "RestaurantViewController.h"
#import "RestaurantPreViewController.h"
#import <QuartzCore/QuartzCore.h>

@implementation ARViewController


#pragma mark -
#pragma mark synthesize
@synthesize cameraController, cameraOverlayView, button_back, arRootView, activityIndicatorView, arrayARRestaurantViews, delegate, wantStop, alertViewForDB;
@synthesize distanceSlider, distanceLabel;
@synthesize maximumScaleDistance, minimumScaleFactor, maximumRotationAngle, rotateViewsBasedOnPerspective, centerCoord,centerInclination, centerAzimuth, maximumDistanceRange, isSingleAR;
@synthesize locationManager, reverseGeocoder, accelerometerManager;
@synthesize dbConnector, arrayAllRestaurants, firstCenterCoord, isNotFoundAddressFromReversegeocoder;
@synthesize startFlag;

#ifdef DEBUG_MODE
@synthesize debugLabel;
#endif

// 초기화 메소드
// 카메라와 코어로케이션을 생성한다.
- (void)initAllInstance
{
	// ------------------------
	//	카메라와 코어로케이션 생성.
	// ------------------------
	
	// 카메라를 지원하는지 확인
	if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
	{
		// 이미지뷰(카메라)와 오버레이뷰 만들기
		[self makeImagePickerAndOverlayView];
		
		// 오버레이뷰가 만들어졌으므로 오버레이뷰의 하위에 액티비티 인디케이터 뷰 생성
		UIActivityIndicatorView *indicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		self.activityIndicatorView = indicatorView;
		[indicatorView release];
		self.activityIndicatorView.center = self.arRootView.center;
		//[self.activityIndicatorView startAnimating];		
		self.activityIndicatorView.hidesWhenStopped = YES;
		[self.cameraOverlayView addSubview:self.activityIndicatorView];
		
		// 종료플래그 초기화
		self.wantStop = NO;
	}
	else 
	{
		self.wantStop = YES;
		return;
	}

	// 코어로케이션(위치찾기와 나침반기능)를 지원하는지 확인
	if([CLLocationManager locationServicesEnabled] && [CLLocationManager headingAvailable])
	{
		// 로케이션 매니저와 가속센서 매니저 만들기
		[self makeLocationAndAccelManager];
	}
	else 
	{
		return;
	}
	
	// -----------------------------------------------------------------------
	//	카메라와 코어로케이션이 성공적으로 만들어졌다면 증강현실뷰를 저장할 array를 생성한다.
	// -----------------------------------------------------------------------
	NSMutableArray *arrayForARRestaurant = [[NSMutableArray alloc] init];
	self.arrayARRestaurantViews = arrayForARRestaurant;
	[arrayForARRestaurant release];
	
	
	// ----------------------------
	//	나머지 증강현실 관련 설정 초기화
	// ----------------------------
	self.minimumScaleFactor = kMinimumScaleFactor;	
	self.rotateViewsBasedOnPerspective = kRotateViewsBasedOnPerspective;
	self.maximumRotationAngle = kMaximumRotationAngle;
	self.maximumDistanceRange = kMaximumDistanceRange;

	// 현재 좌표 초기화
	centerCoord.latitude = 0;
	centerCoord.longitude = 0;
	firstCenterCoord.latitude = 0;
	firstCenterCoord.longitude = 0;
	
	// 시작 플래그
	startFlag = NO;
	self.isNotFoundAddressFromReversegeocoder = NO;
}


// 초기화 메소드
- (id)init
{
	if (self = [super init])
	{

	}
	
	return self;
}

// 생성 - 맛집증강현실뷰를 한개로만 이용할 것지 결정
- (id)initWithUsingSingleAR:(BOOL)single;
{
	if (self = [self init])
	{
		self.isSingleAR = single;
		[self initAllInstance];
	}
	
	return self;
}

// 초기화 메소드
- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super initWithCoder:aDecoder])
	{
		[self initAllInstance];
	}
	
	return self;
}


// 인터페이스빌더를 사용하여 뷰를 생성하기 위한 메소드
// 아웃렛으로 연결한것이 없기 때문에 사용하지 않는다.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withUsingSingleAR:(BOOL)single
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		self.isSingleAR = single;
		[self initAllInstance];
	}
	
	return self;
}


// 인터페이스빌더를 사용하지않고 프로그래밍으로 뷰를 생성하기 위한 메소드
- (void)loadView 
{
	UIView *mainView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, ARVIEW_WIDTH, ARVIEW_HEIGHT)];
	self.view = mainView;
	[mainView release];
	self.view.userInteractionEnabled = YES;
}


- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	// TO DO
}


- (void)didReceiveMemoryWarning 
{
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}


- (void)viewDidUnload 
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];

	// ------------------------------------------------------------
	//	화면에 뷰가 나타났으므로 이미지피커뷰를 모달뷰로 생성하고 업데이트 실시
	// ------------------------------------------------------------
	
	// 이미지피커(카메라)와 로케이션 매니저가 성공적으로 만들어 졌다면 로케이션 업데이트를 실행한다.
	if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
	{
		if (!wantStop)
		{
			// 이미지피커(카메라)뷰컨트롤러를 모달뷰로 붙임.
			[self presentModalViewController:self.cameraController animated:NO];
			[self.cameraController release];
			
			// 업데이트 실시
			[self startLocationManagerUpdating];			
		}
	}
	
	// 만들기에 실패하였으므로 경고창을 띄운다.
	else
	{
		// 카메라와 로케이션 둘다 만들지 못하였다면
		if (!self.cameraController && !self.locationManager)
		{
			// 알람창 보여주기
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle:@"에러" 
								  message:@"카메라와 GPS를 작동할수 없습니다. 실행할수 있는 장치인지 확인하여 주세요.\n상위뷰로 이동합니다." 
								  delegate:self
								  cancelButtonTitle:@"확인"
								  otherButtonTitles:nil
								  ];
			[alert show];
			[alert release];			
		}
		else
		{
			if (!self.cameraController)
			{
				UIAlertView *alert = [[UIAlertView alloc]
									  initWithTitle:@"에러" 
									  message:@"카메라를 작동할수 없습니다. 카메라를 실행할수 있는 장치인지 확인하여 주세요.\n상위뷰로 이동합니다." 
									  delegate:self
									  cancelButtonTitle:@"확인"
									  otherButtonTitles:nil
									  ];
				[alert show];
				[alert release];				
			}
			else
			{
				UIAlertView *alert = [[UIAlertView alloc]
									  initWithTitle:@"에러" 
									  message:@"GPS가 불가능한 장치입니다. 아이폰인지 확인하여 주세요.\n상위뷰로 이동합니다." 
									  delegate:self
									  cancelButtonTitle:@"확인"
									  otherButtonTitles:nil
									  ];
				[alert show];
				[alert release];				
			}
		}
	}
}


- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	if (self.dbConnector)
	{
		[self.dbConnector removeAllDelegateReferenceToNil];
	}
}


- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
	
	// 로케이션 매니저 업데이트 종료하기
	if (self.locationManager)
	{
		[self stopLocationManagerUpdating];
	}
	
	// 지오코더 종료하기
	if (self.reverseGeocoder)
	{
		[self removeReverseGeocoder];
	}
}


- (void)dealloc 
{
	self.cameraOverlayView = nil;
	self.button_back = nil;
	self.distanceSlider = nil;
	self.distanceLabel = nil;
	self.arRootView = nil;
	self.activityIndicatorView = nil;
	self.alertViewForDB = nil;
	self.arrayARRestaurantViews = nil;
	self.locationManager = nil;
	self.dbConnector = nil;
	self.arrayAllRestaurants = nil;
	
#ifdef DEBUG_MODE
	[self.debugLabel release];
#endif

    [super dealloc];
}


#pragma mark -
#pragma mark 각종 매니저와 뷰 관련 메소드
// 이미지피커와 오버레이뷰 만들기
- (void)makeImagePickerAndOverlayView
{
	// 이미지 피커 생성
	UIImagePickerController *pickerController = [[UIImagePickerController alloc] init];
	self.cameraController = pickerController;
	[pickerController release];
	self.cameraController.allowsEditing = NO;
	self.cameraController.sourceType = UIImagePickerControllerSourceTypeCamera;
	self.cameraController.showsCameraControls = NO;
	self.cameraController.wantsFullScreenLayout = YES;
	self.cameraController.navigationBarHidden = YES;
	
	// 뷰 생성하여 이미지피커뷰의 오버레이뷰로 설정. - 사이즈는 현재 가장 상위의 뷰와 동일
	CGRect rect = self.view.bounds;
	UIView *overlayView = [[UIView alloc] initWithFrame:rect];
	self.cameraOverlayView = overlayView;
	[overlayView release];
	self.cameraController.cameraOverlayView = self.cameraOverlayView;
	
	// 오버레이뷰를 회전 시킨다음에 사이즈 재 조정.
	CGAffineTransform transform = CGAffineTransformIdentity;
	self.cameraOverlayView.transform = CGAffineTransformRotate(transform, M_PI/2);
	self.cameraOverlayView.bounds = CGRectMake(0, 0, rect.size.height, rect.size.width);
		
	// 실제로 화면에 보일 증강현실 뷰 생성하여 오버레이뷰에 서브뷰로 추가
	UIView *viewForARRoot = [[UIView alloc] initWithFrame:CGRectMake( (ARVIEW_HEIGHT - ARROOTVIEW_HEIGHT)/2, (ARVIEW_WIDTH - ARROOTVIEW_WIDTH)/2, ARROOTVIEW_HEIGHT, ARROOTVIEW_WIDTH)];
	self.arRootView = viewForARRoot;
	[viewForARRoot release];
	[self.cameraOverlayView addSubview:self.arRootView];
	
	// 뒤로가기 버튼 생성하여 오버레이뷰에 서브뷰로 추가
	self.button_back = [UIButton buttonWithType:UIButtonTypeRoundedRect];
	[self.button_back setTitle:@"뒤로가기" forState:UIControlStateNormal];
	[self.button_back setFrame:CGRectMake(10, 260, 100, 50)];
	[self.button_back addTarget:self action:@selector(touchButton_back:) forControlEvents:UIControlEventTouchUpInside];
	self.button_back.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kBackButtonFontSize];
	[self.cameraOverlayView addSubview:self.button_back];

	
	// 맛집 1개만 증강현실로 보여줄때는 거리조절이 필요하지 않다.
	if (!self.isSingleAR) 
	{
		// 거리를 나타내는 슬라이더 생성하여 서브뷰로 추가
		UISlider *sliderForDistance = [[UISlider alloc] initWithFrame:CGRectMake(SLIDER_POINT_X, SLIDER_POINT_Y, SLIDER_WIDTH, SLIDER_HEIGHT)];
		self.distanceSlider = sliderForDistance;
		[sliderForDistance release];
		self.distanceSlider.maximumValue = kMaximumDistanceRange;	// 증강현실을 보여줄 최대거리
		self.distanceSlider.minimumValue = kMinimumDistanceRange;	// 증강현실을 보여줄 최소거리
		self.distanceSlider.value = kMinimumDistanceRange + kMaximumDistanceRange / 2;			// 최소거리로 값을 초기화
		self.distanceSlider.alpha = kARSliderAlphaValueMin;			// 알파값
		[self.distanceSlider addTarget:self action:@selector(changeSliderValue:) forControlEvents:UIControlEventValueChanged];	// 이벤트 추가
		[self.distanceSlider addTarget:self action:@selector(touchUpSlider:) forControlEvents:UIControlEventTouchUpInside];
		[self.distanceSlider addTarget:self action:@selector(touchDownSlider:) forControlEvents:UIControlEventTouchDown];
		[self.cameraOverlayView addSubview:self.distanceSlider];	// 서브뷰로 추가
		
		//	최대거리를 설정하는 슬라이더값을 넣었으므로 최대거리 값 설정.
		self.maximumScaleDistance = self.distanceSlider.value;
		
		// 거리를 나타내는 레이블 생성하여 서브뷰로 추가
		UILabel *labelForDistance = [[UILabel alloc] initWithFrame:CGRectMake(LABEL_POINT_X, LABEL_POINT_Y, LABEL_WIDTH, LABEL_HEIGHT)];
		self.distanceLabel = labelForDistance;
		[labelForDistance release];
		self.distanceLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kLABEL_FONT_SIZE];	// 폰트 설정
		self.distanceLabel.textColor = [UIColor whiteColor];
		self.distanceLabel.textAlignment = UITextAlignmentCenter;
		self.distanceLabel.backgroundColor = [UIColor clearColor];								// 배경 투명
		self.distanceLabel.alpha = kARSliderAlphaValueMin;
		[self drawDistanceLabel];
		[self.cameraOverlayView addSubview:self.distanceLabel];		// 서브뷰로 추가
	}
	
#ifdef DEBUG_MODE
	// 디버깅화면 뷰 추가
	self.debugLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, ARVIEW_HEIGHT, ARVIEW_WIDTH/8)];
	self.debugLabel.text = @"기다려주세요.";
	self.debugLabel.textAlignment = UITextAlignmentCenter;
	[self.cameraOverlayView addSubview:self.debugLabel];
#endif
}


// 로케이션 매니저와 가속센서 매니저 만들기
- (void)makeLocationAndAccelManager
{
	// 로케이션 매니저 생성
	CLLocationManager *locationManagerForAR = [[CLLocationManager alloc] init];
	self.locationManager = locationManagerForAR;
	[locationManagerForAR release];
	
	// 로케이션 설정.
	self.locationManager.headingFilter = kCLHeadingFilterNone;
	self.locationManager.desiredAccuracy = kCLLocationAccuracyBest;
	self.locationManager.delegate = self;
	self.locationManager.distanceFilter = kMaximumDistanceFilterInAR; // 일정한 거리 이상을 이동하였을 경우에만 로케이션 업데이트를 실시하게 한다.
	
	// 가속센서 생성
	UIAccelerometer *accelManagerForAR = [UIAccelerometer sharedAccelerometer];
	self.accelerometerManager = accelManagerForAR;
	[accelManagerForAR release];
	self.accelerometerManager.updateInterval = 0.01;
	self.accelerometerManager.delegate = self;
}


// 로케이션 매니저 업데이트 시작하기
- (void)startLocationManagerUpdating
{
	// 델리케이트 지정.
	self.locationManager.delegate = self;

	// 위치찾기 업데이트 시작
	[self.locationManager startUpdatingLocation];

	// 방향찾기 업데이트 시작
	[self.locationManager startUpdatingHeading];
	
	// 가속센서 델리게이트 설정
	self.accelerometerManager.delegate = self;
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
	
	// 가속센서 델리게이트 설정
	self.accelerometerManager.delegate = nil;
}


#pragma mark -
#pragma mark 증강현실뷰 추가 삭제
// 증강현실뷰 추가
- (void)addARRestaurantViewWithRestaurant:(Restaurant *)newRestaurant
{	
	// 맛집 인스터스를 가지고 맛집증강현실뷰 컨트롤러를 생성.
#ifdef USE_INTERFACE_BUILDER
	ARRestaurantViewController *controller = [[ARRestaurantViewController alloc] initWithNibName:@"ARRestaurantViewController" bundle:nil withRestaurant:newRestaurant];
#else
	ARRestaurantViewController *controller = [[ARRestaurantViewController alloc] initWithRestaurant:newRestaurant];
#endif
	
	// 맛집 증강현실뷰 터치 델리게이트 설정.
	controller.touchDelegate = self;
	
	// array에 컨트롤러를 추가.
	[self.arrayARRestaurantViews addObject:controller];
	
	// 릴리즈 하기
	[controller release];
}

// 증강현실뷰 추가 - 맛집 array로 부터 추가
- (void)addARRestaurantViewWithRestaurantArrays:(NSMutableArray *)restaurantsArray
{
	// array를 순회하며 맛집데이터를 가지고 맛집 증강현실뷰를 생성
	for (Restaurant *newRestaurant in restaurantsArray) 
	{
		[self addARRestaurantViewWithRestaurant:newRestaurant];
	}
}


// 증강현실뷰 모두 삭제
- (void)removeAllARRestaurantViews
{
	// 순회하며 증강현실루트뷰로부터 해제한다.
	for (ARRestaurantViewController *controller in arrayARRestaurantViews) 
	{
		[controller.view removeFromSuperview];
	}
	
	// 모든 맛집 증강현실뷰 삭제.
	[arrayARRestaurantViews removeAllObjects];
}


#pragma mark -
#pragma mark 증강현실관련 업데이트

//--------------------------------------------------------------------------------
// centerLocation값은 현재 아이폰의 위치를 나타내므로, 
// 위치가 바뀌면 array에 저장된 나머지 위치정보들을 모두 업데이트 시켜야 한다.
//--------------------------------------------------------------------------------
- (void)setMyCenterLocation:(CLLocationCoordinate2D)newCoord;
{
	// 현재 위도, 경도 설정
	centerCoord.latitude = newCoord.latitude;
	centerCoord.longitude = newCoord.longitude;
	
	// array에 저장된 ARRestaurantView 인스턴스들의 거리계산을 한다.
	for (ARRestaurantViewController *controller in self.arrayARRestaurantViews)
	{
		// 아이폰의 위치와 맛집간의 거리를 계산한다.
		[controller calibrateFromCenterCoordinate:centerCoord];
	}
	
	// 정렬하기
	[self sortingARRestaurantViewByRadialDistance];
	
	
	// --------------------------------------------------
	// 만약 증강현실뷰가 1개밖에 없다면 무조건 최대 거리로 정한다.
	// 이 코드가 존재하는 이유는 주변검색이 아닌 
	// 맛집뷰에서 증강현실찾기를 하였을경우가 존재하기 때문이다.
	// --------------------------------------------------
	if (isSingleAR)
	{
		self.maximumScaleDistance = [((ARRestaurantViewController*)[self.arrayARRestaurantViews objectAtIndex:0]).radialDistance doubleValue];
		self.maximumDistanceRange = self.maximumScaleDistance;
	}
}


// 증강현실뷰 업데이트
- (void)updateARRestaurantViews
{
#ifdef DEBUG_MODE
	// 디버그 레이블에 디버그 하기
	NSString *debugData = [NSString stringWithFormat:@"방위각:%.2f, 경사각:%.2f, 위도:%f, 경도:%f", radiansToDegrees(self.centerAzimuth), self.centerInclination, self.centerLocation.coordinate.latitude, self.centerLocation.coordinate.longitude];
	self.debugLabel.text = debugData;
#endif
	
	// 맛집 증강현실뷰가 없으면 업데이트 하지 않는다.
	if (!self.arrayARRestaurantViews || [self.arrayARRestaurantViews count] == 0) 
	{
		return;
	}

	for (ARRestaurantViewController *controller in self.arrayARRestaurantViews)
	{		
		// ---------------------------------------------------------------------------------------------------
		//	인스턴스가 뷰포트(화면)안에 있으며, 보여줄 최대허용범위안에 있는지 판단하여
		//	화면 안에 있다면 좌표, 스케일, 회전변환을 실시하고, ARRoot뷰의 서브뷰로 추가한다.
		//	화면 안에 존재하지않는다면 맛집 증강현실뷰를 그릴필요가 없으므로, ARRoot뷰의 서브뷰에서 없앤다. 즉 슈퍼뷰를 제거한다.
		// ---------------------------------------------------------------------------------------------------
		if ([self isContainsInViewPort:controller] && [controller.radialDistance doubleValue] <= self.maximumScaleDistance)
		{
			// 맛집의 거리비율 - 현재 맛집의 거리 / 맛집중 최대 먼 거리 : 아래의 계산에서 2번 쓰인다.
			double distanceRatio = [controller.radialDistance doubleValue] / self.maximumScaleDistance;
			
			// -----------------------------------------------------
			//	뷰포트안에 있다면 ARRoot뷰에 보일 위치좌표 가져온후 좌표설정.
			// -----------------------------------------------------
			CGPoint loc = [self pointInARRootView:controller];
			
			// 좌표 설정.
			if (isSingleAR)
			{
				controller.view.center = CGPointMake(loc.x, loc.y);
			}
			else
			{
				// 멀리있는 맛집일수록 y좌표를 빼준다. 올린다. 어느정도 위로 올려야 할지 계산
				CGFloat additional_Y_point = kMaximumDistanceFactorBasedOnPerspective * distanceRatio;
				controller.view.center = CGPointMake(loc.x, loc.y - additional_Y_point);
			}
			
			
			// ------------------------------------------------------------
			//	현재 아이폰의 위치와 맛집의 거리차를 이용하여 스케일과 회전변환을 적용.
			// ------------------------------------------------------------
			
			// 변환 초기화.
			CATransform3D transform = CATransform3DIdentity;
			
			// 거리에 따라서 스케일을 조정.
			CGFloat scaleFactor = 1.0;
			if (!isSingleAR)
			{
				scaleFactor = 1.0 - self.minimumScaleFactor * distanceRatio;
			}
			
			// 스케일 변환
			transform = CATransform3DScale(transform, scaleFactor, scaleFactor, scaleFactor);
			
			// 회젼 변환
			if (self.rotateViewsBasedOnPerspective) 
			{
				transform.m34 = 1.0 / 300.0;
				
				double controllerAzimuth = controller.azimuth;
				double curruntCenterAzimuth = self.centerAzimuth;
				if (controllerAzimuth - curruntCenterAzimuth > M_PI) 
					curruntCenterAzimuth += 2*M_PI;
				if (controllerAzimuth - curruntCenterAzimuth < -M_PI) 
					controllerAzimuth += 2 * M_PI;
				double azimuthDifference = controllerAzimuth - curruntCenterAzimuth;
				
				transform = CATransform3DRotate(transform, self.maximumRotationAngle * azimuthDifference / (VIEWPORT_HEIGHT_RADIANS / 2.0) , 0, 1, 0);
			}
			
			// 변환 적용하기.
			controller.view.layer.transform = transform;
			

			// -------------------------------------------------------------------------------------
			//	맛집 증강현실뷰가 ARRoot뷰의 서브뷰로 되어 있는지 확인. 즉 슈퍼뷰가 있는지 확인하여 없다면 추가한다.
			// -------------------------------------------------------------------------------------
			if (!(controller.view.superview)) 
			{
				[self.arRootView addSubview:controller.view];
			}
			
			[self.arRootView sendSubviewToBack:controller.view];
		}
		// 슈퍼뷰에서 제거하고 변환 초기화 한다.
		else 
		{
			[controller.view removeFromSuperview];
			controller.view.transform = CGAffineTransformIdentity;
		}
	}
}


// 맛집 증강현실뷰가 뷰포트에 속해있는지 확인하는 메소드
- (BOOL)isContainsInViewPort:(ARRestaurantViewController *)controller
{
	// --------------------------------------------------
	// 아이폰화면의 넓이, 즉 방위각이 화면에 속하는지 확인하틑 코드
	// --------------------------------------------------
	
	// 현재 아이폰방위각을 중심으로 아이폰화면(뷰)의 왼쪽끝 방위각
	// 만약 아이폰이 90도(실제로는 라디안)를 향하고 있으면 화면의 왼쪽은 VIEWPORT_WIDTH_RADIANS의 반절 각도를 빼주면 아이폰화면 왼쪽의 방위각이 나온다.
	// VIEWPORT_WIDTH_RADIANS를 0.5로 정했으므로 반절인 0.25라디안을 각도로 변환하면 대략 14도이므로 화면 왼쪽끝의 각도는 76도 오른쪽은 104도가 된다.
	double leftAzimuth = self.centerAzimuth - VIEWPORT_HEIGHT_RADIANS / 2.0;
	
	// 왼쪽이 0도 이하라면 보정해준다. -10도는 350도라고 할수 있으므로!! - 역시 라디안값이다.
	if (leftAzimuth < 0.0) 
	{
		leftAzimuth = 2 * M_PI + leftAzimuth;
	}
	
	// 화면의 오른쪽 방위각을 구한다.
	double rightAzimuth = self.centerAzimuth + VIEWPORT_HEIGHT_RADIANS / 2.0;
	
	// 역시 보정해준다.
	if (rightAzimuth > 2 * M_PI) 
	{
		rightAzimuth = rightAzimuth - 2 * M_PI;
	}
	
	// 인스턴스의 방위각이 왼쪽화면 끝의 방위각과 오른쪽 끝의 방위각 사이에 있는지 확인한다.
	BOOL result = (controller.azimuth > leftAzimuth && controller.azimuth < rightAzimuth);
	
	// 왼쪽방위각이 오른쪽 방위각보다 큰경우, 즉 왼쪽방위각이 0이하가 되어 2PI에서 빼주게 되는 경우
	if(leftAzimuth > rightAzimuth) 
	{
		result = (controller.azimuth < rightAzimuth || controller.azimuth > leftAzimuth);
	}
	
	
	// --------------------------------------------------
	// 아이폰화면의 높이, 즉 경사각이 화면에 속하는지 확인하틑 코드
	// --------------------------------------------------
	
	// 좌우가 아닌 상하방향으로도 속해있는지 확인한다. 경사각 확인.
	double bottomInclination = self.centerInclination - VIEWPORT_WIDTH_RADIANS / 2.0;
	double topInclination = self.centerInclination + VIEWPORT_WIDTH_RADIANS / 2.0;
	
	// 모든 증강현실뷰는 경사각이 0도 이다. 따라서 현재 아이폰의 경사각으로 구한 위, 아래의 경사각과 비교한다.
	result = result && (bottomInclination < 0 && topInclination > 0);
	
	
	// ------------------------------------------------
	// 인자값으로 들어온 인스턴스의 위치가 뷰포트에 속해 있는가?
	// ------------------------------------------------
	return result;	
}


// ARRoot뷰안의 맛집 증강현실뷰 위치
- (CGPoint)pointInARRootView:(ARRestaurantViewController *)controller
{
	// ----------------------------
	// 좌표 변수 선언
	// ----------------------------	
	CGPoint point;
	
	
	// ----------------------------
	// X좌표 구하기 코드
	// ----------------------------
	
	// 인자로 넘어온 ARCoordinate 인스턴스의 방위각
	double pointAzimuth = controller.azimuth;
	
	// 뷰포트(화면)의 왼쪽 방위각
	double leftAzimuth = self.centerAzimuth - VIEWPORT_HEIGHT_RADIANS / 2.0;
	
	// 왼쪽방위각의 보정 실시.
	if (leftAzimuth < 0.0)
	{
		leftAzimuth = 2 * M_PI + leftAzimuth;
	}
	
	// x좌표값을 구한다.
	if (pointAzimuth < leftAzimuth) 
	{
		// viewportContainsCoordinate에서 이미 확인하였기때문에
		// 이 분기문으로 들어올수 없을것으로 예상됨.
		// 아니면 잘못 이해 한 것일수도 있음.
		point.x = ((2 * M_PI - leftAzimuth + pointAzimuth) / VIEWPORT_HEIGHT_RADIANS) * self.arRootView.bounds.size.width;
	}
	else
	{
		// (pointAzimuth - leftAzimuth) / VIEWPORT_WIDTH_RADIANS
		// 뷰포트(화면)의 왼쪽에서 얼만큼 떨어져있는지 비율을 구함.
		// 뷰포트(화면)의 width사이즈에 위에서 구한 비율을 곱하여 x좌표의 위치를 얻는다.
		point.x = ((pointAzimuth - leftAzimuth) / VIEWPORT_HEIGHT_RADIANS) * self.arRootView.bounds.size.width;
	}
	
	// ----------------------------
	// Y좌표 구하기 코드
	// ----------------------------
		
	// (pointInclination - topInclination) / VIEWPORT_HEIGHT_RADIANS)
	// 뷰포트(화면)의 아래에서 얼만큼 떨어져있는지 비율을 구함.
	// y좌표는 아래로 내려갈수록 증가하지만 경사각은 화면을 위로 올릴수록 증가하므로 계산에 주의해야 한다.
	double topInclination = self.centerInclination - VIEWPORT_WIDTH_RADIANS / 2.0;
	point.y = self.arRootView.bounds.size.height - ((0 - topInclination) / VIEWPORT_WIDTH_RADIANS) * self.arRootView.bounds.size.height;
	
	
	// ----------------------------
	// 좌표값 리턴하기
	// ----------------------------
	return point;
}



// 증강현실뷰 정렬 메소드 - radialDistance를 기준으로 하여 정렬
- (void)sortingARRestaurantViewByRadialDistance
{
	NSSortDescriptor *distanceSort = [[NSSortDescriptor alloc] initWithKey:@"radialDistance" ascending:YES];
	[self.arrayARRestaurantViews sortUsingDescriptors:[NSArray arrayWithObjects:distanceSort, nil]];
	[distanceSort release];
}


#pragma mark -
#pragma mark 슬라이더 이벤트
// 맛집뷰에서 되돌아오기위한 네비게이션바의 뒤로가기 버튼 이벤트
- (void)touchButton_backFromRestaurantView:(id)sender
{
	//NSLog(@"네이게이션뷰의 뒤로가기 버튼 터치");
}


// 증강현실뷰 슬라이더 값 변화
- (void)changeSliderValue:(id)sender
{
	[self drawDistanceLabel];
	
	// 최대거리를 변화시킴
	self.maximumScaleDistance = self.distanceSlider.value;
}


// 슬라이더 터치 이벤트 - 투명도를 설정
- (void)touchDownSlider:(id)sender
{
	self.distanceSlider.alpha = kARSliderAlphaValueMax;
	self.distanceLabel.alpha = kARSliderAlphaValueMax;
}


- (void)touchUpSlider:(id)sender
{
	self.distanceSlider.alpha = kARSliderAlphaValueMin;
	self.distanceLabel.alpha = kARSliderAlphaValueMin;
}



// 레이블 값 그리기
- (void)drawDistanceLabel
{
	if (self.distanceSlider.value < 1000)
	{
		self.distanceLabel.text = [NSString stringWithFormat:LABEL_FORMAT_FOR_METER, (int)self.distanceSlider.value];
	}
	else
	{
		self.distanceLabel.text = [NSString stringWithFormat:LABEL_FORMAT_FOR_KILO_METER, self.distanceSlider.value/1000];
	}
}

#pragma mark -
#pragma mark 가속센서 델리게이트 메소드
// 선언 및 전역변수
#define kFilteringFactor 0.03					// basic low-pass filter factor
#define kAcell_Z_Avlue_CorrectionFactor 0.12	// z값에 일정한 보정값을 더하여 아래로 바라보게 한다.
#define kAcellToAngleDeltaFactor 0.5			// 각도 변화를 서서히 주고 싶을때 사용하는 보정값
UIAccelerationValue rollingZ;					// 수치계산에 필요한 변수

// 가속센서 델리게이트 메소드 구현.
// 센서값을 가지고 아이폰의 경사각을 구한다.
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
	// 가속센서값이 부드럽게 변할수 있도록 basic low-pass filter 사용
	rollingZ = ((acceleration.z + kAcell_Z_Avlue_CorrectionFactor) * kFilteringFactor) + (rollingZ * (1.0 - kFilteringFactor));
	
	// 현재 사용자가 가지고 있는 아이폰의 경사각 구하기
	// 90을 곱하는 이유 :	아이폰의 가속센서는 -1 ~ 1의 값을 가진다. 눕이면 1, 뒤집으면 -1이므로 각도료 표현하면 90도, -90도가 된다.
	//					따라서 90을 곱하여 바로 각도를 구할수 있게 된다.
	self.centerInclination = degreesToRadians(rollingZ * 90 * kAcellToAngleDeltaFactor);
}


#pragma mark -
#pragma mark 코어로케이션 델리게이트 메소드

// 선언 및 전역변수
#define kHeadingFilteringFactor 0.1		// 값의 변화를 부드럽게 하기위한 상수
#define kMaximumEpsilon 100				// 값의 변화차이가 심하여 격차가 벌어질경우 허용하는 최대변화량
double heading;							// 값의 변화를 저장할 공간


- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading 
{
	// ---------------------------------------------------------------------------------------------------------------------------------
	// newHeading의 magneticHeading값은 0~360도이다.
	// 만약 newHeading.magneticHeading값의 변화가 빨라서 350도 정도에서 10도 정도로 바뀐다면 부드럽게 바뀌기 위한 basic low-pass filter를 사용하므로
	// heading변수의 값이 350 -> 175 -> 90 -> 45 -> 10 이렇게 바뀌기 때문에 오버레이뷰들이 획 도는 현상이 발생한다.
	// 따라서 값의 변화가 일정이상 차이가 나면 basic low-pass filter를 사용하지 않고 바로 newHeading.magneticHeading의 값을 heading변수에 바로 대입한다.
	// ---------------------------------------------------------------------------------------------------------------------------------
	
	// 값의 변화가 크다면 그대로 대입한다.
	if (ABS(newHeading.magneticHeading - heading) > kMaximumEpsilon)
	{
		heading = newHeading.magneticHeading;
	}
	// 그렇지 않으면 부드럽게 하기위한 basic low-pass filter를 사용한다.
	else
	{
		heading = (newHeading.magneticHeading * kHeadingFilteringFactor) + (heading  * (1.0 - kHeadingFilteringFactor));
	}
	
	
	// ----------------------------------------------------
	// 아이폰을 옆으로 놓았다면 heading의 값이 90도 차이가 발생한다.
	// 따라서 90도를 더해준다.
	// ----------------------------------------------------
	double centerAzimuthForRotate = fmod(heading, 360.0) * (M_PI / 180.0);
	centerAzimuthForRotate += (M_PI / 2);
		
	// 각도가 360을 넘으면 360도를 빼줘서 0도부터 시작하게 만든다.
	if (centerAzimuthForRotate >= 2 * M_PI )
	{
		centerAzimuthForRotate -= 2 * M_PI;
	}
	
	// 현재의 방위각 설정.
	self.centerAzimuth = centerAzimuthForRotate;
	
	// 방향이 바뀌므로 업데이트 실시
	[self updateARRestaurantViews];
}


- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager 
{	
	return YES;
}


- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation 
{
	// -------------------------------------------------------------------
	//	로케이션의 값이 주기적으로 계속 들어와서 필요없는 업데이트가 자주 일어나게 된다.
	//	distanceFilter를 이용하여 어느정도 거리이동이 있는경우만 이 메소드가 호출 !!
	// -------------------------------------------------------------------
	[self setMyCenterLocation:newLocation.coordinate];
	
	// ----------------------------------------------------------
	//	증강현실 뷰를 한개 보는것이 아니라면 주변검색을 하는것이다.
	//	따라서 리버스 지오코더를 이용하여 주소를 찾고,
	//	주소의 locality값을 이용하여 데이터베이스에서 맛집데이터를 가져온다.
	//	리버스 지오코더를 생성한다. - 한번만 생성한다!!!
	// ----------------------------------------------------------
	if (!self.isSingleAR)
	{
		if (!self.startFlag)
		{
			[self.activityIndicatorView startAnimating];
			
			// 리버스 지오코더 생성
			MKReverseGeocoder *reverseGeocoderForAR = [[MKReverseGeocoder alloc] initWithCoordinate:newLocation.coordinate];
			self.reverseGeocoder = reverseGeocoderForAR;
			[reverseGeocoderForAR release];
			self.reverseGeocoder.delegate = self;
			[self.reverseGeocoder start];
			
			// 시작 플래그 활성화
			self.startFlag = YES;
		}
	}
//	else
//	{
//		[self.activityIndicatorView stopAnimating];
//	}

	
	// 주소정보를 얻지 못하였을 경우의 업데이트
	if (self.isNotFoundAddressFromReversegeocoder)
	{
		CLLocation *centerLocation = [[CLLocation alloc] initWithLatitude:centerCoord.latitude longitude:centerCoord.longitude];
		CLLocation *firstcenterLocation = [[CLLocation alloc] initWithLatitude:firstCenterCoord.latitude longitude:firstCenterCoord.longitude];
		
		// 처음 좌표와 1km이상 차이가 난다면 모든맛집 데이터로부터 다시 ARRestaurantView를 재 생성한다.
		if ([centerLocation distanceFromLocation:firstcenterLocation] > kMaximumDistanceFilterForAllRestaurants)
		{
			// firstCenterCood값 설정.
			firstCenterCoord = centerCoord;
			
			[self generateARRestaurantViewFromArrayAllRestaurants];
		}
		
		[centerLocation release];
		[firstcenterLocation release];
	}
}


- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error 
{
	
}


#pragma mark -
#pragma mark 리버스 지오코더 델리게이트 메소드
- (void)reverseGeocoder:(MKReverseGeocoder *)geocoder didFindPlacemark:(MKPlacemark *)placemark
{
	// 지오코더 제거
	[self removeReverseGeocoder];
	
	// 지오코더로부터 얻은 주소로부터 데이터베이스 검색
	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = connector;
	[connector release];
	self.dbConnector.arrayRestaurantsFromAreaDelegate = self;
	[self.dbConnector startReceivingArrayRestaurantsFromArea:placemark.locality];
}


- (void)reverseGeocoder:(MKReverseGeocoder *)geocoder didFailWithError:(NSError *)error
{	
	// 지오코더 제거
	[self removeReverseGeocoder];

	// 모든데이터 가져오기 시작
	[self startAllRestaurnatDataAR];
}

#pragma mark -
#pragma mark 리버스 지오코더 관련 메소드
// 지오코더 제거
- (void)removeReverseGeocoder
{
	self.reverseGeocoder.delegate = nil;
	[self.reverseGeocoder cancel];
	self.reverseGeocoder = nil;	
}


#pragma mark -
#pragma mark 종료 메소드
- (void)viewControllerDidFinish
{
	// 지오코더 삭제
	[self removeReverseGeocoder];
	
	// 업데이트 종료
	[self stopLocationManagerUpdating];
	
	// 종료 플래그
	self.wantStop = YES;
		
	// 카메라뷰컨트롤러가 생성되어있다면 모달뷰를 dismiss시킨다.
	if (self.cameraController) 
	{
		[self dismissModalViewControllerAnimated:NO];
	}	
	
	// 종료 델리게이트 호출
	[self.delegate ARViewControllerDidFinish:self];
}

						
// 종료버튼 이벤트
- (void)touchButton_back:(id)sender
{
	[self viewControllerDidFinish];
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
		// 맛집데이터를 추가하여 증강현실뷰 생성하기
		[self addARRestaurantViewWithRestaurantArrays:array];
				
		// 증강현실뷰 거리계산 업데이트 실시
		[self setMyCenterLocation:centerCoord];
	}
	else
	{
		[self startAllRestaurnatDataAR];
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
		//RestaurantViewController *viewController = [[RestaurantViewController alloc] initWithRestaurant:newRestaurant];
		RestaurantPreViewController *viewController = [[RestaurantPreViewController alloc] initWithRestaurant:newRestaurant];
		
		// 뒤로가기버튼 생성을 위한 설정.
		viewController.makeBackButtonForAR = YES;
		
		// 맛집뷰를 루트뷰로 하는 네비게이션뷰 컨트롤러 생성
		UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
			
		// 생성한 맛집-네비게이션뷰 컨트롤러를 모달뷰로 추가한다.
		[self.cameraController presentModalViewController:naviController animated:YES];
		
		// 생성한 뷰 릴리즈
		[viewController release];
		[naviController release];
	}
	// 검색결과가 존재하지 않는다면
	else
	{
		// 알람 뷰 생성
		[self makeAlertView:@"맛집데이터가 존재하지 않습니다."];
	}	
}



#pragma mark -
#pragma mark 맛집 증강현실 뷰 터치 델리게이트
- (void)ARRestaurantViewControllerDidTouched:(ARRestaurantViewController *)controller
{
	if (!isSingleAR)
	{
		//액티비티 인디케이터 뷰 애니메이션 시작
		[self.activityIndicatorView startAnimating];
		
		// 데이터베이스 커넥터 생성
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		self.dbConnector = connector;
		[connector release];
		self.dbConnector.restaurantFromIdDelegate = self;
		[self.dbConnector startReceivingRestaurantFromId:controller.restaurantData.id];
	}
}


#pragma mark -
#pragma mark 주소정보 받지 못할 경우 필요한 메소드
// 주소정보를 받지 못해 모든 데이터로 시작해야하는 경우 발생하는 메소드
- (void)startAllRestaurnatDataAR
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
- (void)generateARRestaurantViewFromArrayAllRestaurants
{
	// 증강현실뷰가 있다면 삭제한다.
	[self removeAllARRestaurantViews];
	
	// 모든 맛집데이터로부터 적절한 거리에 있는 맛집데이터 array를 얻어온다.
	NSMutableArray *arrayForRestaurant = [self getRestaurantsFromArryAllRestaurant];
	
	// 증강현실뷰를 만든다.
	[self addARRestaurantViewWithRestaurantArrays:arrayForRestaurant];
	
	// 증강현실뷰 거리계산 업데이트 실시
	[self setMyCenterLocation:centerCoord];
}


// arrayAllRestaurants로부터 거리차 구하여 restaurant 얻기
- (NSMutableArray*)getRestaurantsFromArryAllRestaurant
{
	NSMutableArray *array = [NSMutableArray array];
	
	CLLocation *centerLocation = [[CLLocation alloc] initWithLatitude:centerCoord.latitude longitude:centerCoord.longitude];
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
	
	// 맛집 증강현실뷰 생성
	[self generateARRestaurantViewFromArrayAllRestaurants];
	
	// 인디케이터 뷰 애니메이션 멈춤
	[self.activityIndicatorView stopAnimating];
}

						
@end
