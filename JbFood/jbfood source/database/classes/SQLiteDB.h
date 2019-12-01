#import <Foundation/Foundation.h>
#import "FMDatabase.h"
#import "FMDatabaseAdditions.h"
#import "FMResultSet.h"

@interface SQLiteDB : NSObject 
{
	// -----------------------------------------------------------
	//	FMDatabase는 내부적으로 autorelase풀에 들어가는것으로 보인다.
	//	따라서 property를 retain 하지 않고, assign으로 하였다.
	//	또한 release를 할필요가 없으며, 종료하려면 close 메소드를 호출하면
	//	메모리에서 자동으로 해제가 된다.
	// -----------------------------------------------------------
	FMDatabase *fmdb;
}

@property (nonatomic, assign) FMDatabase *fmdb;

// 데이터베이스 파일을 열면서 초기화하기
- (id)initWithDBFile:(NSString*)fileName;

// 사용자 저장공간인 도튜먼트 디렉토리 얻어오기
- (NSString *)getDocumentsPath;

// 데이터베이스 파일이 있는지 검사
- (void)checkDBFile:(NSString*)fileName;

// 데이터베이스파일 열기
- (BOOL)openDBFile:(NSString*)fileName;

// 데이터베이스가 연결되어 있는지 확인
- (BOOL)isGoodConnection;

@end
