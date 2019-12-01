#import "RestaurantViewController.h"
#import "Restaurant.h"
#import "RecommendationMenu.h"
#import "SingleMapViewController.h"
#import "ImageReviewCell.h"
#import "InfoCell.h"
#import "SHARED_DEFINE.h"
#import "SQLiteDB.h"
#import "UserReviewResultTableViewController.h"

@implementation RestaurantViewController

#pragma mark -
#pragma mark synthesize
@synthesize restaurant;
@synthesize makeBackButtonForAR;
@synthesize restaurantTableView;
@synthesize makeJoinButtonForJoin;
@synthesize dbConnector;
@synthesize indicatorView;


#pragma mark -
#pragma mark 맛집뷰 초기화, 메모리 관리
- (id)initWithRestaurant:(Restaurant*)newRestaurant
{
	if (self = [super init])
	{
		self.restaurant = newRestaurant;
		self.title = newRestaurant.name;
		self.makeBackButtonForAR = NO;
		self.makeJoinButtonForJoin = YES;
		self.dbConnector = nil;
	}
	return self;
}


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurant:(Restaurant*)newRestaurant
{
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) 
	{
		self.restaurant = newRestaurant;
		self.title = newRestaurant.name;
		self.makeBackButtonForAR = NO;
		self.makeJoinButtonForJoin = YES;
		self.dbConnector = nil;
	}
	
	return self;
}


// 코드로 뷰를 생성
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
	
	// 맛집 메인뷰 생성
	UIView *mainView = [[UIView alloc] initWithFrame:rect];
	self.view = mainView;
	[mainView release];
	
	// 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	
	
	// 맛집 테이블뷰 생성
	UITableView *tableViewForRestaurant = [[UITableView alloc] initWithFrame:rect style:UITableViewStyleGrouped];
	self.restaurantTableView = tableViewForRestaurant;
	[tableViewForRestaurant release];
	self.restaurantTableView.delegate = self;
	self.restaurantTableView.dataSource = self;
	self.restaurantTableView.backgroundColor = [UIColor clearColor];
	
	// 메인뷰에 테이블뷰를 서브뷰로 설정
	[self.view addSubview:self.restaurantTableView];
	
	// 액티비티 인티케이터 뷰 생성
	UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	self.indicatorView = aiView;
	[aiView release];
	self.indicatorView.frame = CGRectMake(0, 0, 50, 50);
	self.indicatorView.center = self.view.center;
	self.indicatorView.hidesWhenStopped = YES;
	self.indicatorView.hidden = YES;
	[self.view addSubview:self.indicatorView];
	
	
	// 이미지 다운로드 시작
	[self.restaurant startDownloadingForImageView:nil];
}


- (void)dealloc 
{
	self.restaurant = nil;
	self.restaurantTableView = nil;
	self.dbConnector = nil;
	
    [super dealloc];
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}


#pragma mark -
#pragma mark 맛집 뷰 관리
- (void)viewDidLoad 
{
    [super viewDidLoad];
	
}

- (void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	
	// 증강현실뷰는 카메라를 모달뷰로 하기때문에 네비게이션바의 뒤로가기버튼이 생성되지 않는다.
	// 따라서 증강현실뷰에서 이 플래그를 YES로 하면 뒤로가기 버튼이 생성된다.
	if (self.makeBackButtonForAR) 
	{
		[self makeBackButton];
		
		// -------------------------------------
		//	버그인지 실수인지 몰라도 상태바가 사라진다.
		//	강제로 상태바를 다시 보이게 하는 부분이다.
		// -------------------------------------
		[[UIApplication sharedApplication] setStatusBarHidden:NO animated:NO];		
	}
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
	
	// 네비게이션바의 단골맛집 버튼 생성
	if (self.makeJoinButtonForJoin)
	{
		[self makeJoinButton];
	}
}


- (void)viewDidUnload 
{
	self.restaurant = nil;
	self.restaurantTableView = nil;
	self.dbConnector = nil;
	
    [super viewDidUnload];
}



- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	// 데이터베이스가 연결되어 있다면 삭제
	if (self.dbConnector)
	{
		[self.dbConnector removeAllDelegateReferenceToNil];
		self.dbConnector = nil;
	}
}


