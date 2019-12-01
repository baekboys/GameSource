#import "SearchBySituationViewController.h"
#import "SearchByAreaResultTableViewController.h"


@implementation SearchBySituationViewController


@synthesize situationPickerView;
@synthesize searchButton;
@synthesize alertViewForDB;
@synthesize indicatorView;
@synthesize dbConnector;
@synthesize situationDictionary;
@synthesize situationKeyArray;
@synthesize selectedDictionary;
@synthesize selectedKeyArray;


#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil 
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) 
	{
		self.title = @"주제별 검색";
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
	
	// 버튼의 폰트 설정
	self.searchButton.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:self.searchButton.font.pointSize];

	
	// ------------------------------------------------------------------
	//
	//							주제별 분류 딕셔너리 생성
	//
	//			*	메인번들의 plist파일로 부터 생성된다.
	//			*	딕셔너리가 성공적으로 생성된경우에만 테이블뷰를 생성한다.
	//
	// ------------------------------------------------------------------
	
	// 메인번들로부터 패스 가져오기
	NSBundle *bundle = [NSBundle mainBundle];
	NSString *plistPath = [bundle pathForResource:kSearchBySituationTypeListFileName ofType:@"plist"];
	
	// 딕셔너리 생성
	NSDictionary *dictionnary = [[NSDictionary alloc] initWithContentsOfFile:plistPath];
			
	// 생성이 되었다면
	if (dictionnary)
	{
		self.situationDictionary = dictionnary;
		[dictionnary release];

		// 딕셔너리의 키값 가져오기
		NSArray *arrayForSituationKey = [self.situationDictionary allKeys];
		self.situationKeyArray = arrayForSituationKey;		
		
		
		// 선택된 딕셔너리 설정 - 초기엔 맨 첫번째 값으로 선택
		NSDictionary *dictionaryForSelected = [self.situationDictionary objectForKey:[self.situationKeyArray objectAtIndex:0]];
		self.selectedDictionary = dictionaryForSelected;
		//[dictionaryForSelected release];
				
		// 선택된 딕셔너리의 키값 정렬하여 가져오기
		NSArray *arrayForSelectedKey = [self.selectedDictionary allKeys];
		NSArray *sortedArray = [arrayForSelectedKey sortedArrayUsingSelector:@selector(compare:)];
		self.selectedKeyArray = sortedArray;

		// 액티비티 인티케이터 뷰 생성
		UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
		self.indicatorView = aiView;
		[aiView release];
		self.indicatorView.frame = kActivityIndicatorViewFrameSizeOnNavi;
		self.indicatorView.hidesWhenStopped = YES;
		self.indicatorView.hidden = YES;
		[self.navigationController.navigationBar addSubview:self.indicatorView];
	}
	else
	{
		[self makeAlertViewForDictionaryFail];
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

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];    
}

- (void)viewDidUnload 
{
    [super viewDidUnload];
	
	self.situationPickerView = nil;
	self.searchButton = nil;
	self.alertViewForDB = nil;
	self.indicatorView = nil;
	self.dbConnector = nil;
	self.situationDictionary = nil;
	self.situationKeyArray = nil;
	self.selectedDictionary = nil;
	self.selectedKeyArray = nil;
}


- (void)dealloc 
{	
	self.situationPickerView = nil;
	self.searchButton = nil;
	self.alertViewForDB = nil;
	self.indicatorView = nil;
	self.dbConnector = nil;
	self.situationDictionary = nil;
	self.situationKeyArray = nil;
	self.selectedDictionary = nil;
	self.selectedKeyArray = nil;
	
    [super dealloc];	
}


#pragma mark -
#pragma mark 버튼 이벤트
- (IBAction)touchSearchButton:(id)sender
{
	// 액티비티 인디케이터 뷰 애니메이션 시작
	[self.indicatorView startAnimating];
	
	NSInteger mainRowNumber = [self.situationPickerView selectedRowInComponent:SITUATION_PICKER_TYPE_MAIN];
	NSInteger subRowNumber = [self.situationPickerView selectedRowInComponent:SITUATION_PICKER_TYPE_SUB];
	
	// 주제
	SUBJECT_SECTION section = [self getSubjectSectionFromString:[self.situationKeyArray objectAtIndex:mainRowNumber]];
	
	// 파라미터
	NSString *type = [self.selectedDictionary objectForKey:[self.selectedKeyArray objectAtIndex:subRowNumber]];
		
	// 데이터베이스 커넥터 생성
	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = connector;
	[connector release];
	self.dbConnector.arrayRestaurantsFromSituationDelegate = self;
	[self.dbConnector startReceivingArrayRestaurantsFromSituation:section withParam:type];	
}


