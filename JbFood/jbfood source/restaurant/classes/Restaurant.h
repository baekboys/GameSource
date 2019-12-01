//-------------------------------------
// 맛집 클래스
// 맛집DB의 Restaurant 테이블과 동일한 구조
//-------------------------------------
#import <Foundation/Foundation.h>

// 데이터베이스 커넥터 - 이미지 다운로드를 위해 필요
#import "DatabaseConnector.h"

// 이미지 다운로드 상태
typedef enum
{
	IMG_DOWNLOAD_READY,
	IMG_DOWNLOAD_DOWNLOADING,
	IMG_DOWNLOAD_FINISHED,
}	IMG_DOWNLOAD_STATE;

@protocol RestaurantImageDelegate;

@interface Restaurant : NSObject 
<
DatabaseConnectorReceiveDataDelegate
//, receiveRecommendationMenuFromIdDelegate
>
{
	// ----------------
	//	맛집 데이터베이스
	// ----------------
	
	NSUInteger								id;						// 아이디 : primary key

	NSString								*name;					// 이름
	NSString								*phone;					// 전화번호
	NSString								*address;				// 주소
	NSString								*city;					// 위치주소
	
//	NSArray									*arrayImageFileName;	// 이미지파일명을 가진 array
	NSString								*imageFileName;			// 이미지파일명
	
	double									latitude;				// 위도
	double									longitude;				// 경도
	
	NSString								*intro;					// 소개
	//NSString								*review;				// 리뷰
	
	NSString								*time;					// 영업시간
	NSString								*car_support;			// 차량지원
	NSString								*accomodation;			// 좌석수
	NSString								*holyday;				// 휴일영업
	NSString								*park;					// 주차공간
	NSString								*reservation;			// 예약가능
	NSString								*etc1;					// 기타1
	NSString								*etc2;					// 기타2
	
	NSArray									*recommendationMenu;	// 추천메뉴
	
	NSArray									*reviewArray;			// 사용자 리뷰
	NSUInteger								starCount;				// 총 별점 개수
	NSUInteger								starAverage;			// 총 별점 평균
	NSString								*starString;			// 별점의 문자열
	
	// ---------------
	//	UI와 이벤트 관련
	// ---------------
	DatabaseConnector						*dbConnector;			// 데이터베이스 커넥터
	NSMutableData							*imageData;				// 이미지데이터 (바이너리 파일)
	id<RestaurantImageDelegate>				imageDelegate;
	UIImageView								*imageView;				// 이미지를 보여주기 위한 이미지뷰
	IMG_DOWNLOAD_STATE						imageDownloadState;		// 이미지 다운로드 상태
}


#pragma mark -
#pragma mark 숫자 타입
@property (nonatomic) NSUInteger id;
@property (nonatomic) double latitude;
@property (nonatomic) double longitude;
@property (nonatomic) IMG_DOWNLOAD_STATE imageDownloadState;
@property (nonatomic) NSUInteger starCount;
@property (nonatomic) NSUInteger starAverage;


#pragma mark -
#pragma mark string 타입 : copy
@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSString *phone;
@property (nonatomic, copy) NSString *address;
@property (nonatomic, copy) NSString *city;
@property (nonatomic, copy) NSString *intro;
@property (nonatomic, copy) NSString *time;
@property (nonatomic, copy) NSString *car_support;
@property (nonatomic, copy) NSString *accomodation;
@property (nonatomic, copy) NSString *holyday;
@property (nonatomic, copy) NSString *park;
@property (nonatomic, copy) NSString *reservation;
@property (nonatomic, copy) NSString *etc1;
@property (nonatomic, copy) NSString *etc2;
@property (nonatomic, copy) NSString *imageFileName;
@property (nonatomic, copy) NSString *starString;

#pragma mark -
#pragma mark NSData 타입 : retain
@property (nonatomic, retain) DatabaseConnector *dbConnector;
@property (nonatomic, retain) NSMutableData *imageData;
@property (nonatomic, retain) UIImageView *imageView;
@property (nonatomic, retain) NSArray *recommendationMenu;
@property (nonatomic, retain) NSArray *reviewArray;

#pragma mark -
#pragma mark delegate 타입 : assign
@property (nonatomic, assign) id<RestaurantImageDelegate> imageDelegate;


#pragma mark -
#pragma mark 이미지데이터 관련 메소드
// 이미지 다운로드가 가능한지 검사
- (BOOL)isPossibleDownloadImage;
// 이미지뷰에다가 그리기위하여 이미지 다운로드 시작
- (void)startDownloadingForImageView:(UIImageView*)view;
// 이미지뷰에다가 그리기위하여 이미지 다운로드 시작
- (void)startThumbnailDownloadingForImageView:(UIImageView*)view;
// 데이터베이스 커넥터 작업 중지 밋 삭제
- (void)removeDbConnector;


#pragma mark -
#pragma mark 사용자리뷰 관련 메소드
// 평균 별점으로부터 문자열 별점으로 가져오기
- (NSString*)getStarStringFromStarAverage:(NSUInteger)newStarAverage;

@end

#pragma mark -
#pragma mark 이미지데이터 프로토콜 델리게이트
@protocol RestaurantImageDelegate
-(void)imageDidReceivingData:(Restaurant*)restaurant withData:(NSData*)data;
-(void)imageDidFail:(Restaurant*)restaurant withError:(NSError*)error;
-(void)imageDidFinish:(Restaurant*)restaurant;
@end

