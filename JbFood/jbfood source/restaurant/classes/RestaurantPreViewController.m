#import "RestaurantPreViewController.h"
#import "SHARED_DEFINE.h"
#import "PreInfoCell.h"
#import "PreStarCell.h"
#import "PreReviewCell.h"
#import "Restaurant.h"
#import "RecommendationMenu.h"
#import "SingleMapViewController.h"
#import "RestaurantViewController.h"
#import "UserReviewResultTableViewController.h"

@implementation RestaurantPreViewController


@synthesize previewTableView;
@synthesize restaurant;
@synthesize dbConnector;
@synthesize indicatorView;
@synthesize makeBackButtonForAR;
@synthesize makeJoinButtonForJoin;


#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithRestaurant:(Restaurant*)newRestaurant
{
	if (self = [super init])
	{
		self.restaurant = newRestaurant;
		self.title = newRestaurant.name;
		self.dbConnector = nil;
		self.makeBackButtonForAR = NO;
		self.makeJoinButtonForJoin = YES;
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
	self.previewTableView = tableViewForRestaurant;
	[tableViewForRestaurant release];
	self.previewTableView.delegate = self;
	self.previewTableView.dataSource = self;
	self.previewTableView.backgroundColor = [UIColor clearColor];
	self.previewTableView.scrollEnabled = NO;
	//self.previewTableView.separatorColor = [UIColor clearColor];
	//self.previewTableView.separatorStyle = UITableViewCellSeparatorStyleSingleLineEtched;
	
	// 메인뷰에 테이블뷰를 서브뷰로 설정
	[self.view addSubview:self.previewTableView];
	
	
	// 액티비티 인티케이터 뷰 생성
	UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	self.indicatorView = aiView;
	[aiView release];
	self.indicatorView.frame = CGRectMake(0, 0, 50, 50);
	self.indicatorView.center = self.view.center;
	self.indicatorView.hidesWhenStopped = YES;
	self.indicatorView.hidden = YES;
	[self.view addSubview:self.indicatorView];	
}


- (void)viewDidLoad 
{
    [super viewDidLoad];

}


- (void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];	
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
//	if (self.makeJoinButtonForJoin)
//	{
//		[self makeJoinButton];
//	}
	
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


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
    
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


- (void)viewDidUnload 
{
    [super viewDidUnload];
		
	self.previewTableView = nil;
}


- (void)dealloc 
{
	self.previewTableView = nil;
	self.restaurant = nil;
	
    [super dealloc];
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


#pragma mark -
#pragma mark 테이블뷰 구현 메소드
// 섹션의 갯수
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return PREVIEW_SECTION_TOTALCOUNT;
}


// 섹션별 row의 갯수
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	NSInteger count = 0;
	
	switch (section)
	{
		case PREVIEW_SECTION_MAININFO:
		{
			count = MAININFO_SECTION_COUNT;
		}	break;
			
		case PREVIEW_SECTION_BUTTON:
		{
			count = 1;
		}	break;
			
		case PREVIEW_SECTION_USERREVIEW:
		{
			count = 2;
		}	break;
	}
	
	return count;
}


