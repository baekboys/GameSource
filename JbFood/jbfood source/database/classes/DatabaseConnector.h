#import <Foundation/Foundation.h>

// 맛집의 데이터베이스를 위한 URL
#import "SHARED_DEFINE.h"

// 맛집 클래스
@class Restaurant;


#pragma mark -
#pragma mark 맛집 데이터베이스 구조
// JBFood 데이터베이스 culumn
//	THID		:	id	
//	F_SNAME		:	이름
//	F_STIME		:	영업시간
//	F_STEL		:	전화번호
//	F_SADDR		:	주소
//	F_SCAR		:	차량지원
//	F_SCSEAT	:	좌석수
//	F_SCTYPE	:	예약가능 ?
//	F_SHDAY		:	휴일유무
//	F_SCONTENT	:	소개
//	F_SFOOD		:	?
//	F_SEQ		:	기타 데이터
//	F_SCSPACE	:	주차공간
//	F_SETC		:	기타
//	F_SMAP		:	지도
//	IMGNAME		:	이미지파일명
//	REG_DATE	:	등록날짜
//	F_WRITER	:	?
//	F_PUSE		:	지금까지 영업중인지?
//	F_SPART		:	?
//	F_SATM		:	?
//	F_SINTE		:	인테리어타입
//	F_SPRICE	:	가격 등급
//	F_SAGE		:	연령병 등급
//	GUGUN		:	구/군
//	SFOODID		:	음식ID
//	F_SPHONE	:	휴대폰
//	F_STITLE	:	자막소개
//	F_LATITUDE	:	위도
//	F_LONGITUDE	:	경도

// 커넥션 타입
typedef enum
{
	CONNECTION_NULL,
	CONNECTION_DATA,
	CONNECTION_FROM_AREA,
	CONNECTION_FROM_ID,
	CONNECTION_FROM_NAME,
	CONNECTION_FROM_SITUATION,
	//CONNECTION_MENU_FROM_ID,
	CONNECTION_ALL_RESTAURANT,
	CONNECTION_USER_REVIEW_WRITE
}	CONNECTION_TYPE;

#define DATABASE_NULL_STRING @"(null)"

// 데이터전송 프로토콜
@protocol DatabaseConnectorReceiveDataDelegate;
@protocol receiveArrayRestaurantsFromAreaDelegate;
@protocol receiveRestaurantFromIdDelegate;
@protocol receiveArrayRestaurantsFromNameDelegate;
@protocol receiveArrayRestaurantsFromSituationDelegate;
//@protocol receiveRecommendationMenuFromIdDelegate;
@protocol receiveAllRestaurantsDelegate;
@protocol sendUserReviewDelegate;



@interface DatabaseConnector : NSObject 
{
	// 데이터전송 델리게이트
	id<DatabaseConnectorReceiveDataDelegate> receiveDataDelegate;
	
	// 맛집 array 델리게이트
	id<receiveArrayRestaurantsFromAreaDelegate> arrayRestaurantsFromAreaDelegate;
	
	// 맛집 array 델리게이트
	id<receiveArrayRestaurantsFromNameDelegate> arrayRestaurantsFromNameDelegate;
	
	// 맛집 array 델리게이트
	id<receiveArrayRestaurantsFromSituationDelegate> arrayRestaurantsFromSituationDelegate;
	
	// 맛집 데이터 델리게이트
	id<receiveRestaurantFromIdDelegate> restaurantFromIdDelegate;
	
	// 맛집의 추천메뉴 델리게이트
	//id<receiveRecommendationMenuFromIdDelegate> recommendationMenuFromIdDelegate;
	
	// 모든 맛집 데이터 델리게이트
	id<receiveAllRestaurantsDelegate> allRestaurantsDelegate;
	
	// 한줄평 리뷰 데이터 델리게이트
	id<sendUserReviewDelegate> userReviewDelegate;
	
	// 데이터베이스로 부터 전송되는 데이터를 저장할 공간
	NSMutableData *receivedData;
	
	// 데이터의 타입
	CONNECTION_TYPE connectionType;
}

#pragma mark -
#pragma mark property
@property (nonatomic, assign)	id<DatabaseConnectorReceiveDataDelegate> receiveDataDelegate;
@property (nonatomic, assign)	id<receiveArrayRestaurantsFromAreaDelegate> arrayRestaurantsFromAreaDelegate;
@property (nonatomic, assign)	id<receiveArrayRestaurantsFromNameDelegate> arrayRestaurantsFromNameDelegate;
@property (nonatomic, assign)	id<receiveArrayRestaurantsFromSituationDelegate> arrayRestaurantsFromSituationDelegate;
@property (nonatomic, assign)	id<receiveRestaurantFromIdDelegate> restaurantFromIdDelegate;
//@property (nonatomic, assign)	id<receiveRecommendationMenuFromIdDelegate> recommendationMenuFromIdDelegate;
@property (nonatomic, assign)	id<receiveAllRestaurantsDelegate> allRestaurantsDelegate;
@property (nonatomic, assign)	id<sendUserReviewDelegate> userReviewDelegate;
@property (nonatomic, retain)	NSMutableData *receivedData;
@property (nonatomic)			CONNECTION_TYPE connectionType;

