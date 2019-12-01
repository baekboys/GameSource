#import "SearchByAreaTableViewController.h"
#import "SearchByAreaResultTableViewController.h"	// 데이터베이스를 통하여 찾은 지역별 맛집을 그릴 테이블 클래스
#import "SearchByAreaTableCell.h"
#import "MapLocation.h"
#import "Restaurant.h"

@implementation SearchByAreaTableViewController

#pragma mark -
#pragma mark synthesize
@synthesize areaTableView;
@synthesize alertViewForDB;
@synthesize indicatorView;
@synthesize areaMapView;
@synthesize areaName;
@synthesize dbConnector;
@synthesize areaDictionary;
@synthesize areaArray;
@synthesize areaAnnotaionArray;


#pragma mark -
#pragma mark 초기화 메소드
- (id)init
{
	if (self = [super init]) 
	{
		self.title = @"지역별 맛집";
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
	
	
	// 테이블 뷰 생성 및 설정
//	UITableView *tableViewForArea = [[UITableView alloc] initWithFrame:rect style:UITableViewStylePlain];
//	self.areaTableView = tableViewForArea;
//	[tableViewForArea release];
//	self.areaTableView.delegate = self;
//	self.areaTableView.dataSource = self;
//	self.areaTableView.backgroundColor = [UIColor clearColor];
//	[self.view addSubview:self.areaTableView];
	
	
	// ------------------------------------------------------------------
	//
	//							지역별 분류 딕셔너리 생성
	//
	//			*	메인번들의 plist파일로 부터 생성된다.
	//			*	딕셔너리가 성공적으로 생성된경우에만 맵뷰를 생성한다.
	//
	// ------------------------------------------------------------------
	
	// 메인번들로부터 패스 가져오기
	NSBundle *bundle = [NSBundle mainBundle];
	NSString *pListPath = [bundle pathForResource:kJeonbukAreaPlistFileName ofType:@"plist"];
	
	// 딕셔너리 생성
	NSDictionary *dictionnary = [[NSDictionary alloc] initWithContentsOfFile:pListPath];
	
	// 생성이 되었다면
	if (dictionnary)
	{
		// 지역별 딕셔너리 설정
		self.areaDictionary = dictionnary;
		[dictionnary release];

		// 지역별 이름 array 설정
		NSArray *areaListArray = [self.areaDictionary allKeys];
		self.areaArray = areaListArray;
		
		// 지역별 annotation을 저장할 array생성
		NSMutableArray *arrayAnnotation = [[NSMutableArray alloc] init];
		self.areaAnnotaionArray = arrayAnnotation;
		[arrayAnnotation release];
		
		
		// 맵뷰 생성
		MKMapView *mapView = [[MKMapView alloc] initWithFrame:rect];
		self.areaMapView = mapView;
//		self.areaMapView.showsUserLocation = YES;
		[mapView release];
		self.areaMapView.delegate = self;
//		self.areaMapView.mapType = MKMapTypeHybrid;
		[self.view addSubview:self.areaMapView];
		
		
		// 맵뷰 초기화
		CLLocationCoordinate2D coord;
		coord.latitude = kJeonbukLatitude;
		coord.longitude = kJeonbukLongitude;	
		MKCoordinateRegion viewRegion = MKCoordinateRegionMakeWithDistance(coord, kJeonbukInitSpan, kJeonbukInitSpan); 
		MKCoordinateRegion adjustedRegion = [mapView regionThatFits:viewRegion];
		[self.areaMapView setRegion:adjustedRegion animated:NO];

		
		
		// ------------------------------------------
		//	어노티에션을 담고 있을 array가 필요하다. !!!!!
		//	만약 array가 없으면 맵뷰에 보이지 않는다.
		// ------------------------------------------
		
		// 지역별이름 어노테이션 생성하기
		NSUInteger i;
		for (i = 0 ; i < [self.areaArray count] ; ++i)
		{
			//	SearchByAreaTypeList.plist의 구조
			//	0번 위도, 1번 경도, 2번 이미지파일명
			
			NSArray *areaDataArray = (NSArray*)[self.areaDictionary objectForKey:[self.areaArray objectAtIndex:i]];
			//id object = (NSArray*)[self.areaDictionary objectForKey:[self.areaArray objectAtIndex:i]];
			CLLocationCoordinate2D coord;
			
			// 위도
			coord.latitude = [[areaDataArray objectAtIndex:0] doubleValue];
			// 경도
			coord.longitude = [[areaDataArray objectAtIndex:1] doubleValue];
			
			// 이미지 파일명
			NSString *fileName = [areaDataArray objectAtIndex:2];
			
			// 맵뷰에 추가하기
			[self addMapLocationWithArea:i withCoord:coord withImageFileName:fileName];		
		}	
		
		
		// 맵뷰에 어노테이션 추가하기
		[self.areaMapView addAnnotations:self.areaAnnotaionArray];
		
		// 액티비티 인티케이터 뷰 생성
		UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
		self.indicatorView = aiView;
		[aiView release];
		self.indicatorView.frame = kActivityIndicatorViewFrameSizeOnNavi;
		self.indicatorView.hidesWhenStopped = YES;
		self.indicatorView.hidden = YES;
		[self.navigationController.navigationBar addSubview:self.indicatorView];		
	}	
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];	
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

- (void)viewDidLoad 
{
    [super viewDidLoad];
}


- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.areaTableView = nil;
	self.alertViewForDB = nil;
	self.indicatorView = nil;
	self.areaName = nil;
	self.dbConnector = nil;
	self.areaMapView = nil;
	self.areaDictionary = nil;
	self.areaArray = nil;
	self.areaAnnotaionArray = nil;
}


