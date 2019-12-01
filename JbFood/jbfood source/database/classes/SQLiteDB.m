#import "SQLiteDB.h"


@implementation SQLiteDB


@synthesize fmdb;


- (id)initWithDBFile:(NSString*)fileName
{
	if(self = [super init])
	{
		// 데이터베이스 파일있는지 확인
		[self checkDBFile:fileName];
		// 데이터베이스 파일 열기
		[self openDBFile:fileName];		
	}
	
	return self;
}

- (void)checkDBFile:(NSString*)fileName
{
	// 파일매니저 불러오기
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSError *error;
	
	// 아이폰의 사용자 저장장소인 도튜먼트에 데이터베이스 파일이 있는지 검사
	// 데이터베이스 파일명 : 도튜먼트디렉토리 (getDocumentPath) + 파일명
	NSString *dbPath = [[self getDocumentsPath] stringByAppendingFormat:@"/%@",fileName];

	// 파일이 존재하는지 확인
	BOOL success = [fileManager fileExistsAtPath:dbPath];

	// 도튜먼트 디렉토리에 파일이 없으므로 리소스 디렉토리로부터 파일을 가져와서 복사
	if(!success)
	{
		NSString *defaultDBPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:fileName];
		success = [fileManager copyItemAtPath:defaultDBPath toPath:dbPath error:&error];
	}
}

- (NSString *)getDocumentsPath
{
	// 사용자 저장 영역인 도튜먼트 패스 얻어오기
	NSArray *path = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDir = [path objectAtIndex:0];
	return documentsDir;
}

- (BOOL)openDBFile:(NSString*)fileName;
{
	// 데이터베이스 파일명 : 도튜먼트디렉토리 (getDocumentPath) + 파일명
	NSString *dbPath = [[self getDocumentsPath] stringByAppendingFormat:@"/%@",fileName];
	
	// FMDatabase 핸들 얻어오기
	FMDatabase *db = [FMDatabase databaseWithPath:dbPath];
	self.fmdb = db;
	
	// 데이터베이스 열기 시도
	if ([fmdb open])
	{
		// 설정
		[fmdb setShouldCacheStatements:YES];

		// 성공적으로 열렸으므로 YES값 반환
		return YES;
	}
	return NO;
}


// 데이터베이스가 연결되어 있는지 확인
- (BOOL)isGoodConnection
{
	return [fmdb goodConnection];
}


- (void)dealloc
{
	// 데이터베이스 핸들이 있다면 닫기
	if (self.fmdb)
	{
		[fmdb close];
		self.fmdb = nil;
	}
	
	[super dealloc];
}


@end