// 셀 구현
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
	UITableViewCell *cell = nil;
	
	switch (indexPath.section)
	{
		// 간단한 정보 섹션
		case PREVIEW_SECTION_MAININFO:
		{
			switch (indexPath.row)
			{
				case MAININFO_SECTION_STAR:
				{
					// 식별자 생성
					static NSString *CellIdentifier = @"PreStarCellIdentifier";
					
					// 셀 가져오기
					PreStarCell *preStarCell = (PreStarCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
					
					// 셀이 존재하지 않는다면 새로 생성
					if (preStarCell == nil)
					{
						// Xib파일로부터 셀을 생성한다.
						NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"PreStarCell" owner:self options:nil];
						preStarCell = (PreStarCell*)[nib objectAtIndex:0];
						
						// 폰트 설정
						preStarCell.starLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preStarCell.starLabel.font pointSize]];
					}
					
					
					if (self.restaurant.starCount > 0)
					{
						// 텍스트 설정
						preStarCell.starLabel.text = self.restaurant.starString;
						
						// 텍스트 컬러
						preStarCell.starLabel.textColor = [UIColor colorWithRed:0.67 green:0.141 blue:0.0745 alpha:1];						
					}
					else
					{
						preStarCell.starLabel.text = @"☆☆☆☆☆";
						preStarCell.starLabel.alpha = 0.2;
					}
					
					
					return preStarCell;						
					
				}	break;
					
				case MAININFO_SECTION_INFO:
				{
					// 식별자 생성
					static NSString *CellIdentifier = @"PreInfoCellIdentifier";
					
					// 셀 가져오기
					PreInfoCell *preInfoCell = (PreInfoCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
					
					// 셀이 존재하지 않는다면 새로 생성
					if (preInfoCell == nil)
					{
						// Xib파일로부터 셀을 생성한다.
						NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"PreInfoCell" owner:self options:nil];
						preInfoCell = (PreInfoCell*)[nib objectAtIndex:0];
						
						// 폰트 설정
						preInfoCell.addressLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preInfoCell.addressLabel.font pointSize]];
						preInfoCell.addressNameLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preInfoCell.addressNameLabel.font pointSize]];
						preInfoCell.phoneLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preInfoCell.phoneLabel.font pointSize]];
						preInfoCell.phoneNameLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preInfoCell.phoneNameLabel.font pointSize]];
						preInfoCell.menuLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preInfoCell.menuLabel.font pointSize]];
						preInfoCell.menuNameLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preInfoCell.menuNameLabel.font pointSize]];
					}
					
					// 주소 설정
					preInfoCell.addressLabel.text = self.restaurant.address;

					// 전화번호 설정
					preInfoCell.phoneLabel.text = self.restaurant.phone;
					
					// 메뉴 설정
					if (self.restaurant.recommendationMenu)
					{
						NSMutableString *menuString = [NSMutableString string];
						
						for (RecommendationMenu *menu in self.restaurant.recommendationMenu)
						{
							if ([restaurant.recommendationMenu lastObject] == menu)
								[menuString appendFormat:@"%@", menu.menu];
							else
								[menuString appendFormat:@"%@, ", menu.menu];
						}
						
						preInfoCell.menuLabel.text = menuString;
					}
					
					return preInfoCell;
					
				}	break;
					
				case MAININFO_SECTION_REVIEW:
				{
					// 식별자 생성
					static NSString *CellIdentifier = @"PreReviewCellIdentifier";
					
					// 셀 가져오기
					PreReviewCell *preReviewCell = (PreReviewCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
					
					// 셀이 존재하지 않는다면 새로 생성
					if (preReviewCell == nil)
					{
						// Xib파일로부터 셀을 생성한다.
						NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"PreReviewCell" owner:self options:nil];
						preReviewCell = (PreReviewCell*)[nib objectAtIndex:0];
						
						// 폰트 설정
						preReviewCell.reviewLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[preReviewCell.reviewLabel.font pointSize]];
					}
					
					preReviewCell.reviewLabel.text = self.restaurant.intro;
					
					return preReviewCell;			
					
				}	break;
			}
			
		}	break;
			
			
		// 전화걸기, 지도보기, 증강현실보기를 위한 버튼 섹션
		case PREVIEW_SECTION_BUTTON:
		{
			// 식별자 생성
			static NSString *CellIdentifier = @"ButtonCellIdentifier";
			
			// 셀 가져오기
			ButtonCell *buttonCell = (ButtonCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (buttonCell == nil)
			{
				// Xib파일로부터 셀을 생성한다.
				NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"ButtonCell" owner:self options:nil];
				buttonCell = (ButtonCell*)[nib objectAtIndex:0];
				
				// 버튼 델리게이트 설정
				buttonCell.buttonDelegate = self;
				
				// 폰트 설정
				buttonCell.callButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:buttonCell.callButton.font.pointSize];
				buttonCell.mapButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:buttonCell.mapButton.font.pointSize];
				buttonCell.arButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:buttonCell.arButton.font.pointSize];
			}
			
						
			return buttonCell;
			
		}	break;
			
		
		// 맛집톡톡 "사용자 한줄평" 섹션
		case PREVIEW_SECTION_USERREVIEW:
		{
			// 식별자 생성
			static NSString *CellIdentifier = @"UserReviewCellIdentifier";
			
			// 셀 가져오기
			UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
			
			// 셀이 존재하지 않는다면 새로 생성
			if (cell == nil)
			{
				cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
				
				// 배경 설정
				cell.backgroundColor = [UIColor clearColor];
				
				// 폰트 설정
				NSUInteger fontSize = 17;
				cell.textLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:fontSize];
				
				// 셀의 텍스트를 중앙으로 정렬
				cell.textLabel.textAlignment = UITextAlignmentCenter;
				
				// 셀의 터치 효과 없음으로 설정
				cell.selectionStyle = UITableViewCellSelectionStyleNone;
			}
			
			switch (indexPath.row)
			{
				case USERREVIEW_SECTION_READ:
				{
					if (self.restaurant.starCount > 0)
					{
						cell.textLabel.text = @"맛집톡톡 보러가기";
						cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
					}
					else
					{
						cell.textLabel.text = @"맛집톡톡이 작성되어 있지 않습니다.";
						cell.accessoryType = UITableViewCellAccessoryNone;
					}
					
				}	break;
					
				case USERREVIEW_SECTION_WRITE:
				{
					cell.textLabel.text = @"맛집톡톡 작성하기";
					cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
				}	break;					
			}
			
			// 생성된 셀 리턴
			return cell;

		}	break;
	}
	
    return cell;
}


