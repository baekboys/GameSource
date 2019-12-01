#import "UserReviewResultTableViewController.h"
#import "SHARED_DEFINE.h"
#import "UserReviewResultCell.h"
#import "UserReview.h"
#import "UserReviewDetailViewController.h"

@implementation UserReviewResultTableViewController


@synthesize arrayUserReviews;


// 한줄평 뮤터블 배열로 초기화
- (id)iniWithArrayUserReviews:(NSArray*)newArrayUserReviews
{
	if (self = [super init])
	{
		self.arrayUserReviews = newArrayUserReviews;
		self.title = @"맛집톡톡";
	}
	return self;
}


- (void)viewDidLoad 
{
    [super viewDidLoad];
}


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
    
}


- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.arrayUserReviews = nil;
}


- (void)dealloc 
{
	self.arrayUserReviews = nil;
	
    [super dealloc];
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	
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
	
	// 네비게이션바의 타이틀뷰 설정
	UILabel *titleLabel = [[UILabel alloc] init];
	titleLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:JBFOOD_NAVIGATION_TITLE_FONT_SIZE];
	titleLabel.text = self.title;
	titleLabel.backgroundColor = [UIColor clearColor];
	[titleLabel sizeToFit];
	self.navigationItem.titleView = titleLabel;
	[titleLabel release];
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

#pragma mark -
#pragma mark 테이블뷰 구현 메소드
// 섹션의 갯수
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 1;
}


// 섹션별 row의 갯수
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (!self.arrayUserReviews)
	{
		return 0;
	}
	
    return [self.arrayUserReviews count];
}


// 셀 구현
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
	// 식별자 생성
	static NSString *CellIdentifier = @"UserReviewResultCellIdentifier";
	
	// 셀 가져오기
	UserReviewResultCell *cell = (UserReviewResultCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	
	// 셀이 존재하지 않는다면 새로 생성
	if (cell == nil)
	{
		// Xib파일로부터 셀을 생성한다.
		NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"UserReviewResultCell" owner:self options:nil];
		cell = (UserReviewResultCell*)[nib objectAtIndex:0];
		
		// 폰트 설정
		cell.userReview.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[cell.userReview.font pointSize]];
	}
	
	UserReview *review = (UserReview*)[self.arrayUserReviews objectAtIndex:indexPath.row];
	
	// 셀의 별점 설정
	cell.starString.text = [self getStarStringFromStar:review.star];
	
	// 셀의 한줄평 설정
	cell.userReview.text = review.review;
	
	
    return cell;
}


// 셀의 높이값 리턴
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	return kUserReviewResultCellHeight;
}


// 셀 터치 이벤트
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	UserReview *review = (UserReview*)[self.arrayUserReviews objectAtIndex:indexPath.row];
	
	UserReviewDetailViewController *viewController = [[UserReviewDetailViewController alloc] initWithNibName:@"UserReviewDetailViewController" bundle:nil withUserReview:review];
	[self.navigationController pushViewController:viewController animated:YES];
	[viewController release];
}


@end
