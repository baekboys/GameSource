#import "FavoriteResultTableViewController.h"
#import "SQLiteDatabaseConnector.h"
#import "SQLiteDB.h"
#import "Restaurant.h"

@implementation FavoriteResultTableViewController


- (id)init
{	
	if (self = [super initWithArrayRestaurants:nil withAreaName:@"단골 맛집"])
	{
		// 단골맛집을 보여주는 결과 테이블에서는 맛집뷰의 단골맛집 추가 버튼을 생성할 필요가 없다.!!!!
		self.makeJoinButtonForRestaurantView = NO;
	}
		
	return self;
}

- (void)loadView 
{
	[super loadView];
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
}


- (void)dealloc 
{	
    [super dealloc];
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	
	// 네비게이션바의 삭제버튼 생성
	UIBarButtonItem *editButton = [[UIBarButtonItem alloc] initWithTitle:@"삭제" style:UIBarButtonItemStyleBordered target:self action:@selector(toggleEdit:)];
	self.navigationItem.leftBarButtonItem = editButton;
	[editButton release];
}

- (void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	
	// sqlite 커넥터 생성
	SQLiteDatabaseConnector *connector = [[SQLiteDatabaseConnector alloc] init];
	
	// 연결이 잘 되었는지 확인
	if ([connector.sqliteDB isGoodConnection])
	{
		// sqlite로부터 단골맛집 데이터 가져오기
		NSMutableArray *restaurnatArray = [connector getRestaurantArrayFromDB];
		
		// 기존의 맛집데이터 array 삭제
		self.arrayRestaurants = nil;
		
		// 단골맛집 데이터 설정
		self.arrayRestaurants = restaurnatArray;
		
		// 테이블뷰 리로드
		[self.areaResultTableView reloadData];
	}
	
	// 커넥터 릴리즈
	[connector release];
	
	
	// 네비게이션바의 타이틀뷰 설정
	UILabel *titleLabel = [[UILabel alloc] init];
	titleLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:JBFOOD_NAVIGATION_TITLE_FONT_SIZE];
	titleLabel.text = self.title;
	titleLabel.backgroundColor = [UIColor clearColor];
	[titleLabel sizeToFit];
	self.navigationItem.titleView = titleLabel;
	[titleLabel release];
}
								   

// 삭제버튼 토글
- (void)toggleEdit:(id)sender
{
	[self.areaResultTableView setEditing:!self.areaResultTableView.editing animated:YES];
	
	if (self.areaResultTableView.editing)
	{
		[self.navigationItem.leftBarButtonItem setTitle:@"완료"];
	}
	else
	{
		[self.navigationItem.leftBarButtonItem setTitle:@"삭제"];
	}
}

// 삭제 메소드
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
	Restaurant *restaurant = [self.arrayRestaurants objectAtIndex:indexPath.row];
	
	// sqlite 커넥터 생성
	SQLiteDatabaseConnector *connector = [[SQLiteDatabaseConnector alloc] init];
	
	// 연결이 잘 되었는지 확인
	if ([connector.sqliteDB isGoodConnection])
	{
		// 삭제 실행
		if([connector removeRestaurantFromSQliteDBWithID:restaurant.id])
		{
			// 단골맛집데이터 array에서 삭제
			[self.arrayRestaurants removeObjectAtIndex:indexPath.row];
			
			// 셀 삭제하기
			[tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
		}
	}
	
	// 커넥터 릴리즈
	[connector release];
}

@end