#pragma mark -
#pragma mark 맛집뷰 이벤트
// 맛집의 위치를 보여줄 싱글맵뷰 만들기
- (void)makeMapView;
{
	SingleMapViewController *viewController = [[SingleMapViewController alloc] initWithNibName:@"SingleMapViewController" bundle:nil withRestaurant:self.restaurant];
	[self.navigationController pushViewController:viewController animated:YES];
	[viewController release];
}


// 맛집의 증강현실 위치를 보여줄 증강현실뷰 만들기
- (void)makeARView;
{
	ARViewController *viewController = [[ARViewController alloc] initWithUsingSingleAR:YES];
	[viewController addARRestaurantViewWithRestaurant:self.restaurant];
	viewController.delegate = self;
	[self.navigationController pushViewController:viewController animated:NO];
	[viewController release];
}


// 뒤로가기버튼 만들기
- (void)makeBackButton
{
	UIBarButtonItem *button = [[UIBarButtonItem alloc] initWithTitle:@"뒤로가기" style:UIBarButtonItemStyleBordered target:self action:@selector(touchBackButton:)];
	self.navigationItem.leftBarButtonItem = button;
	[button release];
}


// 뒤로가기버튼 이벤트
- (void)touchBackButton:(id)sender
{
	if (self.modalViewController)
	{
		if ([self.modalViewController isKindOfClass:[ARViewController class]]) 
		{
			[(ARViewController*)self.parentViewController startLocationManagerUpdating];
		}
	}
	
	[self dismissModalViewControllerAnimated:YES];
}


// 단골맛집 버튼만들기
- (void)makeJoinButton
{
	UIBarButtonItem *button = [[UIBarButtonItem alloc] initWithTitle:@"단골맛집 추가" style:UIBarButtonItemStyleBordered target:self action:@selector(touchJoinButton:)];
	self.navigationItem.rightBarButtonItem = button;
	[button release];
}


// 단골맛집 버튼의 이벤트
- (void)touchJoinButton:(id)sender
{
	[self makeJoinActionSheet];
}


// 단골맛집 추가하기
- (void)addEnjoyedRestaurant
{
	// sqlite 커넥터 생성
	SQLiteDatabaseConnector *connector = [[SQLiteDatabaseConnector alloc] init];
	
	if ([connector.sqliteDB isGoodConnection])
	{
		ADD_RETURN_TYPE resultType = [connector addRestaurantToSQliteDB:self.restaurant];
		[self makeJoinResultActionSheet:resultType];
	}
	
	[connector release];
}


#pragma mark -
#pragma mark 증강현실뷰 델리게이트 메소드
- (void)ARViewControllerDidFinish:(ARViewController *)controller
{
	[self.navigationController popViewControllerAnimated:YES];
}

#pragma mark -
#pragma mark 테이블뷰 델리게이트 메소드
// 섹션의 갯수
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return TABLE_SECTION_COUNT;
}


// 섹션별 row의 갯수
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	NSInteger rows = 0;
	
	switch (section)
	{
		case TABLE_SECTION_ADDRESS:
		{
			rows = ADDRESS_SECTION_COUNT;
		}	break;
						
		case TABLE_SECTION_IMAGE_AND_REVIEW:
		{
			rows = 1;
		}	break;
			
		case TABLE_SECTION_RECOMMENDATION_MENU:
		{
			if (self.restaurant.recommendationMenu)
			{
				// 추천메뉴 갯수 + 타이틀
				rows = [self.restaurant.recommendationMenu count] + 1;
			}
			else
			{
				// 추천메뉴가 없음.
				rows = RECOMMENDATION_MENU_COUNT;
			}
		}	break;
			
		case TABLE_SECTION_INFO:
		{
			rows = INFO_SECTION_COUNT;
		}	break;
			
		case TABLE_SECTION_ETC:
		{
			rows = ETC_SECTION_COUNT;
		}	break;
			
//		case TABLE_SECTION_USER_REVIEW:
//		{
//			rows = USER_REVIEW_COUNT;
//		}	break;			
	}
	
    return rows;
}


