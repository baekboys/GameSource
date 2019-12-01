#import "SearchBySurroundingRootController.h"


@implementation SearchBySurroundingRootController


@synthesize button_map, button_ar;


#pragma mark -
#pragma mark 초기화 메소드
- (id)init
{
	if (self = [super init]) 
	{
		self.title = @"주변 맛집";
	}
	return self;
}


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		self.title = @"주변 맛집";
	}
	return self;
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
    
}


- (void)viewDidLoad
{
	// 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];

	// 버튼의 폰트 설정
	self.button_map.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_map.font.pointSize];
	self.button_ar.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.button_ar.font.pointSize];
}


- (void)viewDidAppear:(BOOL)animated
{
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


- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.button_map = nil;
	self.button_ar = nil;
}


- (void)dealloc 
{
	self.button_map = nil;
	self.button_ar = nil;

    [super dealloc];
}


- (IBAction)touchButton:(id)sender
{
	if (sender == self.button_map)
	{
		// 주변검색 맵뷰 컨트롤러 생성
		MultiMapViewController *viewController = [[MultiMapViewController alloc] initWithNibName:@"MultiMapViewController" bundle:nil];
		viewController.delegate = self;
		
		// 주변검색 맵뷰 푸쉬.
		[self.navigationController pushViewController:viewController animated:YES];
		
		// array와 뷰컨트롤러 릴리즈
		[viewController release];
	}
	
	else if (sender == self.button_ar)
	{
		// 증강현실 뷰 컨트롤러 생성 - SingleAR을 NO로 초기화
		ARViewController *viewController = [[ARViewController alloc] initWithUsingSingleAR:NO];
		viewController.delegate = self;

		// 증강현실뷰 푸쉬.
		[self.navigationController pushViewController:viewController animated:NO];
		
		// array와 뷰컨트롤러 릴리즈
		[viewController release];
	}
}

// 증강현실뷰 델리게이트 구현 메소드
- (void)ARViewControllerDidFinish:(ARViewController *)controller
{
	[self.navigationController popViewControllerAnimated:YES];
}


- (void)MultiMapViewControllerDidFinish:(MultiMapViewController *)controller
{
	[self.navigationController popViewControllerAnimated:YES];
}

@end
