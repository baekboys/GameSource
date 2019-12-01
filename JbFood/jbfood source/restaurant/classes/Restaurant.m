#import "Restaurant.h"
#import "RecommendationMenu.h"

@implementation Restaurant


@synthesize id;
@synthesize name;
@synthesize phone;
@synthesize address;
@synthesize city;
@synthesize latitude;
@synthesize longitude;
@synthesize intro;
@synthesize dbConnector;
@synthesize imageData;
@synthesize imageDelegate;
@synthesize imageView;
@synthesize imageDownloadState;
@synthesize time;
@synthesize car_support;
@synthesize accomodation;
@synthesize holyday;
@synthesize park;
@synthesize reservation;
@synthesize etc1, etc2;
@synthesize recommendationMenu;
@synthesize imageFileName;
@synthesize reviewArray;
@synthesize starCount;
@synthesize starAverage;
@synthesize starString;

#pragma mark -
#pragma mark 맛집 인스턴스 생성 및 삭제
-(id)init
{
	if(self = [super init])
	{
		self.name = nil;
		self.phone = nil;
		self.address = nil;
		self.city = nil;
		self.latitude = 0;
		self.longitude = 0;
		self.intro = nil;
		self.time = nil;
		self.car_support = nil;
		self.accomodation = nil;
		self.holyday = nil;
		self.park = nil;
		self.reservation = nil;
		self.etc1 = nil;
		self.etc2 = nil;
		self.recommendationMenu = nil;
		self.reviewArray = nil;
		self.imageFileName = nil;
		self.starString = nil;
		
		self.dbConnector = nil;
		self.imageData = nil;
		self.imageDelegate = nil;
		self.imageView = nil;
		self.imageDownloadState = IMG_DOWNLOAD_READY;
	}
	
	return self;
}


- (void)dealloc 
{
	self.name = nil;
	self.phone = nil;
	self.address = nil;
	self.city = nil;
	self.intro = nil;
	self.time = nil;
	self.car_support = nil;
	self.accomodation = nil;
	self.holyday = nil;
	self.park = nil;
	self.reservation = nil;
	self.etc1 = nil;
	self.etc2 = nil;
	self.recommendationMenu = nil;
	self.reviewArray = nil;
	self.imageFileName = nil;
	self.starString = nil;
	
	self.imageData = nil;
	self.imageView = nil;
	
	if (self.dbConnector)
		[self removeDbConnector];
	
	[super dealloc];
}


#pragma mark -
#pragma mark 이미지데이터 관련 메소드
// 이미지 다운로드가 가능한지 검사
- (BOOL)isPossibleDownloadImage
{
	return ( self.imageFileName != nil );
}


// 이미지뷰에다가 그리기위하여 다운로드 시작
- (void)startDownloadingForImageView:(UIImageView*)view;
{
	// 이미지뷰를 참조 - 셀의 이미지뷰
	self.imageView = view;
	
	// 데이터베이스 커넥터 생성
	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = connector;
	[connector release];
	self.dbConnector.receiveDataDelegate = self;
		
	// 이지미데이터의 URL과 이미지 파일명을 합친 최종 URL을 이용하여 이미지파일 전송
	[self.dbConnector startReceivingDataFromURL: [NSString stringWithFormat:@"%@%@", JBFOOD_DATABASE_URL_IMAGEDATA, self.imageFileName] ];
	
	// 이미지 다운로드 상태 설정
	self.imageDownloadState = IMG_DOWNLOAD_DOWNLOADING;
}


// 이미지뷰에다가 그리기위하여 이미지 다운로드 시작
- (void)startThumbnailDownloadingForImageView:(UIImageView*)view
{
	// 이미지뷰를 참조 - 셀의 이미지뷰
	self.imageView = view;
	
	// 데이터베이스 커넥터 생성
	DatabaseConnector *connector = [[DatabaseConnector alloc] init];
	self.dbConnector = connector;
	[connector release];
	self.dbConnector.receiveDataDelegate = self;
	
	// 이지미데이터의 URL과 이미지 파일명을 합친 최종 URL을 이용하여 이미지파일 전송
	[self.dbConnector startReceivingDataFromURL: [NSString stringWithFormat:@"%@%@", JBFOOD_DATABASE_URL_THUMBNAILDATA, self.imageFileName] ];
	
	// 이미지 다운로드 상태 설정
	self.imageDownloadState = IMG_DOWNLOAD_DOWNLOADING;
	
}


// 데이터베이스 커넥터 작업 중지 밋 삭제
- (void)removeDbConnector
{
	self.dbConnector.receiveDataDelegate = nil;
	self.dbConnector = nil;
}



#pragma mark -
#pragma mark 사용자리뷰 관련 메소드
// 평균 별점으로부터 문자열 별점으로 가져오기
- (NSString*)getStarStringFromStarAverage:(NSUInteger)newStarAverage
{
	NSString *activeStar = @"★";
	NSString *deactiveStar = @"☆";
	
	NSMutableString *mutableString = [NSMutableString stringWithCapacity:5];
	
	NSUInteger index;
	for ( index = 1 ; index < 6 ; ++index ) 
	{
		if (index <= newStarAverage)
			[mutableString appendString:activeStar];
		else
			[mutableString appendString:deactiveStar];
	}
	
	return [NSString stringWithString:mutableString];
}



#pragma mark -
#pragma mark 데이터전송 프로토콜 구현 메소드
- (void)didReceive:(DatabaseConnector*)connector withResponse:(NSURLResponse *)response
{
	// 이미지데이터를 저장할 데이터 생성
	self.imageData = [NSMutableData data];
	[self.imageData setLength:0];
}

- (void)didFail:(DatabaseConnector*)connector withError:(NSError *)error
{
	// 데이터베이스 커넥터 릴리즈
	[self removeDbConnector];
	
	// 이미지 델리게이트 메소드 호출 - 에러
	[self.imageDelegate imageDidFail:self withError:error];
}


- (void)didReceivingData:(DatabaseConnector*)connector withData:(NSData *)data
{
	[self.imageData appendData:data];
		
	// 이미지 델리게이트 메소드 호출 - self.imageData를 보낸다.
	[self.imageDelegate imageDidReceivingData:self withData:self.imageData];
	
}

- (void)didFinishReceivingData:(DatabaseConnector*)connector
{
	// 데이터베이스 커넥터 릴리즈
	[self removeDbConnector];
	
	// 전송된 데이터가 이미지로 변환이 되었을 경우에만 이미지뷰에 이미지를 그린다.
	UIImage *recievedImage = [UIImage imageWithData:self.imageData];
	if (recievedImage)
	{
		self.imageView.image = recievedImage;
	}
	
	// 이미지 다운로드 상태 설정
	self.imageDownloadState = IMG_DOWNLOAD_FINISHED;
	
	// 이미지 델리게이트 메소드 호출 - 종료
	[self.imageDelegate imageDidFinish:self];	
}

#pragma mark -
#pragma mark 추천메뉴 전송 프로토콜 구현 메소드
- (void)recommendationMenuDidFail:(DatabaseConnector*)connector withError:(NSError*)data
{
	// 커넥터 릴리즈
	[connector release];
}

- (void)recommendationMenuDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
{
	
}


- (void)recommendationMenuDidFinish:(DatabaseConnector*)connector withMenuArray:(NSArray*)array
{
	// 커넥트 릴리즈
	[connector release];
	
	// 추천메뉴 데이터 설정.
	self.recommendationMenu = array;
}


@end