// 각 섹션의 타이틀
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	NSString *sectionName = nil;
	
	switch (section)
	{
		case TABLE_SECTION_ADDRESS:
		{
			sectionName = TABLE_SECTION_NAME_ADDRESS;
		}	break;
			
		case TABLE_SECTION_RECOMMENDATION_MENU:
		{
			sectionName = TABLE_SECTION_NAME_RECOMMENDATION_MENU;
		}	break;
			
		case TABLE_SECTION_IMAGE_AND_REVIEW:
		{
			sectionName = TABLE_SECTION_NAME_IMAGE_REVIEW;
		}	break;
			
		case TABLE_SECTION_INFO:
		{
			sectionName = TABLE_SECTION_NAME_INFO;
		}	break;
			
		case TABLE_SECTION_ETC:
		{
			sectionName = TABLE_SECTION_NAME_ETC;
		}	break;			
			
//		case TABLE_SECTION_USER_REVIEW:
//		{
//			sectionName = TABLE_SECTION_NAME_USER_REVIEW;
//		}	break;						
	}
	
	return sectionName;
}


// 각 섹션별 row의 높이
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	CGFloat height = 44;
	
	switch (indexPath.section)
	{
		// 주소정보는 오직 문자열 데이터이므로 높이가 InfoCell크기와 동일함.
		case TABLE_SECTION_ADDRESS:
		{
			switch (indexPath.row)
			{
				case ADDRESS_SECTION_ADDRESS_FEILD:
				{
					height = kInfoCellHeight;
				}	break;
					
				case ADDRESS_SECTION_PHONE_FIELD:
				{
					height = kInfoCellHeight;
				}	break;					
			}
		}	break;
		
			
		// 이미지와 리뷰섹션은 하나의 셀만 이용한다. ImageReviewCell의 크기
		case TABLE_SECTION_IMAGE_AND_REVIEW:
		{
			height = kImageReviewCellHeight;
		}	break;

		// 추천메뉴는 오직 문자열 데이터이므로 높이가 InfoCell크기와 동일함.
		case TABLE_SECTION_RECOMMENDATION_MENU:
		{
			height = kInfoCellHeight;
		}	break;
		
		// InfoCell크기와 동일함.
		case TABLE_SECTION_INFO:
		{
			height = kInfoCellHeight;
		}	break;
		
		// InfoCell크기와 동일함.
		case TABLE_SECTION_ETC:
		{
			height = 70;
		}	break;
			
//		// InfoCell크기와 동일함.
//		case TABLE_SECTION_USER_REVIEW:
//		{
//			height = kInfoCellHeight;
//		}	break;			
	}

	return height;
}