- (void)dealloc 
{
	self.areaTableView = nil;
	self.alertViewForDB = nil;
	self.indicatorView = nil;
	self.areaName = nil;
	self.dbConnector = nil;
	self.areaMapView = nil;
	self.areaDictionary = nil;
	self.areaArray = nil;
	self.areaAnnotaionArray = nil;
	
    [super dealloc];
}


#pragma mark -
#pragma mark 테이블뷰 델리게이트 메소드
// 섹션의 갯수
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 1;
}


// 섹션별 row의 갯수
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    return AREA_TOTAL_COUNT;
}


// 셀 구현
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{	
	// 식별자 생성
	static NSString *CellIdentifier = @"SearchByAreaCellIdentifier";
	
	// 셀 가져오기
	SearchByAreaTableCell *cell = (SearchByAreaTableCell*)[tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	
	// 셀이 존재하지 않는다면 새로 생성
	if (cell == nil)
	{
		// Xib파일로부터 셀을 생성한다.
		NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"SearchByAreaTableCell" owner:self options:nil];
		cell = (SearchByAreaTableCell*)[nib objectAtIndex:0];

		// 셀의 폰트 설정
		cell.nameLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:kSearchByAreaTableViewFontSize];
	}
	
	// 셀의 레이블과 폰트 설정.
	cell.nameLabel.text = [self getAreaNameFromAREALIST:indexPath.row];
	
    return cell;
}


// 셀의 높이값 리턴
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	return kSearchByAreaTableCellHeight;
}


// 셀을 선택한 경우 발생되는 델리게이트 메소드
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	// 지역 이름 가져오기
	self.areaName = [self getAreaNameFromAREALIST:indexPath.row];
	
	if (!self.dbConnector) 
	{
		// 액티비티 인디케이터 뷰 애니메이션 시작
		[self.indicatorView startAnimating];
		
		
		// 데이터베이스 커넥터 생성
		[self.dbConnector removeAllDelegateReferenceToNil];
		DatabaseConnector *connnector = [[DatabaseConnector alloc] init];
		self.dbConnector = connnector;
		[connnector release];
		self.dbConnector.arrayRestaurantsFromAreaDelegate = self;
		
		// 데이터받기 시작
		[self.dbConnector startReceivingArrayRestaurantsFromArea:self.areaName];
	}
}

#pragma mark -
#pragma mark 테이블뷰 구현 메소드
// SHARED_DEFINE에 정의된 지역별 맛집 열거자를 넣어 지역이름 얻어오기
- (NSString*)getAreaNameFromAREALIST:(AREA_LIST)list
{
	switch (list) 
	{
		case AREA_GOCHANG:
		{
			return kAREA_GOCHANG;
		} break;
			
		case AREA_GUNSAN:
		{
			return kAREA_GUNSAN;
		} break;
			
		case AREA_KIMJE:
		{
			return kAREA_KIMJE;
		} break;
			
		case AREA_NAMWON:
		{
			return kAREA_NAMWON;
		} break;
			
		case AREA_MUJU:
		{
			return kAREA_MUJU;
		} break;
			
		case AREA_BUAN:
		{
			return kAREA_BUAN;
		} break;
			
		case AREA_SUNCHANG:
		{
			return kAREA_SUNCHANG;
		} break;
			
		case AREA_WANJU:
		{
			return kAREA_WANJU;
		} break;
			
		case AREA_IKSAN:
		{
			return kAREA_IKSAN;
		} break;
			
		case AREA_IMSIL:
		{
			return kAREA_IMSIL;
		} break;
			
		case AREA_JANGSU:
		{
			return kAREA_JANGSU;
		} break;
			
		case AREA_JEONJU:
		{
			return kAREA_JEONJU;
		} break;
			
		case AREA_JEONGEB:
		{
			return kAREA_JEONGEB;
		} break;
			
		case AREA_JINAN:
		{
			return kAREA_JINAN;
		} break;
	}
	
	return nil;
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
	switch (buttonIndex)
	{
		// 취소버튼을 눌렀을 경우
		case 0:
		{
			
		} break;			
	}
}


#pragma mark -
#pragma mark 데이터베이스 연결 델리게이트
- (void)arrayDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{

}


- (void)arrayDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{
	// 데이터베이스 커넥터 종료
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 알람 뷰 생성
	[self makeAlertView:@"연결에 실패하였습니다."];
}


