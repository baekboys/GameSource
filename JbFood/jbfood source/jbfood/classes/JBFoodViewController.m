#import "JBFoodViewController.h"

// 맛집 정보 탭바 컨트롤러ㅏ
#import "RestaurantInfoTabbarController.h"

// 전북 음식문화 플라자 뷰 컨트롤러
#import "FoodCulturePlazaViewController.h"

// 개발자 정보 뷰 컨트롤러
#import "UKGInfoViewController.h"

@implementation JBFoodViewController

#pragma mark -
#pragma mark synthesize
@synthesize button_foodInfo, button_culturePlaza, button_naverCafe, titleLabel, infoLabel;

#pragma mark -
#pragma mark 초기화 메소드
- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
	
}


- (void)viewDidLoad
{
	[super viewDidLoad];
	
	// 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_MAIN_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	
	// 버튼 폰트 설정
	self.button_foodInfo.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_foodInfo.font.pointSize];
	self.button_culturePlaza.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_culturePlaza.font.pointSize];
	self.button_naverCafe.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_naverCafe.font.pointSize];
	
	// 레이블 폰트 설정
	self.titleLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.titleLabel.font.pointSize];
	self.infoLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.infoLabel.font.pointSize];
}


- (void)viewDidUnload 
{
	self.button_foodInfo = nil;
	self.button_culturePlaza = nil;
	self.button_naverCafe = nil;
	self.infoLabel = nil;
	self.titleLabel = nil;
}


- (void)dealloc
{
	self.button_foodInfo = nil;
	self.button_culturePlaza = nil;
	self.button_naverCafe = nil;
	self.infoLabel = nil;
	self.titleLabel = nil;
	
    [super dealloc];
}


#pragma mark -
#pragma mark 버튼 이벤트
- (IBAction)touchButton:(id)sender
{
	// 맛집 정보 버튼을 눌렀을 경우
	if (sender == self.button_foodInfo)
	{
		// 맛집정보 탭바 컨트롤러 생성
		RestaurantInfoTabbarController *tabbarController = [[RestaurantInfoTabbarController alloc] init];
		
		// 애니메이션 설정
		tabbarController.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
		
		// 모달뷰컨트롤러로 붙임.
		[self presentModalViewController:tabbarController animated:YES];

		// 릴리즈
		[tabbarController release];
	}
	// 음식문화프라자 버튼을 눌렀을 경우
	else if (sender == self.button_culturePlaza)
	{
		// 음식문화프라자 뷰 컨트롤러 생성
		FoodCulturePlazaViewController *viewController = [[FoodCulturePlazaViewController alloc] initWithURL:FOOD_CULTURE_PLAZA_URL withTitle:FOOD_CULTURE_PLAZA_TITLE];

		// 네비게이션 컨트롤러 생성
		UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
		
		// 애니메이션 설정
		naviController.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;

		// 모달뷰컨트롤러로 붙임.
		[self presentModalViewController:naviController animated:YES];

		// 릴리즈.
		[viewController release];
		[naviController release];
	}
	// 네이버카페 버튼을 눌렀을 경우
	else if (sender == self.button_naverCafe)
	{
		// 네이버카페 뷰 컨트롤러 생성
		FoodCulturePlazaViewController *viewController = [[FoodCulturePlazaViewController alloc] initWithURL:NAVER_CAFE_URL withTitle:NAVER_CAFE_TITLE];
		
		// 네비게이션 컨트롤러 생성
		UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
		
		// 애니메이션 설정
		naviController.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
		
		// 모달뷰컨트롤러로 붙임.
		[self presentModalViewController:naviController animated:YES];
		
		// 릴리즈.
		[viewController release];
		[naviController release];
	}
}


- (IBAction)touchInfoButton:(id)sender
{
	UKGInfoViewController *viewController = [[UKGInfoViewController alloc] initWithNibName:@"UKGInfoViewController" bundle:nil];
	
	// 모달뷰컨트롤러로 붙임.
	[self presentModalViewController:viewController animated:YES];
	
	[viewController release];
}


- (IBAction)touchNaverCafe:(id)sender
{
	
}

@end