#pragma mark -
#pragma mark 파싱이전의 수순한 string 타입 - 내부적으로만 사용
// JSON Parser를 이용하여 맛집데이터베이스를 Restaurant array로 가져온다.
- (NSMutableArray*)getRestaurantsArrayFromDatabaseByJSonParser:(NSString*)resultData;
// JSON Parser를 이용하여 맛집데이터베이스를 Restaurant로 가져온다.
- (Restaurant*)getRestaurantDataByJSonParser:(NSString*)resultData;
// 데이터베이스로부터 맛집 이미지에 관한 파일명 받아서 파싱하여 array에 넣는다.
- (NSString*)getImageNameFromRawData:(NSString*)rawData;
// 데이터베이스로부터 한줄평 리뷰에 관한 결과를 문자열로 가져온다.
- (NSString*)getUserReviewResultFromDatabaseByJSonParser:(NSString*)resultData;
//// 데이터베이스로부터 추천메뉴를 받아서 array에 넣는다.
//- (NSArray*)getRecommendationMenuArrayFromDatabaseByJSonParser:(NSString*)resultData;
// 널문자인지 확인하여 "(null)"이면 "", "Y"이면 "예", "N"이면 "아니오"를 반환
- (NSString*)checkNullString:(NSString*)newString;

#pragma mark -
#pragma mark 데이터 전송시작
// 데이터 전송 시작
- (BOOL)startReceivingDataFromURL:(NSString*)dataURL;
// 지역검색 결과 맛집 array 데이터 전송시작
- (BOOL)startReceivingArrayRestaurantsFromArea:(NSString*)area;
// 상호명 검색 결과 맛집 array 데이터 전송시작
- (BOOL)startReceivingArrayRestaurantsFromName:(NSString*)name;
// 상황별 검색 결과 맛집 array 데이터 전송시작
- (BOOL)startReceivingArrayRestaurantsFromSituation:(SUBJECT_SECTION)section withParam:(NSString*)param;
// 맛집 데이터 전송시작
- (BOOL)startReceivingRestaurantFromId:(int)id;
//// 추천메뉴 전송시작
//- (BOOL)startReceivingRecommendationMenuFromId:(int)id;
// 모든 맛집 전송시작
- (BOOL)startReceivingAllRestaurants;
// 한줄평 리뷰 전송시작
- (BOOL)startSendingUserReivew:(NSUInteger)id Phone:(NSString*)phoneNumber Review:(NSString*)review Star:(NSUInteger)star;

// 데이터객체 만들기
- (void)makeMutableData;

#pragma mark -
#pragma mark 델리게이트 삭제
- (void)removeAllDelegateReferenceToNil;


@end


#pragma mark -
#pragma mark 데이터전송 프로토콜 구현 메소드
@protocol DatabaseConnectorReceiveDataDelegate
- (void)didReceive:(DatabaseConnector*)connector withResponse:(NSURLResponse *)response;
- (void)didFail:(DatabaseConnector*)connector withError:(NSError *)error;
- (void)didReceivingData:(DatabaseConnector*)connector withData:(NSData *)data;
- (void)didFinishReceivingData:(DatabaseConnector*)connector;
@end


#pragma mark -
#pragma mark 지역검색 결과 맛집 array 전송 프로토콜 구현 메소드
@protocol receiveArrayRestaurantsFromAreaDelegate
- (void)arrayDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
- (void)arrayDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
- (void)arrayDidFinish:(DatabaseConnector*)connector withArrayRestaurants:(NSMutableArray*)array;
@end


#pragma mark -
#pragma mark 지역검색 결과 맛집 array 전송 프로토콜 구현 메소드
@protocol receiveRestaurantFromIdDelegate
- (void)restaurantDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
- (void)restaurantDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
- (void)restaurantDidFinish:(DatabaseConnector*)connector withRestaurant:(Restaurant*)newRestaurant;
@end


#pragma mark -
#pragma mark 상호명 검색 결과 맛집 array 전송 프로토콜 구현 메소드
@protocol receiveArrayRestaurantsFromNameDelegate
- (void)arrayDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
- (void)arrayDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
- (void)arrayDidFinish:(DatabaseConnector*)connector withArrayRestaurants:(NSMutableArray*)array;
@end


#pragma mark -
#pragma mark 상황별 검색 결과 맛집 array 전송 프로토콜 구현 메소드
@protocol receiveArrayRestaurantsFromSituationDelegate
- (void)arrayDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
- (void)arrayDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
- (void)arrayDidFinish:(DatabaseConnector*)connector withArrayRestaurants:(NSMutableArray*)array;
@end


//#pragma mark -
//#pragma mark 추천메뉴 전송 프로토콜 구현 메소드
//@protocol receiveRecommendationMenuFromIdDelegate
//- (void)recommendationMenuDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
//- (void)recommendationMenuDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
//- (void)recommendationMenuDidFinish:(DatabaseConnector*)connector withMenuArray:(NSArray*)array;
//@end


#pragma mark -
#pragma mark 모든맛집 전송 프로토콜 구현 메소드
@protocol receiveAllRestaurantsDelegate
- (void)allRestaurantsDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
- (void)allRestaurantsDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
- (void)allRestaurantsDidFinish:(DatabaseConnector*)connector withArray:(NSMutableArray*)array;
@end


#pragma mark -
#pragma mark 한줄평 리뷰 전송 프로토콜 구현 메소드
@protocol sendUserReviewDelegate
- (void)sendUserReviewDidFail:(DatabaseConnector*)connector withError:(NSError*)data;
- (void)sendUserReviewDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data;
- (void)sendUserReviewDidFinish:(DatabaseConnector*)connector withResult:(NSString*)result;
@end
