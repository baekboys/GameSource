#import <Foundation/Foundation.h>

@class SQLiteDB;
@class Restaurant;


// sqlite에 맛집추가 결과 열거자
typedef enum
{
	ADD_RETURN_COMPLETE,			// 맛집 데이터 추가 성공
	ADD_RETURN_FAILED,				// 맛집 데이터 추가 실패
	ADD_RETURN_ALREADY,				// 이미 등록이 되어 있음.
	ADD_RETURN_NO_CONNECTION		// 데이터베이스 연결이 되어 있지 않음.
}	ADD_RETURN_TYPE;

@interface SQLiteDatabaseConnector : NSObject 
{
	SQLiteDB *sqliteDB;
}

@property (nonatomic, retain) SQLiteDB *sqliteDB;


// sqlite에 맛집 데이터 추가
- (ADD_RETURN_TYPE)addRestaurantToSQliteDB:(Restaurant*)newRestaurant;

// sqlite로부터 데이터 삭제
- (BOOL)removeRestaurantFromSQliteDBWithID:(NSInteger)id;

// 맛집 데이터가 있는지 id값으로 검사
- (BOOL)findRegistedRestaurantDBFromID:(NSInteger)id;

// 맛집 array 데이터로 가져오기
- (NSMutableArray*)getRestaurantArrayFromDB;

@end