// 셀 구현
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
	// --------------------------------
	//	폰트 만들기 - 폰트는 공통으로 쓰인다.
	// --------------------------------
	UIFont *font = [UIFont fontWithName:JBFOOD_FONT_NAME size:20];
	
	
	// --------------------------------------------------
	//	섹션으로 대분류를 한후,
	//	다시 row로 소분류를 하여 각 해당하는 셀과 데이터를 만든다.
	// --------------------------------------------------
	switch (indexPath.section)
	{
		// ------------------------------------------------------------------------------------------
		//										주소 섹션 셀 만들기
		// ------------------------------------------------------------------------------------------
		case TABLE_SECTION_ADDRESS:
		{
			// 식별자 생성
			static NSString *CellIdentifier = @"BasicCellIdentifierForAddress";
			
			// 셀 가져오기
			UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (cell == nil)
			{
				cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
				
				// 배경 설정
				cell.backgroundColor = [UIColor clearColor];
				
				// 폰트 설정
				cell.textLabel.font = font;
				
				
				// 데이터 넣기
				switch (indexPath.row) 
				{
					case ADDRESS_SECTION_ADDRESS_FEILD:
					{
						// 악세사리 이미지 설정
						cell.accessoryView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"map.png"]];
						[cell.accessoryView release];
						cell.accessoryView.bounds = CGRectMake(0, 0, 35, 35);
						
						// 주소 설정.
						cell.textLabel.text = self.restaurant.address;
						
						// 데이터가 길면 2줄에 표시
						cell.textLabel.numberOfLines = 2;
					}	break;
						
					case ADDRESS_SECTION_PHONE_FIELD:
					{
						// 악세사리 이미지 설정
						cell.accessoryView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"phone.png"]];
						[cell.accessoryView release];
						cell.accessoryView.bounds = CGRectMake(0, 0, 35, 35);
						
						// 전화번호 설정
						cell.textLabel.text = self.restaurant.phone;
					}	break;
				}
			}
						
			// 생성된 셀 리턴
			return cell;
			
		}	break;

			
		// ------------------------------------------------------------------------------------------
		//									이미지와 리뷰 섹션 셀 만들기
		// ------------------------------------------------------------------------------------------
		case TABLE_SECTION_IMAGE_AND_REVIEW:
		{
			// 식별자 생성
			static NSString *CellIdentifier = ImageReviewCellIdentifier;
			
			// 셀 가져오기
			ImageReviewCell *cell = (ImageReviewCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (cell == nil)
			{
				// Xib파일로부터 셀을 생성한다.
				NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"ImageReviewCell" owner:self options:nil];
				cell = (ImageReviewCell*)[nib objectAtIndex:0];
				
				// 폰트 설정
				cell.reviewTextView.font = font;
				
				// 데이터 넣기
				cell.reviewTextView.text = self.restaurant.intro;
				
				// 맛집 이미지가 다운로드가 가능한지 판단한다.
				// 이미지 설정 - 우선 빈 이미지를 넣는다.				
				cell.restaurantImageView.image = [UIImage imageNamed:JBFOOD_BLANK_NORMAL_IMG_FILENAME];
				if ( [restaurant isPossibleDownloadImage] )
				{		
					switch (restaurant.imageDownloadState)
					{
						case IMG_DOWNLOAD_READY:
						{
							[self.restaurant startDownloadingForImageView:cell.restaurantImageView];
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
			}
			
			
			// 생성된 셀 리턴
			return cell;
			
		}	break;

			
		// ------------------------------------------------------------------------------------------
		//										추천메뉴 섹션 셀 만들기
		// ------------------------------------------------------------------------------------------
		case TABLE_SECTION_RECOMMENDATION_MENU:
		{
			
			static NSString *CellIdentifier = @"MenuCellIdentifier";
			
			// 셀 가져오기
			UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (cell == nil)
			{
				cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
				
				// 배경 설정
				cell.backgroundColor = [UIColor clearColor];
				
				// 폰트 설정
				cell.textLabel.font = font;
				
				// 정렬하기
				cell.textLabel.textAlignment = UITextAlignmentCenter;
			}
			
			// 타이틀 데이터 넣기
			if (indexPath.row == RECOMMENDATION_MENU_TITLE)
			{
				cell.textLabel.text = RECOMMENDATION_MENU_NAME_TITLE;
			}
			// 실제 추천메뉴 데이터 넣기
			else
			{
				// 추천메뉴가 존재한다면
				if (self.restaurant.recommendationMenu)
				{
					// 인덱스는 타이틀이 존재하기 때문에 indexPath.row - 1을 한다.
					RecommendationMenu *menu = [self.restaurant.recommendationMenu objectAtIndex:(indexPath.row-1)];
					cell.textLabel.text = menu.menu;
					
					// 데이터가 길면 2줄에 표시
					cell.textLabel.numberOfLines = 2;
				}
			}
			
			
//			// 식별자 생성
//			static NSString *CellIdentifier = InfoCellIdentifier;
//		
//			// 셀 가져오기
//			InfoCell *cell = (InfoCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
//			
//			// 셀이 존재하지 않는다면 새로 생성
//			if (cell == nil)
//			{
//				// Xib파일로부터 셀을 생성한다.
//				NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"InfoCell" owner:self options:nil];
//				cell = (InfoCell*)[nib objectAtIndex:0];
//				
//				// 폰트 설정
//				cell.label_title.font = font;
//				cell.label_contents.font = font;				
//			}
//			
//			// 타이틀 데이터 넣기
//			if (indexPath.row == RECOMMENDATION_MENU_TITLE)
//			{
//				cell.label_title.text = RECOMMENDATION_MENU_NAME_TITLE;
//				cell.label_contents.text = RECOMMENDATION_MENU_NAME_PRICE;
//			}
//			// 실제 추천메뉴 데이터 넣기
//			else
//			{
//				// 추천메뉴가 존재한다면
//				if (self.restaurant.recommendationMenu)
//				{
//					// 인덱스는 타이틀이 존재하기 때문에 indexPath.row - 1을 한다.
//					RecommendationMenu *menu = [self.restaurant.recommendationMenu objectAtIndex:(indexPath.row-1)];
//					
//					cell.label_title.text = menu.menu;
//					cell.label_contents.text = menu.price;
//				}
//			}
			
						
			// 생성된 셀 리턴
			return cell;
			
		}	break;
			
			
			
		// ------------------------------------------------------------------------------------------
		//										정보 섹션 셀 만들기
		// ------------------------------------------------------------------------------------------
		case TABLE_SECTION_INFO:
		{
			// 식별자 생성
			static NSString *CellIdentifier = InfoCellIdentifier;
			
			// 셀 가져오기
			InfoCell *cell = (InfoCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (cell == nil)
			{
				// Xib파일로부터 셀을 생성한다.
				NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"InfoCell" owner:self options:nil];
				cell = (InfoCell*)[nib objectAtIndex:0];
				
				// 폰트 설정
				cell.label_title.font = font;
				cell.label_contents.font = font;
				
				// 데이터 넣기
				switch (indexPath.row)
				{
					case INFO_SECTION_TIME:
					{
						cell.label_title.text = INFO_SECTION_NAME_TIME;
						cell.label_contents.text = self.restaurant.time;
					}	break;
						
					case INFO_SECTION_ACCOMODATION:
					{
						cell.label_title.text = INFO_SECTION_NAME_ACCOMODATION;
						cell.label_contents.text = self.restaurant.accomodation;
					}	break;
						
					case INFO_SECTION_CAR_SUPPORT:
					{
						cell.label_title.text = INFO_SECTION_NAME_CAR_SUPPORT;
						cell.label_contents.text = self.restaurant.car_support;
					}	break;
						
					case INFO_SECTION_RESERVATION:
					{
						cell.label_title.text = INFO_SECTION_NAME_RESERVATION;
						cell.label_contents.text = self.restaurant.reservation;
					}	break;
						
					case INFO_SECTION_HOLYDAY:
					{
						cell.label_title.text = INFO_SECTION_NAME_HOLYDAY;
						cell.label_contents.text = self.restaurant.holyday;
					}	break;					
						
					case INFO_SECTION_PARKING:
					{
						cell.label_title.text = INFO_SECTION_NAME_PARKING;
						cell.label_contents.text = self.restaurant.park;
					}	break;					
				}
			}
									
			// 생성된 셀 리턴
			return cell;			
			
		}	break;
			
			
		// ------------------------------------------------------------------------------------------
		//									기타정보 섹션 셀 만들기
		// ------------------------------------------------------------------------------------------
		case TABLE_SECTION_ETC:
		{
			// 식별자 생성
			static NSString *CellIdentifier = @"BasicCellIdentifierForInfo";
			
			// 셀 가져오기
			UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (cell == nil)
			{
				cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
				
				// 배경 설정
				cell.backgroundColor = [UIColor clearColor];
				
				// 폰트 설정
				cell.textLabel.font = font;
				
				// 라인 설정
				cell.textLabel.numberOfLines = 3;
				
				// 악세서리 없음
				cell.accessoryType = UITableViewCellAccessoryNone;
				
				// 셀 선택시에 애니메이션효과(선택효과) 없음.
				cell.selectionStyle = UITableViewCellSelectionStyleNone;
				
				// 데이터 넣기
				switch (indexPath.row) 
				{
					case ETC_SECTION_1:
					{
						cell.textLabel.text = self.restaurant.etc1;
					}	break;
						
					case ETC_SECTION_2:
					{
						cell.textLabel.text = self.restaurant.etc2;
					}	break;
				}
			}
			
			
			
			// 생성된 셀 리턴
			return cell;			
			
		}	break;
			
			
			
//		// ------------------------------------------------------------------------------------------
//		//									맛집 평가 섹션 셀 만들기
//		// ------------------------------------------------------------------------------------------
//		case TABLE_SECTION_USER_REVIEW:
//		{
//			// 식별자 생성
//			static NSString *CellIdentifier = @"BasicCellIdentifierForUserReview";
//			
//			// 셀 가져오기
//			UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
//			
//			// 셀이 존재하지 않는다면 새로 생성
//			if (cell == nil)
//			{
//				cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
//				
//				// 배경 설정
//				cell.backgroundColor = [UIColor clearColor];
//				
//				// 폰트 설정
//				cell.textLabel.font = font;
//			}
//			
//			switch (indexPath.row)
//			{
//				case USER_REVIEW_STAR:
//				{
//					// 악세서리 없음
//					cell.accessoryType = UITableViewCellAccessoryNone;
//					
//					if (self.restaurant.starCount > 0)
//					{
//						// 텍스트 설정
//						cell.textLabel.text = self.restaurant.starString;
//						
//						// 텍스트 컬러
//						cell.textLabel.textColor = [UIColor colorWithRed:0.67 green:0.141 blue:0.0745 alpha:1];						
//					}
//					else
//					{
//						// 텍스트 설정
//						cell.textLabel.text = @"아직 별점이 없습니다.";
//						
//						// 텍스트 컬러
//						cell.textLabel.textColor = [UIColor blackColor];
//					}
//
//										
//					// 셀 선택시에 애니메이션효과(선택효과) 없음.
//					cell.selectionStyle = UITableViewCellSelectionStyleNone;
//					
//				}	break;
//					
//				case USER_REVIEW_READ:
//				{
//					// 텍스트 컬러
//					cell.textLabel.textColor = [UIColor blackColor];
//
//					if ([self.restaurant.reviewArray count] > 0)
//					{
//						// 텍스트 설정
//						cell.textLabel.text = [NSString stringWithFormat:@"%@의 한줄평 보기", self.restaurant.name];
//						
//						// 악세서리 설정.
//						cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
//
//					}
//					else
//					{
//						// 텍스트 설정
//						cell.textLabel.text = @"아직 한줄평이 없습니다.";
//						
//						// 악세서리 없음
//						cell.accessoryType = UITableViewCellAccessoryNone;
//						
//						// 셀 선택시에 애니메이션효과(선택효과) 없음.
//						cell.selectionStyle = UITableViewCellSelectionStyleNone;
//					}
//
//				}	break;
//
//					
//				case USER_REVIEW_WRITE:
//				{
//					// 텍스트 컬러
//					cell.textLabel.textColor = [UIColor blackColor];
//					
//					// 악세서리 설정
//					cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
//					
//					// 텍스트 설정
//					cell.textLabel.text = @"한줄평 쓰기";					
//					
//				}	break;					
//			}
//			
//			// 생성된 셀 리턴
//			return cell;			
//			
//		}	break;	
	}
	
	
	// -----------------------------------------------------------------------------------------------
	//							위의 switch-case에 걸리지 않는다면 기본으로 생성한다.
	// -----------------------------------------------------------------------------------------------
	static NSString *CellIdentifier = @"CellIdentifier";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	if (cell == nil)
	{
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
	}
	// 배경 설정
	//cell.backgroundColor = [UIColor clearColor];
	
    return cell;
}


// 셀을 선택한 경우 발생되는 델리게이트 메소드
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	switch (indexPath.section)
	{
		case TABLE_SECTION_ADDRESS:
		{
			switch (indexPath.row)
			{
				case ADDRESS_SECTION_ADDRESS_FEILD:
				{
					[self makeGeoActionSheet];
				}	break;
					
				case ADDRESS_SECTION_PHONE_FIELD:
				{
					[self makePhoneCallActionSheet];
				}	break;
			}
	
		}	break;
			
		case TABLE_SECTION_RECOMMENDATION_MENU:
		{
	
		}	break;
			
		case TABLE_SECTION_IMAGE_AND_REVIEW:
		{
	
		}	break;
			
		case TABLE_SECTION_INFO:
		{
	
		}	break;
			
		case TABLE_SECTION_ETC:
		{
	
		}	break;
			
//		case TABLE_SECTION_USER_REVIEW:
//		{
//			switch (indexPath.row)
//			{
//				case USER_REVIEW_STAR:
//				{
//					
//				}	break;
//					
//				case USER_REVIEW_READ:
//				{
//					// 한줄평이 1개이상 존재할때만 한줄평 뷰 컨트롤러를 생성하여 보여줌.
//					if (self.restaurant.starCount > 0)
//					{
//						// 한줄평 보기 뷰 컨트롤러 생성
//						UserReviewResultTableViewController *viewController = [[UserReviewResultTableViewController alloc] iniWithArrayUserReviews:self.restaurant.reviewArray];
//						[self.navigationController pushViewController:viewController animated:YES];
//						[viewController release];						
//					}
//					
//				}	break;
//
//				case USER_REVIEW_WRITE:
//				{
//					// 리뷰 쓰기 뷰 컨트롤러 생성
//					UserReviewViewController *viewController = [[UserReviewViewController alloc] initWithNibName:@"UserReviewViewController" bundle:nil withRestaurantId:self.restaurant.id];
//					[self.navigationController pushViewController:viewController animated:YES];
//					viewController.userReviewWriteDelegate = self;
//					[viewController release];
//					
//				}	break;					
//			}
//			
//		}	break;
	}
}


