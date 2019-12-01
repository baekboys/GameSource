#import "ARRestaurantViewController.h"
#import "Restaurant.h"
#import "RestaurantViewController.h"

@implementation ARRestaurantViewController


#pragma mark -
#pragma mark synthesize
@synthesize imageButton, distanceLabel, isUseInterfaceBuilder;
@synthesize restaurantData;
@synthesize radialDistance;
@synthesize azimuth;
@synthesize touchDelegate;


#pragma mark -
#pragma mark 증강현실 맛집뷰 인스턴스 생성 및 삭제
// 맛집데이터와 함께 초기화 - 인터페이스빌더를 사용하지 않고 초기화
- (id)initWithRestaurant:(Restaurant*)restaurant
{
	if(self = [super init])
	{
		// 맛집 데이터 설정
		self.restaurantData = restaurant;
		
		// 인터페이스 빌더 사용?
		self.isUseInterfaceBuilder = NO;
	}
	
	return self;
}


// 인터페이스빌더를 사용하는 경우, 맛집데이터와 함께 initWithNibName 초기화
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurant:(Restaurant*)restaurant
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		// 맛집데이터 설정
		self.restaurantData = restaurant;
		
		// 인터페이스 빌더 사용?
		self.isUseInterfaceBuilder = YES;
	}
	
	return self;
}


// 버튼과 레이블 생성
- (void)makeButtonAndLabel
{
	//	이미지버튼 생성
	UIButton *button = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, kButtonWidth, kButtonHeight)];
	self.imageButton = button;
	[button release];
	
	//	이미지 버튼의 폰트 설정
	self.imageButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kButtonFontSize];

	//	레이블 생성
	UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, kButtonWidth, kLabelWidth, kLabelHeight)];
	self.distanceLabel = label;
	[label release];
	
	//	레이블의 폰트 설정
	self.distanceLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kLabelFontSize];

}


// 프로그래밍으로 생성할때 호출되는 메소드
-(void)loadView
{
	// -------------------------------------------
	//	컨트롤러의 뷰 만들기 - 즉 가장 최상위 뷰를 만든다.
	// -------------------------------------------
	self.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, kTotalViewWidth, kTotalViewHeight)];
	self.view.userInteractionEnabled = YES;
	
	[self makeButtonAndLabel];
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	// 인터페이스빌더때문에 버튼과 레이블이 아웃렛으로 연결되어 초기화에서 생성되지 않는다.
	if (isUseInterfaceBuilder)
	{
		// 버튼 타이틀 설정
		[self.imageButton setTitle:self.restaurantData.name forState:UIControlStateNormal];
		
		// 레이블 설정
		[self drawLabelFromDistance];
	}
	else
	{
		// --------------
		//	이미지버튼 설정
		// --------------
		
		self.imageButton.userInteractionEnabled = YES;
		
		UIImage *buttonImage = [UIImage imageNamed:kButtonImageFileName];
		[self.imageButton setBackgroundImage:buttonImage forState:UIControlStateNormal];
		[self.imageButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
		self.imageButton.backgroundColor = [UIColor clearColor];
		
		if (self.restaurantData)
		{
			[self.imageButton setTitle:self.restaurantData.name forState:UIControlStateNormal];
			self.imageButton.titleLabel.numberOfLines = 2;
			self.imageButton.titleLabel.adjustsFontSizeToFitWidth = NO;
		}
		
		// 버튼이벤트 설정
		[self.imageButton addTarget:self action:@selector(touchtButton:) forControlEvents:UIControlEventTouchUpInside];
		
		// 가장 최상위뷰에 버튼 추가
		[self.view addSubview:self.imageButton];

		
		// -----------
		//	레이블 설정
		// -----------
		self.distanceLabel.textAlignment = UITextAlignmentCenter;
		self.distanceLabel.textColor = [UIColor whiteColor];
		self.distanceLabel.backgroundColor = [UIColor clearColor];
		[self drawLabelFromDistance];
		
		
		// 가장 최상위뷰에 레이블 추가
		[self.view addSubview:self.distanceLabel];
	}

}


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.imageButton = nil;
	self.distanceLabel = nil;
	self.restaurantData = nil;
	self.radialDistance = nil;
}


