#import "UserReviewDetailViewController.h"
#import "UserReview.h"
#import "SHARED_DEFINE.h"

@implementation UserReviewDetailViewController


@synthesize starLabel;
@synthesize textView;
@synthesize userReview;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withUserReview:(UserReview*)newUserReview
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		self.userReview = newUserReview;
		
		// 타이틀 설정
		self.title = @"맛집톡톡";
    }
	
    return self;
}


- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	
	// 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	
	// 네비게이션바의 배경화면 설정
	UIImage *navigatinoBarBgImage = [UIImage imageNamed:JBFOOD_NAVIGATION_BAR_BG_FILENAME];
	CGRect bgRect = self.navigationController.navigationBar.bounds;
	UIImageView *navigatinoBarBgImageView = [[UIImageView alloc] initWithFrame:bgRect];
	navigatinoBarBgImageView.image = navigatinoBarBgImage;
	[self.navigationController.navigationBar insertSubview:navigatinoBarBgImageView atIndex:0];
	[navigatinoBarBgImageView release];
	
		
	// 별점 출력
	NSMutableString *starString = [NSMutableString string];
	NSUInteger index;
	for (index = 0 ; index < self.userReview.star ; ++index)
	{
		[starString appendString:@"★"];
	}
	
	// 별점 텍스트
	self.starLabel.text = [self getStarStringFromStar:self.userReview.star];
	
	// 별점 폰트
	self.starLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[self.starLabel.font pointSize]];

	
	// 내용 출력
	self.textView.text = self.userReview.review;
	//self.textView.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[self.textView.font pointSize]];
	self.textView.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:25];
	
	
	// rect 생성
	CGRect rect = CGRectMake(0, 0, kMAINVIEW_WIDTH, kMAINVIEW_HEGIHT);
	
	// 네비게이션바가 있다면 네비게이션바의 높이사이즈만큼 줄인다.
	if (self.navigationController)
	{
		rect.size.height -= self.navigationController.navigationBar.frame.size.height;
	}
	
	// 탭바가 있다면 탭바의 높이사이즈만큼 줄인다.
	if (self.tabBarController)
	{
		rect.size.height -= self.tabBarController.tabBar.frame.size.height;
	}	
	
	self.view.frame = rect;
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	
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
	
	self.starLabel = nil;
	self.textView = nil;
}


- (void)dealloc 
{
	self.starLabel = nil;
	self.textView = nil;
	self.userReview = nil;
	
    [super dealloc];
}


// 평균 별점으로부터 문자열 별점으로 가져오기
- (NSString*)getStarStringFromStar:(NSUInteger)newStarAverage
{
	NSString *activeStar = @"★";
	NSString *deactiveStar = @"☆";
	
	NSMutableString *mutableString = [NSMutableString stringWithCapacity:5];
	
	NSUInteger index;
	for ( index = 1 ; index < 6 ; ++index ) 
	{
		if (index <= newStarAverage)
			[mutableString appendString:activeStar];
		else
			[mutableString appendString:deactiveStar];
	}
	
	return [NSString stringWithString:mutableString];
}


@end