// 셀의 높이값 리턴
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	CGFloat height = 44;
	
	switch (indexPath.section)
	{
		case PREVIEW_SECTION_MAININFO:
		{
			switch (indexPath.row) 
			{
				case MAININFO_SECTION_STAR:
				{
					height = kPreStarCellHeight;
				}	break;
					
				case MAININFO_SECTION_INFO:
				{
					height = kPreInfoCellHeight;
				}	break;
					
				case MAININFO_SECTION_REVIEW:
				{
					height = kPreReviewCellHeight;
				}	break;					
			}
			
		}	break;
			
		case PREVIEW_SECTION_BUTTON:
		{
			height = kButtonCellHeight;
		}	break;
			
		case PREVIEW_SECTION_USERREVIEW:
		{
			height = 44;
		}	break;
	}
	
	return height;
}


// 셀을 선택한 경우 발생되는 델리게이트 메소드
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	switch (indexPath.section)
	{
		case PREVIEW_SECTION_MAININFO:
		{
			// 리뷰섹션이나 정보섹션을 터치하면
			if (indexPath.row == MAININFO_SECTION_REVIEW || indexPath.row == MAININFO_SECTION_INFO)
			{
				// 맛집뷰 생성
				RestaurantViewController *viewController = [[RestaurantViewController alloc] initWithRestaurant:self.restaurant];
				
				// 맛집뷰의 단골맛집버튼 생성 플래그 설정.
				viewController.makeJoinButtonForJoin = YES;
				
				
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
			
		}	break;

		case PREVIEW_SECTION_BUTTON:
		{
			
		}	break;

		case PREVIEW_SECTION_USERREVIEW:
		{
			switch (indexPath.row)
			{
				case USERREVIEW_SECTION_READ:
				{
					// 한줄평이 1개이상 존재할때만 한줄평 뷰 컨트롤러를 생성하여 보여줌.
					if (self.restaurant.starCount > 0)
					{
						// 한줄평 보기 뷰 컨트롤러 생성
						UserReviewResultTableViewController *viewController = [[UserReviewResultTableViewController alloc] iniWithArrayUserReviews:self.restaurant.reviewArray];
						[self.navigationController pushViewController:viewController animated:YES];
						[viewController release];						
					}
					
				}	break;

				case USERREVIEW_SECTION_WRITE:
				{
					// 리뷰 쓰기 뷰 컨트롤러 생성
					UserReviewViewController *viewController = [[UserReviewViewController alloc] initWithNibName:@"UserReviewViewController" bundle:nil withRestaurantId:self.restaurant.id];
					[self.navigationController pushViewController:viewController animated:YES];
					viewController.userReviewWriteDelegate = self;
					[viewController release];					
					
				}	break;
			}
			
		}	break;
	}
}

// 버튼이벤트 델리게이트
- (void) buttonDidTouchEvent:(BUTTONCELL_TYPE)button
{
	switch (button)
	{
		case BUTTONCELL_TYPE_CALL:
		{
			// 전화걸기 이벤트
			[self makePhoneCallActionSheet];
		}	break;
			
		case BUTTONCELL_TYPE_MAP:
		{
			// 지도보기 
			SingleMapViewController *viewController = [[SingleMapViewController alloc] initWithNibName:@"SingleMapViewController" bundle:nil withRestaurant:self.restaurant];
			[self.navigationController pushViewController:viewController animated:YES];
			[viewController release];
			
		}	break;
			
		case BUTTONCELL_TYPE_AR:
		{
			// 증강현실 보기
			ARViewController *viewController = [[ARViewController alloc] initWithUsingSingleAR:YES];
			[viewController addARRestaurantViewWithRestaurant:self.restaurant];
			viewController.delegate = self;
			[self.navigationController pushViewController:viewController animated:NO];
			[viewController release];			
		}	break;
			
	}
}


// 전화걸기 액션시트 만들기
- (void)makePhoneCallActionSheet
{
	// 타이틀 생성
	NSString *title = [NSString stringWithFormat:@"%@\n%@\n연결하시겠습니까?", self.restaurant.name, self.restaurant.phone];
	
	// 액션시트 생성
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:title 
															 delegate:self 
													cancelButtonTitle:@"아니오" destructiveButtonTitle:@"예" otherButtonTitles:nil
								  ];
	
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
}


#pragma mark -
#pragma mark 증강현실뷰 델리게이트 메소드
- (void)ARViewControllerDidFinish:(ARViewController *)controller
{
	[self.navigationController popViewControllerAnimated:YES];
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
		[self.previewTableView reloadData];
	}	
	
}

@end
