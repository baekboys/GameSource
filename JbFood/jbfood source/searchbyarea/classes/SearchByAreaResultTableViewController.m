#import "SearchByAreaResultTableViewController.h"

#import "Restaurant.h"						// 맛집 클래스
#import "RecommendationMenu.h"				// 추천 메뉴
//#import "RestaurantViewController.h"		// 맛집 정보를 보여주는 클래스
#import "RestaurantPreViewController.h"		// 맛집 정보 미리보기 클래스
#import "DatabaseConnector.h"				// 맛집 데이터베이스 클래스
#import "SearchByAreaResultTableCell.h"
#import "SHARED_DEFINE.h"

@implementation SearchByAreaResultTableViewController


#pragma mark -
#pragma mark synthesize
@synthesize areaResultTableView;
@synthesize indicatorView;
@synthesize arrayRestaurants;
@synthesize alertViewForDB;
@synthesize areaName;
@synthesize dbConnector;
@synthesize makeJoinButtonForRestaurantView;

#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithArrayRestaurants:(NSMutableArray*)newArrayRestaurants withAreaName:(NSString*)newName
{
	if (self = [super init])
	{
		// 맛집 데이터가 있는 array 설정.
		self.arrayRestaurants = newArrayRestaurants;
		
		// 타이틀 설정.
		self.title = newName;
		
		// 단골맛집 버튼 생성 플래그
		self.makeJoinButtonForRestaurantView = YES;
	}
	
	return self;
}


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
    
}


- (void)loadView
{
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
	
	// 메인뷰 생성
	UIView *mainView = [[UIView alloc] initWithFrame:rect];
	self.view = mainView;
	[mainView release];
	
	// 메인뷰 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	

	// 테이블 뷰 생성 및 설정
	UITableView *tableViewForResult = [[UITableView alloc] initWithFrame:rect style:UITableViewStylePlain];
	self.areaResultTableView = tableViewForResult;
	[tableViewForResult release];
	self.areaResultTableView.delegate = self;
	self.areaResultTableView.dataSource = self;
	self.areaResultTableView.backgroundColor = [UIColor clearColor];
	[self.view addSubview:self.areaResultTableView];
	
	
	// 액티비티 인티케이터 뷰 생성
	UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	self.indicatorView = aiView;
	[aiView release];
	self.indicatorView.frame = kActivityIndicatorViewFrameSizeOnNavi;
	self.indicatorView.hidesWhenStopped = YES;
	self.indicatorView.hidden = YES;
	[self.navigationController.navigationBar addSubview:self.indicatorView];	
}


- (void)viewDidLoad
{
	[super viewDidLoad];
}


- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.areaResultTableView = nil;
	self.indicatorView = nil;
	self.alertViewForDB = nil;
	self.areaName = nil;
	self.dbConnector = nil;	
	self.arrayRestaurants = nil;
}


- (void)dealloc 
{
	self.areaResultTableView = nil;
	self.indicatorView = nil;
	self.alertViewForDB = nil;
	self.areaName = nil;
	self.dbConnector = nil;	
	self.arrayRestaurants = nil;
	
    [super dealloc];
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


- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	if (self.dbConnector)
	{
		[self.dbConnector removeAllDelegateReferenceToNil];
	}
	
	[self.indicatorView stopAnimating];
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
	if (!self.arrayRestaurants)
	{
		return 0;
	}
	
    return [self.arrayRestaurants count];
}


