#import "SearchMainViewController.h"
#import "SHARED_DEFINE.h"
#import "SearchByNameViewController.h"
#import "SearchBySituationViewController.h"

@implementation SearchMainViewController


@synthesize nameSearchButton;
@synthesize situationSearchButton;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil 
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) 
	{
		self.title = @"검색하기";
    }
    return self;
}



- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	
	// 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	
	
	// 버튼의 폰트 설정
	self.nameSearchButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.nameSearchButton.font.pointSize];
	self.situationSearchButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.situationSearchButton.font.pointSize];
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


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
	
}


- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.nameSearchButton = nil;
	self.situationSearchButton = nil;
}


- (void)dealloc 
{
	self.nameSearchButton = nil;
	self.situationSearchButton = nil;
    [super dealloc];
}


// 터치 이벤트 메소드
- (IBAction)touchButton:(id)sender
{
	if (sender == self.nameSearchButton)
	{
		// 상호명 검색 뷰 컨트롤러 생성
		SearchByNameViewController *viewController = [[SearchByNameViewController alloc] initWithNibName:@"SearchByNameViewController" bundle:nil];
		
		// 뷰 푸쉬.
		[self.navigationController pushViewController:viewController animated:YES];
		
		// array와 뷰컨트롤러 릴리즈
		[viewController release];

	}
	else if (sender == self.situationSearchButton)
	{
		// 상황별 검색 뷰 컨트롤러 생성
		SearchBySituationViewController *viewController = [[SearchBySituationViewController alloc] initWithNibName:@"SearchBySituationViewController" bundle:nil];
		
		// 뷰 푸쉬.
		[self.navigationController pushViewController:viewController animated:YES];
		
		// array와 뷰컨트롤러 릴리즈
		[viewController release];
	}
}

@end
