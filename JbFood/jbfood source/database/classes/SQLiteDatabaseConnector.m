#import "SQLiteDatabaseConnector.h"
#import "SQLiteDB.h"
#import "SHARED_DEFINE.h"
#import "Restaurant.h"
#import "RecommendationMenu.h"

@implementation SQLiteDatabaseConnector


@synthesize sqliteDB;


// 초기화
- (id)init
{
	if (self = [super init])
	{
		SQLiteDB *db = [[SQLiteDB alloc] initWithDBFile:JBFOOD_SQLTIE_FILENAME];
		self.sqliteDB = db;
		[db release];
	}
	
	return self;
}


- (void)dealloc
{
	self.sqliteDB = nil;
	[super dealloc];
}


// sqlite에 맛집 데이터 추가
- (ADD_RETURN_TYPE)addRestaurantToSQliteDB:(Restaurant*)newRestaurant
{
	ADD_RETURN_TYPE resultType = ADD_RETURN_COMPLETE;
	
	// -----------------
	//	(1) 연결상태 확인
	// -----------------
	if (![sqliteDB isGoodConnection])
		return ADD_RETURN_NO_CONNECTION;
	
	
	// --------------------------------------------
	//	(2) 이미 데이터베이스에 저장이 되어있는지 확인한다.
	// --------------------------------------------
	if ([self findRegistedRestaurantDBFromID:newRestaurant.id])
		return ADD_RETURN_ALREADY;
	
	
	// -------------------------------------------------
	//	(3) 중복된 데이터가 없으므로 데이터를 추가할 준비를 한다.
	// -------------------------------------------------
	
	// 추천메뉴 가져오기.
	NSMutableString *menuString = [NSMutableString string];
	for (RecommendationMenu *menu in newRestaurant.recommendationMenu)
	{
		if ([newRestaurant.recommendationMenu lastObject] == menu)
			[menuString appendFormat:@"%@", menu.menu];
		else
			[menuString appendFormat:@"%@, ", menu.menu];
	}

	
	// 실제로 sqlite 데이터베이스 시스템에 추가한다.
	BOOL result =  [sqliteDB.fmdb  executeUpdate:
					@"insert into Restaurant (THID, F_SNAME, IMGNAME, F_LATITUDE, F_LONGITUDE, F_SMENU, F_SSTARSTRING) values (?,?,?,?,?,?,?)",
					[NSNumber numberWithInt:newRestaurant.id],
					newRestaurant.name,
					newRestaurant.imageFileName,
					[NSNumber numberWithDouble:newRestaurant.latitude],
					[NSNumber numberWithDouble:newRestaurant.longitude],
					menuString,
					newRestaurant.starString
					];
	
	// 성공여부 리턴
	if (!result)
	{
		resultType = ADD_RETURN_FAILED;
	}
	
	return resultType;
}


// sqlite로부터 데이터 삭제
- (BOOL)removeRestaurantFromSQliteDBWithID:(NSInteger)id
{
	if (![sqliteDB isGoodConnection])
		return NO;

	BOOL result = [sqliteDB.fmdb  executeUpdate:@"delete From Restaurant where THID = ?", [NSNumber numberWithInt:id]];
	
	if (!result)
		return NO;
	
	return YES;	
}


// 맛집 데이터가 있는지 id값으로 검사
- (BOOL)findRegistedRestaurantDBFromID:(NSInteger)id
{
	if (![sqliteDB isGoodConnection])
		return NO;
	
	// id값으로 검색하여 총 갯수 구하기
	int count = [sqliteDB.fmdb intForQuery:@"select count(*) from Restaurant where THID = ?", [NSNumber numberWithInt:id]];
	
	// 카운트값이 0이상이라면 데이터가 이미 존재한다는 의미
	if (count > 0)
	{
		return YES;
	}
	
	return NO;
}


// 맛집 array 데이터로 가져오기
- (NSMutableArray*)getRestaurantArrayFromDB
{
	if (![sqliteDB isGoodConnection])
		return nil;
	
	// 총 갯수 가져오기
	NSUInteger totalCount = [sqliteDB.fmdb intForQuery:@"select count(*) from Restaurant"];

	// 갯수로부터 array 생성
	NSMutableArray *restaurantsArray = [[NSMutableArray alloc] initWithCapacity:totalCount];

	// 데이터베이스 전체 가져오기
	FMResultSet *rs = [sqliteDB.fmdb executeQuery:@"select * from Restaurant order by F_SNAME asc"];
		
	while ([rs next])
	{
		// 맛집 객체 생성
		Restaurant *newRestaurant = [[Restaurant alloc] init];
		
		// 데이터 입력
		newRestaurant.id = [rs intForColumn:@"THID"];

		newRestaurant.name = [rs stringForColumn:@"F_SNAME"];
		
		newRestaurant.imageFileName = [rs stringForColumn:@"IMGNAME"];
		
		newRestaurant.latitude = [rs doubleForColumn:@"F_LATITUDE"];
		
		newRestaurant.longitude = [rs doubleForColumn:@"F_LONGITUDE"];
		
		NSString *menuString = [rs stringForColumn:@"F_SMENU"];
		if (menuString)
		{
			RecommendationMenu *menu = [[[RecommendationMenu alloc] init] autorelease];
			menu.menu = menuString;
			NSArray *array = [[[NSArray alloc] initWithObjects:menu, nil] autorelease];
			newRestaurant.recommendationMenu = array;
		}
		
		NSString *star = [rs stringForColumn:@"F_SSTARSTRING"];
		if (star)
		{
			newRestaurant.starString = star;
		}
		
		// array에 추가
		[restaurantsArray addObject:newRestaurant];

		// release
		[newRestaurant release];
	}
	
	[rs close];
	
	return [restaurantsArray autorelease];
}


@end
