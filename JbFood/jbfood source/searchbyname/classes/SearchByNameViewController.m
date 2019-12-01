#import "SearchByNameViewController.h"
#import "SHARED_DEFINE.h"
#import "SearchByAreaResultTableViewController.h"


@implementation SearchByNameViewController


@synthesize nameSearchBar;
@synthesize alertViewForDB;
@synthesize indicatorView;
@synthesize dbConnector;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil 
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) 
	{
		self.title = @"통합 검색";
    }
    return self;
}


- (void)viewDidLoad 
{
    [super viewDidLoad];
	
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
	
	// 메인뷰 사이즈 조정
	self.view.frame = rect;
	
	// 메인뷰 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	
	// 액티비티 인티케이터 뷰 생성
	UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	self.indicatorView = aiView;
	[aiView release];
	self.indicatorView.frame = kActivityIndicatorViewFrameSizeOnNavi;
	self.indicatorView.hidesWhenStopped = YES;
	self.indicatorView.hidden = YES;
	[self.navigationController.navigationBar addSubview:self.indicatorView];
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
	
	self.nameSearchBar = nil;
	self.alertViewForDB = nil;
	self.indicatorView = nil;
	self.dbConnector = nil;
}


- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	if (self.dbConnector)
	{
		[self.dbConnector removeAllDelegateReferenceToNil];
	}
	
	[self.indicatorView stopAnimating];
}


- (void)dealloc 
{
	self.nameSearchBar = nil;
	self.alertViewForDB = nil;
	self.indicatorView = nil;
	self.dbConnector = nil;
	
    [super dealloc];
}


#pragma mark -
#pragma mark 검색바 델리게이트 메소드
// 검색버튼을 터치하였다면
- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar
{
	// 키보드 가리기
	[searchBar resignFirstResponder];
	
	// 액티비티 인디케이터 뷰 애니메이션 시작
	[self.indicatorView startAnimating];

	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = 	connector;
	[connector release];
	self.dbConnector.arrayRestaurantsFromNameDelegate = self;
	[self.dbConnector startReceivingArrayRestaurantsFromName:searchBar.text];		
}

- (void)searchBarCancelButtonClicked:(UISearchBar *) searchBar
{
	searchBar.text = nil;
	[searchBar resignFirstResponder];
}


#pragma mark -
#pragma mark 알람뷰 만들기 및 델리게이트
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
#pragma mark 상호명 검색 결과 맛집 array 전송 프로토콜 구현 메소드
- (void)arrayDidFail:(DatabaseConnector*)connector withError:(NSError*)data
{	
	// 데이터베이스 커넥터 종료
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 알람 뷰 생성
	[self makeAlertView:@"연결에 실패하였습니다."];
}


- (void)arrayDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
{

}


- (void)arrayDidFinish:(DatabaseConnector*)connector withArrayRestaurants:(NSMutableArray*)array
{
	// 데이터베이스 커넥터 종료
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 검색결과가 존재한다면
	if ([array count] > 0)
	{
		// 검색결과를 그려줄 테이블뷰 생성
		NSString *title = [NSString stringWithFormat:@"검색결과 : %d개", [array count]];
		SearchByAreaResultTableViewController *viewController = [[SearchByAreaResultTableViewController alloc] initWithArrayRestaurants:array withAreaName:title];
		
		// 생성한 테이블뷰를 push
		[self.navigationController pushViewController:viewController animated:YES];
		
		// 생성한 테이블뷰 릴리즈
		[viewController release];

	}
	// 검색결과가 존재하지 않는다면
	else
	{
		// 알람뷰 생성
		[self makeAlertView:@"검색결과가 없습니다."];
	}
}


@end