#pragma mark -
#pragma mark 액션시트 만들기
// 지도보기, 증강현실보기 액션시트 만들기
- (void)makeGeoActionSheet
{
	// 타이틀 생성
	NSString *title = [NSString stringWithFormat:@"%@\n%@\n", restaurant.name, restaurant.address];
	
	// 액션시트 생성
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:title 
															 delegate:self 
													cancelButtonTitle:@"취소" destructiveButtonTitle:nil otherButtonTitles:@"지도보기", @"증강현실보기", nil
								  ];
	// 기본스타일 액션시트
	actionSheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
	
	// 태그에 액션시트 타입 값 설정.
	actionSheet.tag = ACTIONSHEET_TAG_GEO;
	
	// 액션시트 보이기 - 탭바가 존재한다면 탭바에서부터 보이도록 한다.
	if (self.tabBarController)
	{
		[actionSheet showFromTabBar:self.tabBarController.tabBar];
	}
	else
	{
		[actionSheet showInView:self.view];
	}
	
	
	// 릴리즈
	[actionSheet release];
}

// 전화걸기 액션시트 만들기
- (void)makePhoneCallActionSheet
{
	// 타이틀 생성
	NSString *title = [NSString stringWithFormat:@"%@\n%@\n연결하시겠습니까?", restaurant.name, restaurant.phone];
	
	// 액션시트 생성
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:title 
															 delegate:self 
													cancelButtonTitle:@"아니오" destructiveButtonTitle:@"예" otherButtonTitles:nil
								  ];
	// 기본스타일 액션시트
	actionSheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
	
	// 태그에 액션시트 타입 값 설정.
	actionSheet.tag = ACTIONSHEET_TAG_CALL;
	
	// 액션시트 보이기 - 탭바가 존재한다면 탭바에서부터 보이도록 한다.
	if (self.tabBarController)
	{
		[actionSheet showFromTabBar:self.tabBarController.tabBar];
	}
	else
	{
		[actionSheet showInView:self.view];
	}

	// 릴리즈
	[actionSheet release];	
}