- (void)arrayDidFinish:(DatabaseConnector *)connector withArrayRestaurants:(NSMutableArray *)array
{
	// 데이터베이스 커넥터 종료
	connector.arrayRestaurantsFromAreaDelegate = nil;
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];
	
	// 검색결과가 존재한다면
	if ([array count] > 0)
	{
		// array를 별점순으로 정렬한다.
		[array sortUsingFunction:sortByStarAvarage context:NULL];
		
		// 검색결과를 그려줄 테이블뷰 생성
		SearchByAreaResultTableViewController *viewController = [[SearchByAreaResultTableViewController alloc] initWithArrayRestaurants:array withAreaName:self.areaName];
		
		// 생성한 테이블뷰를 push
		[self.navigationController pushViewController:viewController animated:YES];
		
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
#pragma mark 맵 델리게이트 메소드
- (MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation
{
	// MapLocatoin 클래스라면 맛집데이터를 나타내는 어노테이션이다.
	if ([annotation isKindOfClass:[MapLocation class]])
	{
		// 형변환 시킨다.
		MapLocation *mapLocation = (MapLocation*)annotation;
		
	    static NSString *mapLocationIdentifier = @"MapLocationIdentifierForArea";
		
		// plcaemarkIdentifier값을 통해서 어노테이션 뷰를 가져온다.
		MKAnnotationView *annotationView = (MKAnnotationView *)[mapView dequeueReusableAnnotationViewWithIdentifier:mapLocationIdentifier];
		
		// 어노테이션뷰가 없다면 새로 생성
		if (annotationView == nil)  
		{
			annotationView = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:mapLocationIdentifier] autorelease];
            annotationView.image = [UIImage imageNamed:mapLocation.etc];
			annotationView.canShowCallout = YES;
			
			UIButton* rightButton = [UIButton buttonWithType:UIButtonTypeDetailDisclosure];

			// 이벤트를 처리할 버튼 생성하여 버튼의 태그에 self.areaArray의 인덱스번호를 넣는다. 차후에 이벤트가 발생되면 이태그값을 가지고 self.areaArray의 인덱스번호를 통하여 데이터를 얻어낸다.
			rightButton.tag = mapLocation.id;
			[rightButton addTarget:self action:@selector(touchButtonOfAnnotation:) forControlEvents:UIControlEventTouchUpInside];
			annotationView.rightCalloutAccessoryView = rightButton;
			
            return annotationView;
        }            
        else
		{
            annotationView.annotation = annotation;
		}
        
        return annotationView;
    }
	
	
    return nil;
}


// 맛집데이터로부터 MapLocation 추가하기
- (void)addMapLocationWithArea:(NSInteger)areaIndex withCoord:(CLLocationCoordinate2D)newCoordiate withImageFileName:(NSString*)newFileName
{
	// 현재 유저의 위치어노테이션 생성
	MapLocation *location = [[MapLocation alloc] init];
	
	// 지역 입력 - self.areaArray의 인덱스를 통하여 지역명을 가져온다.
	location.name = (NSString*)[self.areaArray objectAtIndex:areaIndex];
		
	// 설명 입력
	location.address = @"목록을 보려면 오른쪽버튼을 눌러주세요.";
	
	// 위도경도 입력
	location.coordinate = newCoordiate;
	
	// 인덱스번호를 id값에 저장. 차후에 버튼이벤트에 쓰인다.
	location.id = areaIndex;
	
	// 이미지파일명 저장
	location.etc = newFileName;

	// array에 추가
	[self.areaAnnotaionArray addObject:location];
	
	// 맵뷰에 추가
	//[self.areaMapView addAnnotation:location];

	
	// 릴리즈
	[location release];
}


// 지역별 버튼을 눌렀을때의 이벤트 처리
- (void)touchButtonOfAnnotation:(id)sender
{
	UIButton *button = (UIButton*)sender;
	
	// 지역 이름 가져오기
	self.areaName = [self.areaArray objectAtIndex:button.tag];
	
	if (!self.dbConnector) 
	{
		// 액티비티 인디케이터 뷰 애니메이션 시작
		[self.indicatorView startAnimating];
		
		
		// 데이터베이스 커넥터 생성
		[self.dbConnector removeAllDelegateReferenceToNil];
		DatabaseConnector *connnector = [[DatabaseConnector alloc] init];
		self.dbConnector = connnector;
		[connnector release];
		self.dbConnector.arrayRestaurantsFromAreaDelegate = self;
		
		// 데이터받기 시작
		[self.dbConnector startReceivingArrayRestaurantsFromArea:self.areaName];
	}
	
}


#pragma mark -
#pragma mark 별점 정렬 메소드
NSInteger sortByStarAvarage(id restaurant1, id restaurant2, void *context)
{
	NSUInteger v1 = ((Restaurant*)restaurant1).starAverage;
	NSUInteger v2 = ((Restaurant*)restaurant2).starAverage;
	
	if (v1 > v2)
        return NSOrderedAscending;
    else if (v1 < v2)
        return NSOrderedDescending;
    else
        return NSOrderedSame;
}

@end