- (void)dealloc 
{
//	[self.imageButton release];
//	[self.distanceLabel release];
//	[self.restaurantData release];
//	[self.radialDistance release];
	
	self.imageButton = nil;
	self.distanceLabel = nil;
	self.restaurantData = nil;
	self.radialDistance = nil;
	
    [super dealloc];
}

#pragma mark -
#pragma mark 증강현실을 위한 메소드
// 방위각을 알아내기
// 증강현실 구현을 위하여 아이폰을 가지고 있는 사용자의 위치와 목표로 하는 맛집 위치의 각도이다.
- (float)angleFromCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
{
	// -----------------------------------------------------------------------------
	//	맛집 인스턴스의 위도-경도위치에서 아이폰을 가지고 있는 사용자의 위도-경도 위치를 빼줘야한다.
	//	뺄셈의 차례가 바뀐다면 180도 위치가 달라지기 때문에 주의해야 한다.
	//	맛집 위치 - 현재위치 !!
	// -----------------------------------------------------------------------------
 	
	// 두위치의 경도차이
	float longitudinalDifference =  self.restaurantData.longitude - centerCoordinate.longitude;
	
	// 두위치의 위도차이
	float latitudinalDifference = self.restaurantData.latitude - centerCoordinate.latitude;
	
	// 방위각 = atan2( dy, dx)
	float possibleAzimuth = atan2(longitudinalDifference, latitudinalDifference);
	
	// 탄젠트의 범위는 -180 ~ 180도. 라디안으로 표현하면 -파이 ~ 파이의 범위.
	if (possibleAzimuth < 0)
	{
		// 음수이면 360도 (2파이)를 더해준다.
		possibleAzimuth = (2 * M_PI) + possibleAzimuth;
	}
	
	return possibleAzimuth;
}


// 다른 위치(실제로는 아이폰의 위치)의 값을 통하여 이 인스턴스의 멤버변수 값을 설정.
- (void)calibrateFromCenterCoordinate:(CLLocationCoordinate2D)coordinate
{
	// -------------------------
	//	현재위치의 로케이션 인스턴스
	// -------------------------
	CLLocation *locationFromCenter = [[CLLocation alloc] initWithLatitude:coordinate.latitude longitude:coordinate.longitude];

	
	// ----------------------
	//	맛집의 로케이션 인스턴스
	// ----------------------
	CLLocation *locationFromThisCoordinate = [[CLLocation alloc] initWithLatitude:self.restaurantData.latitude longitude:self.restaurantData.longitude];
	
	
	// -----------------------------------------------
	//	2차원 평면간의 직선 거리.(지도상에서의 거리, 미터단위)
	// -----------------------------------------------
	self.radialDistance = [NSNumber numberWithDouble:[locationFromCenter distanceFromLocation:locationFromThisCoordinate]];
	
	
	// -----------------------
	//	레이블에 직선거리 표시하기
	// -----------------------
	[self drawLabelFromDistance];
		
	// ---------------------------------------------------------
	//	angleFromCoordinate 메소드 이용하여 이 인스턴스의 방위각 설정.
	// ---------------------------------------------------------
	self.azimuth = [self angleFromCenterCoordinate:coordinate];
	
	
	// -----------
	//	릴리즈 하기
	// -----------
	[locationFromCenter release];
	[locationFromThisCoordinate release];
}


- (void)drawLabelFromDistance
{
	// 레이블에 표시할 스트링
	NSString *distanceString;
	
	// 거리 받아오기
	double distance = [self.radialDistance intValue];
	
	// 거리차가 1km이상이라면 포맷형식을 km단위로 바꾸기
	if (distance >= 1000)
	{
		distance = distance / 1000;
		distanceString = [NSString stringWithFormat:@"%.1fKm", distance];
	}
	// 거리차가 1km미만이라면 미터단위로 그대로 쓰기
	else
	{
		distanceString = [NSString stringWithFormat:@"%dm", (int)distance];
	}
	
	// 레이블에 저장
	self.distanceLabel.text = distanceString;	
}

#pragma mark -
#pragma mark 터치 이벤트
- (IBAction)touchtButton:(id)sender
{
	[self.touchDelegate ARRestaurantViewControllerDidTouched:self];
}

@end