//  단골맛집 액션시트 만들기
- (void)makeJoinActionSheet
{
	// 타이틀 생성
	NSString *title = [NSString stringWithFormat:@"%@\n단골맛집으로 추가하시겠습니까?", restaurant.name, restaurant.phone];
	
	// 액션시트 생성
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:title 
															 delegate:self 
													cancelButtonTitle:@"아니오" destructiveButtonTitle:@"예" otherButtonTitles:nil
								  ];
	// 기본스타일 액션시트
	actionSheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
	
	// 태그에 액션시트 타입 값 설정.
	actionSheet.tag = ACTIONSHEET_TAG_JOIN;
	
	// 액션시트 보이기 - 탭바가 존재한다면 탭바에서부터 보이도록 한다.
	if (self.tabBarController)
	{
		[actionSheet showFromTabBar:self.tabBarController.tabBar];
	}
	else
	{
		[actionSheet showInView:self.view];
	}
	
	// 릴리즈
	[actionSheet release];
}


// 단골맛집 추가한 결과 액션시트 만들기
- (void)makeJoinResultActionSheet:(ADD_RETURN_TYPE)result
{
	// 타이틀 생성
	NSString *title;
	
	switch (result)
	{
		case ADD_RETURN_COMPLETE:
		{
			title = @"단골맛집으로 추가 되었습니다.";
		}	break;
			
		case ADD_RETURN_FAILED:
		{
			title = @"단골맛집으로 추가하는데 실패하였습니다.";
		}	break;
			
		case ADD_RETURN_ALREADY:
		{
			title = @"이미 단골맛집으로 등록되어 있습니다.";
		}	break;			
	}
		
	// 액션시트 생성
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:title 
								delegate:nil cancelButtonTitle:@"확인" destructiveButtonTitle:nil otherButtonTitles:nil
								  ];
	// 기본스타일 액션시트
	actionSheet.actionSheetStyle = UIActionSheetStyleBlackOpaque;
	
	// 태그에 액션시트 타입 값 설정.
	actionSheet.tag = ACTIONSHEET_TAG_JOIN_RESULT;
	
	// 액션시트 보이기 - 탭바가 존재한다면 탭바에서부터 보이도록 한다.
	if (self.tabBarController)
	{
		[actionSheet showFromTabBar:self.tabBarController.tabBar];
	}
	else
	{
		[actionSheet showInView:self.view];
	}
	
	// 릴리즈
	[actionSheet release];
}