// 셀 구현
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
	// 식별자 생성
	static NSString *CellIdentifier = @"SearchByAreaResultCellIdentifier";

	// 셀 가져오기
	SearchByAreaResultTableCell *cell = (SearchByAreaResultTableCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	
	// 셀이 존재하지 않는다면 새로 생성
	if (cell == nil)
	{
		// Xib파일로부터 셀을 생성한다.
		NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"SearchByAreaResultTableCell" owner:self options:nil];
		cell = (SearchByAreaResultTableCell*)[nib objectAtIndex:0];
		
		// 폰트 설정
		cell.nameLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kSearchByResultTableViewNameFontSize];
		cell.menuLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kSearchByResultTableViewMenuFontSize];
	}
	
	Restaurant *restaurant = (Restaurant*)[self.arrayRestaurants objectAtIndex:indexPath.row];
	
	// 셀의 이름레이블과 폰트 설정.
	cell.nameLabel.text = restaurant.name;
	
	// 셀의 메뉴 레이블과 폰트 설정.
	NSMutableString *menuString = [NSMutableString string];
	for (RecommendationMenu *menu in restaurant.recommendationMenu)
	{
		if ([restaurant.recommendationMenu lastObject] == menu)
			[menuString appendFormat:@"%@", menu.menu];
		else
			[menuString appendFormat:@"%@, ", menu.menu];
	}
	cell.menuLabel.text = menuString;
	
	// 셀의 별점 레이블 설정.
	if (restaurant.starCount > 0)
	{
		cell.starLabel.text = restaurant.starString;
		cell.starLabel.alpha = 1;
	}
	else
	{
		cell.starLabel.text = @"☆☆☆☆☆";
		cell.starLabel.alpha = 0.2;
	}

	
	// 셀의 섬네일이미지 설정 - 먼저 비어있는 이미지를 넣어두고 맛집 섬네일이미지가 다운로드가 가능한지 판단한다.
	cell.restaurantImageView.image = [UIImage imageNamed:JBFOOD_BLANK_SMALL_IMG_FILENAME];
	if ( [restaurant isPossibleDownloadImage] )
	{		
		switch (restaurant.imageDownloadState)
		{
			case IMG_DOWNLOAD_READY:
			{
				[restaurant startThumbnailDownloadingForImageView:cell.restaurantImageView];
			}	break;
				
			case IMG_DOWNLOAD_DOWNLOADING:
			{

			}	break;
				
			case IMG_DOWNLOAD_FINISHED:
			{
				UIImage *image = [UIImage imageWithData:restaurant.imageData];
				
				if (image)
				{
					cell.restaurantImageView.image = image;
				}
			}	break;
		}
	}

    return cell;
}


// 셀의 높이값 리턴
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	return kSearchByAreaResultTableCellHieght;
}


// 셀을 선택한 경우 발생되는 델리게이트 메소드
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	// indexPath를 통하여 해당하는 맛집의 id값을 가져온다.
	int idNumber = ((Restaurant*)[self.arrayRestaurants objectAtIndex:indexPath.row]).id;
	
	if (!self.dbConnector) 
	{
		// 액티비티 인디케이터 뷰 애니메이션 시작
		[self.indicatorView startAnimating];

		
		// 데이터베이스 커넥터 생성
		[self.dbConnector removeAllDelegateReferenceToNil];
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		self.dbConnector = connector;
		[connector release];
		self.dbConnector.restaurantFromIdDelegate = self;
		
		// 데이터 연결 시작
		[self.dbConnector startReceivingRestaurantFromId:idNumber];
	}	
}


#pragma mark -
#pragma mark 경고창 만들기 및 델리게이트
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


#pragma mark -
#pragma mark 알람뷰 델리게이트 메소드
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	switch (buttonIndex)
	{
			// 취소버튼을 눌렀을 경우
		case 0:
		{
			
		}	break;			
	}
}


#pragma mark -
#pragma mark 데이터베이스 연결 델리게이트
- (void)restaurantDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{

}


- (void)restaurantDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{
	// 데이터베이스 커넥터 종료
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 알람 뷰 생성
	[self makeAlertView:@"연결에 실패하였습니다."];
}


- (void)restaurantDidFinish:(DatabaseConnector *)connector withRestaurant:(Restaurant *)newRestaurant
{
	// 데이터베이스 커넥터 종료
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 검색결과가 존재한다면
	if (newRestaurant)
	{
		// 경고창 제거
		[self.alertViewForDB dismissWithClickedButtonIndex:0 animated:YES];
		self.alertViewForDB = nil;
		
		// 맛집뷰 생성
		//RestaurantViewController *viewController = [[RestaurantViewController alloc] initWithNibName:@"RestaurantViewController" bundle:nil withRestaurant:newRestaurant];
		
		// 맛집뷰의 단골맛집버튼 생성 플래그 설정.
		//viewController.makeJoinButtonForJoin = self.makeJoinButtonForRestaurantView;

		// 맛집 미리보기 뷰 생성
		RestaurantPreViewController *viewController = [[RestaurantPreViewController alloc] initWithRestaurant:newRestaurant];
		
		
		// 생성한 맛집뷰를 push
		if (self.navigationController)
		{
			[self.navigationController pushViewController:viewController animated:YES];
		}
		else
		{
			[self presentModalViewController:viewController animated:YES];
		}

		
		
		// 생성한 테이블뷰 릴리즈
		[viewController release];
		
	}
	// 검색결과가 존재하지 않는다면
	else
	{
		// 알람뷰 생성
		[self makeAlertView:@"데이터가 존재하지 않습니다."];
	}
	
}


#pragma mark -
#pragma mark 이름순으로 정렬 메소드
- (void)sortingArrayRestaurantsByName
{
	NSSortDescriptor *distanceSort = [[NSSortDescriptor alloc] initWithKey:@"name" ascending:YES];
	[self.arrayRestaurants sortUsingDescriptors:[NSArray arrayWithObjects:distanceSort, nil]];
	[distanceSort release];
	
}

@end