#pragma mark -
#pragma mark 주제별 열거자 가져오기
- (SUBJECT_SECTION)getSubjectSectionFromString:(NSString*)type
{
	SUBJECT_SECTION section;
	
	if ([type isEqualToString:kSUBJECT_THEME_NAME])
	{
		section = SUBJECT_THEME;
	}
	
	else if ([type isEqualToString:kSUBJECT_PRICE_NAME])
	{
		section = SUBJECT_PRICE;
	}
	
	else if ([type isEqualToString:kSUBJECT_MOOD_NAME])
	{
		section = SUBJECT_MOOD;
	}
	
	else if ([type isEqualToString:kSUBJECT_INTERIA_NAME])
	{
		section = SUBJECT_INTERIA;
	}
	
	else if ([type isEqualToString:kSUBJECT_AGE_NAME])
	{
		section = SUBJECT_AGE;
	}
	
	return section;
}


#pragma mark -
#pragma mark 알람뷰 생성 메소드
// 알람뷰 생성 - 주제별 딕셔너리 생성실패
-(void)makeAlertViewForDictionaryFail
{
	// 주제별 딕셔너리 생성실패 알람뷰 생성
	UIAlertView *view = [[UIAlertView alloc]
						 initWithTitle:@"에러" 
						 message:@"SearchBySituationTypeList.plist 파일이 없습니다."
						 delegate:nil
						 cancelButtonTitle:@"확인"
						 otherButtonTitles:nil
						 ];
	
	// 알람뷰 보이기
	[view show];
	[view release];
}


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
	
}


#pragma mark -
#pragma mark 피커뷰 델리게이트 메소드
//	2개의 컴포넌트로 구성된다.
//	예) 분위기 - 아늑함
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{	
	return SITUATION_PICKER_TYPE_TOTAL_COUNT;
}


// returns the # of rows in each component..
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
	NSInteger numberOfRows = 0;
	
	switch (component)
	{
		case SITUATION_PICKER_TYPE_MAIN:
		{
			numberOfRows = [self.situationDictionary count];
		}	break;
			
		case SITUATION_PICKER_TYPE_SUB:
		{
			numberOfRows = [self.selectedDictionary count];
		}	break;
	}
	
	return numberOfRows;
}


- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
	NSString *titleForRow = nil;
	
	switch (component)
	{
		case SITUATION_PICKER_TYPE_MAIN:
		{
			titleForRow = [self.situationKeyArray objectAtIndex:row];
		}	break;
			
		case SITUATION_PICKER_TYPE_SUB:
		{
			titleForRow = [self.selectedKeyArray objectAtIndex:row];
		}	break;
	}
	
	return titleForRow;
}


- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
	if (component == SITUATION_PICKER_TYPE_MAIN) 
	{
		self.selectedDictionary = nil;
		self.selectedKeyArray = nil;
		
		// 서브 딕셔너리 가져오기
		NSDictionary *dictionary = [self.situationDictionary objectForKey:[self.situationKeyArray objectAtIndex:row]];
		self.selectedDictionary = dictionary;
		
		// 선택된 딕셔너리의 키값 정렬하여 가져오기
		NSArray *arrayForSelectedKey = [self.selectedDictionary allKeys];
		NSArray *sortedArray = [arrayForSelectedKey sortedArrayUsingSelector:@selector(compare:)];
		self.selectedKeyArray = sortedArray;
		
		[pickerView selectRow:0 inComponent:SITUATION_PICKER_TYPE_SUB animated:YES];
		[pickerView reloadComponent:SITUATION_PICKER_TYPE_SUB];
	}
}

#pragma mark -
#pragma mark 주제별 검색 결과 맛집 array 전송 프로토콜 구현 메소드
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