#pragma mark -
#pragma mark 액션시트 델리게이트 메소드
// 액션시트 델리게이트 메소드
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	switch (actionSheet.tag)
	{
			// 전화걸기
		case ACTIONSHEET_TAG_CALL:
		{
			switch (buttonIndex)
			{
					// 전화걸기
				case 0:
				{
					NSString *phoneCall = [NSString stringWithFormat:@"tel:%@", restaurant.phone];
					[[UIApplication sharedApplication] openURL:[NSURL URLWithString:phoneCall]];
				}	break;
					
					// 취소하기
				case 1:
				{
				}	break;
			}
		} break;
			
		case ACTIONSHEET_TAG_GEO:
		{
			switch (buttonIndex)
			{
					// 지도보기
				case 0:
				{
					[self makeMapView];
				}	break;
					
					// 증강현실 보기
				case 1:
				{
					[self makeARView];
				}	break;
					
					// 취소하기
				case 2:
				{
				}	break;
			}
			
		} break;
			
		case ACTIONSHEET_TAG_JOIN:
		{
			switch (buttonIndex)
			{
					// 단골맛집 추가하기
				case 0:
				{
					[self addEnjoyedRestaurant];
				}	break;
					
					// 취소하기
				case 1:
				{
					//NSLog(@"단골맛집 취소");
				}	break;
					
			}
			
		} break;
	}
}


