#import "FoodCulturePlazaViewController.h"

@implementation FoodCulturePlazaViewController

#pragma mark -
#pragma mark synthesize
@synthesize delegate;
@synthesize webView;
@synthesize barButton_main;
@synthesize urlString;

#pragma mark -
#pragma mark 초기화 메소드
-(id)initWithURL:(NSString*)newURLString withTitle:(NSString*)newTitle
{
	if (self = [super initWithNibName:@"FoodCulturePlazaViewController" bundle:nil]) 
	{
		self.urlString = newURLString;
		self.title = newTitle;
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
	
	// 타이틀 설정
	//self.title = @"음식문화 프라자";
	
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
	

	// url 연결
	NSURL *url = [NSURL URLWithString:self.urlString];
	NSURLRequest *request = [NSURLRequest requestWithURL:url];
	[self.webView loadRequest:request];
}


- (void)viewDidUnload 
{
	self.webView = nil;
	self.barButton_main = nil;
}


- (void)dealloc 
{
	self.webView = nil;
	self.barButton_main = nil;
	self.urlString = nil;
	
    [super dealloc];
}

#pragma mark -
#pragma mark 버튼 이벤트
- (IBAction)touchButton:(id)sender
{
	[self dismissModalViewControllerAnimated:YES];
}

@end