#pragma mark -
#pragma mark 알람뷰 만들기
- (void)makeAlertView:(NSString*)title
{
	UIAlertView *alertView = [[UIAlertView alloc]
							  initWithTitle:@"알림" 
							  message:title
							  delegate:nil
							  cancelButtonTitle:@"확인"
							  otherButtonTitles:nil
							  ];
	[alertView show];
	[alertView release];
}


#pragma mark -
#pragma mark 한줄평 리뷰 쓰기 성공 델리게이트 메소드
- (void) userReviewWriteDidSuccess
{
	// 액티비티 인티케이터 뷰 애니메이터 시작
	[self.indicatorView startAnimating];
	
	// 데이터베이스 커넥터 생성
	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = connector;
	[connector release];
	self.dbConnector.restaurantFromIdDelegate = self;
	
	// 데이터 연결 시작
	[self.dbConnector startReceivingRestaurantFromId:self.restaurant.id];
}


#pragma mark -
#pragma mark 데이터베이스 연결 델리게이트
- (void)restaurantDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{
	
}


- (void)restaurantDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{
	// 데이터베이스 커넥터 종료
	connector.restaurantFromIdDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 알람 뷰 생성
	[self makeAlertView:@"맛집데이터 새로고침 실패"];
}


- (void)restaurantDidFinish:(DatabaseConnector *)connector withRestaurant:(Restaurant *)newRestaurant
{
	// 데이터베이스 커넥터 종료
	connector.restaurantFromIdDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 검색결과가 존재한다면
	if (newRestaurant)
	{
		// 기존의 맛집데이터 릴리즈
		//[self.restaurant release];
		
		// 새로운 맛집데이터로 시작. 한줄평이 업데이트되있기 때문.
		self.restaurant = newRestaurant;
		
		// 테이블 새로 고침
		[self.restaurantTableView reloadData];
	}	
	
}

@end
