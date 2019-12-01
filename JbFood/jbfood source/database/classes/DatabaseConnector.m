#import "DatabaseConnector.h"
#import "JSON.h"
#import "Restaurant.h"
#import "RecommendationMenu.h"
#import "UserReview.h"

@implementation DatabaseConnector

#pragma mark -
#pragma mark synthesize
@synthesize receiveDataDelegate;
@synthesize arrayRestaurantsFromAreaDelegate;
@synthesize arrayRestaurantsFromNameDelegate;
@synthesize arrayRestaurantsFromSituationDelegate;
@synthesize restaurantFromIdDelegate;
//@synthesize recommendationMenuFromIdDelegate;
@synthesize allRestaurantsDelegate;
@synthesize userReviewDelegate;
@synthesize receivedData;
@synthesize connectionType;


#pragma mark -
#pragma mark 초기화 메소드
- (id)init
{
	if (self = [super init])
	{
		// 델리게이트와 커넥션 타입 초기화
		self.receiveDataDelegate = nil;
		self.arrayRestaurantsFromAreaDelegate = nil;
		self.arrayRestaurantsFromNameDelegate = nil;
		self.arrayRestaurantsFromSituationDelegate = nil;
		self.restaurantFromIdDelegate = nil;
		//self.recommendationMenuFromIdDelegate = nil;
		self.allRestaurantsDelegate = nil;
		self.userReviewDelegate = nil;
		self.connectionType = CONNECTION_NULL;
	}
	
	return self;
}

- (void)dealloc
{
	self.receivedData = nil;
	[super dealloc];
}


// JSON Parser를 이용하여 맛집데이터베이스를 Restaurant array로 가져온다.
- (NSMutableArray*)getRestaurantsArrayFromDatabaseByJSonParser:(NSString*)resultData
{
	// JSON 파서 생성
	SBJsonParser *parser = [[[SBJsonParser alloc] init] autorelease];
	
	// ----------------------------------
	// 텍스트뷰에 웹에서 받아온 스트링을 넣는다.
	// 황당하지만 이렇게 해야 json 파싱이 된다.
	// ----------------------------------
	UITextView *textView = [[UITextView alloc] init];
	textView.text = resultData;
	
	// 파싱을 위해서 뉴라인을 제거한다.
	NSString *jsonData = [textView.text stringByReplacingOccurrencesOfString:@"\n" withString:@""];
	
	// 파싱을 위해서 파서에 데이터베이스 결과를 넣는다.
	NSError *error;
	NSArray *arrayParsingResults = (NSArray*)[parser objectWithString:jsonData error:&error];
	
	// 파싱한 결과값 즉, 맛집 데이터베이스의 갯수를 가지고 Restaurant를 저장하는 array컨테이너를 생성한다.
	NSMutableArray *arrayRestaurants = [NSMutableArray arrayWithCapacity:[arrayParsingResults count]];

	
	for (NSDictionary* dicRestaurant in arrayParsingResults)
	{
		if ([arrayParsingResults lastObject] == dicRestaurant)
			break;
		
		// Restaurnat 인스턴스를 생성한다.
		Restaurant *restaurant = [[Restaurant alloc] init];
		
		// id
		restaurant.id = [[dicRestaurant objectForKey:@"THID"] intValue];
		
		// 이름
		restaurant.name = [dicRestaurant objectForKey:@"F_SNAME"];
		
		// 위도
		restaurant.latitude = [[dicRestaurant objectForKey:@"F_LATITUDE"] doubleValue];
		
		// 경도
		restaurant.longitude = [[dicRestaurant objectForKey:@"F_LONGITUDE"] doubleValue];
		
		// 이미지 파일명 array
		restaurant.imageFileName = [self getImageNameFromRawData:[dicRestaurant objectForKey:@"IMGNAME"]];
		
		// 추천메뉴
		NSArray *arrayRecommendationMenu = (NSArray*)[dicRestaurant objectForKey:@"TB_SFOOD_MENU"];
		
		if (arrayRecommendationMenu && [arrayRecommendationMenu count] > 1)
		{
			// 추천메뉴가 존재한다면 추천메뉴 객체를 생성하여 데이터를 설정한다.
			NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:[arrayRecommendationMenu count]];
			
			for (NSDictionary *dicRecommendationMenu in arrayRecommendationMenu)
			{
				if ([arrayRecommendationMenu lastObject] != dicRecommendationMenu)
				{
					RecommendationMenu *menu = [[RecommendationMenu alloc] init];
					menu.menu = [self checkNullString:[dicRecommendationMenu objectForKey:@"S_MENU"]];
					menu.price = [self checkNullString:[dicRecommendationMenu objectForKey:@"S_PRICE"]];
					
					// 뮤터블 배열에 추가
					[mutableArray addObject:menu];
					[menu release];				
				}			
			}
			
			// 맛집 데이터에 추천메뉴 추가
			restaurant.recommendationMenu = [NSArray arrayWithArray:mutableArray];
			[mutableArray release];
		}
		
		
		// 사용자 평균 별점
		restaurant.starAverage = [[dicRestaurant objectForKey:@"TB_StarScore"] intValue];
		
		// 사용자 리뷰 갯수
		restaurant.starCount = [[dicRestaurant objectForKey:@"TB_StarCount"] intValue];
		
		// 문자열 별점
		restaurant.starString = [restaurant getStarStringFromStarAverage:restaurant.starAverage];
		
		
		// array에 추가한다.
		[arrayRestaurants addObject:restaurant];
		
		// 릴리즈시킨다.
		[restaurant release];
	}
	
	return arrayRestaurants;
}


- (Restaurant*)getRestaurantDataByJSonParser:(NSString*)resultData
{
	// JSON 파서 생성
	SBJsonParser *parser = [[[SBJsonParser alloc] init] autorelease];

	// ----------------------------------
	// 텍스트뷰에 웹에서 받아온 스트링을 넣는다.
	// 황당하지만 이렇게 해야 json 파싱이 된다.
	// ----------------------------------
	UITextView *textView = [[UITextView alloc] init];
	textView.text = resultData;
		
	// 파싱을 위해서 뉴라인을 제거한다.
	NSString *jsonData = [textView.text stringByReplacingOccurrencesOfString:@"\n" withString:@""];
	
	// 파싱을 위해서 파서에 데이터베이스 결과를 넣는다.
	NSError *error;
	id dictionary = [parser objectWithString:jsonData error:&error];
	
	// 텍스트 뷰 릴리즈
	[textView release];

	if (!dictionary)
	{
		return nil;
	}
		
	// 맛집 정보
	NSDictionary *dicRestaurant = (NSDictionary*)[((NSDictionary*)dictionary) objectForKey:@"TB_SFOOD"];
	
	// Restaurnat 인스턴스를 생성한다.
	Restaurant *restaurant = [[Restaurant alloc] init];
	
	// id
	restaurant.id = [[dicRestaurant objectForKey:@"THID"] intValue];
	
	// 이름
	restaurant.name = [self checkNullString:[dicRestaurant objectForKey:@"F_SNAME"]];
	
	// 주소 - 널값 확인
	restaurant.address = [self checkNullString:[dicRestaurant objectForKey:@"F_SADDR"]];
	
	// 전화번호 - 널값 확인
	restaurant.phone = [self checkNullString:[dicRestaurant objectForKey:@"F_STEL"]];
	
	// 이미지 파일명 array - 널값 확인
	restaurant.imageFileName = [self getImageNameFromRawData:[dicRestaurant objectForKey:@"IMGNAME"]];
	
	// 소개 - 널값 확인
	restaurant.intro = [self checkNullString:[dicRestaurant objectForKey:@"F_SCONTENT"]];
	
	// 리뷰 - 널값 확인
	//restaurant.review = [self checkNullString:[dicRestaurant objectForKey:@"F_STITLE"]];
	
	// 위도
	restaurant.latitude = [[dicRestaurant objectForKey:@"F_LATITUDE"] doubleValue];
	
	// 경도
	restaurant.longitude = [[dicRestaurant objectForKey:@"F_LONGITUDE"] doubleValue];
	
	// 분류주소
	restaurant.city = [dicRestaurant objectForKey:@"GUGUN"];
	
	// 주차공간
	restaurant.park = [self checkNullString:[dicRestaurant objectForKey:@"F_SCSPACE"]];
	
	// 좌석수
	restaurant.accomodation = [self checkNullString:[dicRestaurant objectForKey:@"F_SCSEAT"]];
	
	// 휴일
	restaurant.holyday = [self checkNullString:[dicRestaurant objectForKey:@"F_SHDAY"]];
	
	// 영업시간
	restaurant.time = [self checkNullString:[dicRestaurant objectForKey:@"F_STIME"]];
	
	// 차량지원
	restaurant.car_support = [self checkNullString:[dicRestaurant objectForKey:@"F_SCAR"]];
	
	// 예약가능
	restaurant.reservation = [self checkNullString:[dicRestaurant objectForKey:@"F_SCTYPE"]];
	
	// 기타1
	restaurant.etc1 = [self checkNullString:[dicRestaurant objectForKey:@"F_SEQ"]];
	
	// 기타2
	restaurant.etc2 = [self checkNullString:[dicRestaurant objectForKey:@"F_SETC"]];
	
	// 추천메뉴
	NSArray *arrayRecommendationMenu = (NSArray*)[dictionary objectForKey:@"TB_SFOOD_MENU"];

	if (arrayRecommendationMenu && [arrayRecommendationMenu count] > 0)
	{
		// 추천메뉴가 존재한다면 추천메뉴 객체를 생성하여 데이터를 설정한다.
		NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:[arrayRecommendationMenu count]];
		
		for (NSDictionary *dicRecommendationMenu in arrayRecommendationMenu)
		{
			if ([arrayRecommendationMenu lastObject] != dicRecommendationMenu)
			{
				RecommendationMenu *menu = [[RecommendationMenu alloc] init];
				menu.menu = [self checkNullString:[dicRecommendationMenu objectForKey:@"S_MENU"]];
				menu.price = [self checkNullString:[dicRecommendationMenu objectForKey:@"S_PRICE"]];
				
				// 뮤터블 배열에 추가
				[mutableArray addObject:menu];
				[menu release];				
			}			
		}
		
		// 맛집 데이터에 추천메뉴 추가
		restaurant.recommendationMenu = [NSArray arrayWithArray:mutableArray];
		[mutableArray release];
	}
	
	// 사용자 리뷰
	NSArray *arrayUserReview = (NSArray*)[dictionary objectForKey:@"review_list"];
	
	if (arrayUserReview && [arrayUserReview count] > 0)
	{
		// 추천메뉴가 존재한다면 추천메뉴 객체를 생성하여 데이터를 설정한다.
		NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:[arrayUserReview count]];
		
		for (NSDictionary *dicUserReview in arrayUserReview)
		{
			if ([arrayUserReview lastObject] != dicUserReview)
			{
				UserReview *review = [[UserReview alloc] init];
				review.phone = [self checkNullString:[dicUserReview objectForKey:@"F_TEL"]];
				review.review = [self checkNullString:[dicUserReview objectForKey:@"F_SCONTENT"]];
				review.star = [[dicUserReview objectForKey:@"F_RATE"] intValue];
				
				// 뮤터블 배열에 추가
				[mutableArray addObject:review];
				[review release];				
			}			
		}
		
		// 맛집 데이터에 추천메뉴 추가
		restaurant.reviewArray = [NSArray arrayWithArray:mutableArray];
		[mutableArray release];
	}
	
	// 사용자 평균 별점
	restaurant.starAverage = [[dictionary objectForKey:@"TB_StarScore"] intValue];
	
	// 사용자 리뷰 갯수
	restaurant.starCount = [[dictionary objectForKey:@"TB_StarCount"] intValue];
	
	// 문자열 별점
	restaurant.starString = [restaurant getStarStringFromStarAverage:restaurant.starAverage];
	
	return [restaurant autorelease];
}


//// 데이터베이스로부터 추천메뉴를 받아서 array에 넣는다.
//- (NSArray*)getRecommendationMenuArrayFromDatabaseByJSonParser:(NSString*)resultData
//{
//	// JSON 파서 생성
//	SBJsonParser *parser = [[SBJsonParser alloc] init];
//	
//	// 파싱을 위해서 파서에 데이터베이스 결과를 넣는다.
//	NSArray *dataArray = (NSArray*)[parser objectWithString:resultData];
//	
//	// 파서 릴리즈
//	[parser release];
//
//	if ([dataArray count] == 0)
//	{
//		return nil;
//	}
//	
//	
//	NSMutableArray *array = [[NSMutableArray alloc] init];
//	
//	for (NSDictionary *dictionary in dataArray)
//	{
//		// 추천메뉴 생성
//		RecommendationMenu *menu = [[RecommendationMenu alloc] init];
//		
//		// 데이터 넣기
//		menu.menu = [self checkNullString:[dictionary objectForKey:@"S_MENU"]];
//		menu.price = [self checkNullString:[dictionary objectForKey:@"S_PRICE"]];
//		
//		// array에 추가
//		[array addObject:menu];
//		
//		// 릴리즈
//		[menu release];
//	}
//	
//	// 리턴할 결과array
//	NSArray *resultArray = [[NSArray alloc] initWithArray:array];
//	[array release];
//	
//	// 결과값 리턴
//	return [resultArray autorelease];
//}


// 데이터베이스로부터 맛집 이미지에 관한 파일명 받아서 파싱하여 array에 넣는다.
- (NSString*)getImageNameFromRawData:(NSString*)rawData
{
	if (!rawData)
	{
		return nil;
	}
	
	if ( (![rawData isEqualToString:@"end"]) && (![rawData isEqualToString:@"(null)"]) && (![rawData isEqualToString:@""]))
	{
		// 이미지 파일명을 "|"으로 파싱한다. 첫번째 파싱이 이미지파일명이다.
		NSMutableString *imageFileName = [NSMutableString stringWithString:rawData];

		// "|"이 있는 문자열의 범위를 구한다.
		NSRange range =	[rawData rangeOfString:@"|"];
		
		// "|" 문자열 뒤의 범위를 구하여 원래 이미지파일 문자열에서 잘라낸다.
		range.length = [imageFileName length] - range.location;
		[imageFileName deleteCharactersInRange:range];
		
		return imageFileName;
	}
	
	return nil;
}


// 데이터베이스로부터 한줄평 리뷰에 관한 결과를 문자열로 가져온다.
- (NSString*)getUserReviewResultFromDatabaseByJSonParser:(NSString*)resultData
{
	// JSON 파서 생성
	SBJsonParser *parser = [[SBJsonParser alloc] init];
	
	// 파싱을 위해서 파서에 데이터베이스 결과를 넣는다.
	NSDictionary *dictionary = (NSDictionary*)[parser objectWithString:resultData];

	// 결과값 얻기
	NSString *resultString = (NSString*)[dictionary objectForKey:@"result"];
	
	// 파서 릴리즈
	[parser release];
	
	return resultString;
}


// 널문자인지 확인하는 메소드
- (NSString*)checkNullString:(NSString*)newString
{
	if ([newString isEqualToString:@"(null)"])
	{
		return @"";
	}
	else if ([newString isEqualToString:@"Y"])
	{
		return @"예";
	}
	else if ([newString isEqualToString:@"N"])
	{
		return @"아니오";
	}	
	
	return newString;
}


#pragma mark -
#pragma mark 데이터 전송시작
- (BOOL)startReceivingDataFromURL:(NSString*)dataURL
{
	NSURLRequest *req = [[NSURLRequest alloc] initWithURL:[NSURL URLWithString:dataURL]];
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:req delegate:self];
	[req release];
	
	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_DATA;
		
		return YES;
	}
	return NO;
}


// 맛집 array 데이터 전송시작
- (BOOL)startReceivingArrayRestaurantsFromArea:(NSString*)area
{
	// URL
	NSString *url_string = JBFOOD_DATABASE_URL_AREA;
	
	// post 명령 사용
	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
	[request setURL:[NSURL URLWithString:url_string]];
	[request setHTTPMethod:@"POST"];
	
	// body 작성
	NSMutableData *body = [NSMutableData data];
	[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&area=%@", area] dataUsingEncoding:-2147481280]];
	[request setHTTPBody:body];
	
	
	// 커넥션 시작
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];

	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_FROM_AREA;
		
		return YES;
	}
	return NO;	
}


// 상호명 검색 결과 맛집 array 데이터 전송시작
- (BOOL)startReceivingArrayRestaurantsFromName:(NSString*)name
{
	// URL
	NSString *url_string = JBFOOD_DATABASE_URL_NAME_MENU_TITLE;
	
	// post 명령 사용
	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
	[request setURL:[NSURL URLWithString:url_string]];
	[request setHTTPMethod:@"POST"];
	
	// body 작성
	NSMutableData *body = [NSMutableData data];
	[body appendData:[[NSString stringWithFormat:@"menu=%@&name=%@&title=%@", name, name, name] dataUsingEncoding:-2147481280]];
	[request setHTTPBody:body];
	
	
	// 커넥션 시작
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];
	
	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_FROM_NAME;
		
		return YES;
	}
	return NO;	
}


// 상황별 검색 결과 맛집 array 데이터 전송시작
- (BOOL)startReceivingArrayRestaurantsFromSituation:(SUBJECT_SECTION)section withParam:(NSString*)param
{
	// URL
	NSString *url_string = JBFOOD_DATABASE_URL_SITUATION;
		
	// post 명령 사용
	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
	[request setURL:[NSURL URLWithString:url_string]];
	[request setHTTPMethod:@"POST"];
	
	// body 작성
	NSMutableData *body = [NSMutableData data];

	switch (section)
	{
		case SUBJECT_THEME:
		{
			[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&class=F_SPART&param=%@", param] dataUsingEncoding:-2147481280]];
		}	break;
			
		case SUBJECT_MOOD:
		{
			[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&class=F_SATM&param=%@", param] dataUsingEncoding:-2147481280]];
		}	break;			
			
		case SUBJECT_INTERIA:
		{
			[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&class=F_SINTE&param=%@", param] dataUsingEncoding:-2147481280]];
		}	break;
			
		case SUBJECT_AGE:
		{
			[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&class=F_SAGE&param=%@", param] dataUsingEncoding:-2147481280]];
		}	break;
			
		case SUBJECT_PRICE:
		{
			[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&class=F_SPRICE&param=%@", param] dataUsingEncoding:-2147481280]];
		}	break;			
	}
	
	[request setHTTPBody:body];
	
	
	// 커넥션 시작
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];
	
	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_FROM_SITUATION;
		
		return YES;
	}
	return NO;
}


// 맛집 데이터 전송시작
- (BOOL)startReceivingRestaurantFromId:(int)id
{
	NSString *idString = [NSString stringWithFormat:@"%d", id];
	
	// post 명령 사용
	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
	[request setURL:[NSURL URLWithString:JBFOOD_DATABASE_URL_DATA]];
	[request setHTTPMethod:@"POST"];
	
	// body 작성
	NSMutableData *body = [NSMutableData data];
	[body appendData:[[NSString stringWithFormat:@"table=TB_SFOOD&mshopid=%@", idString] dataUsingEncoding:-2147481280]];
	[request setHTTPBody:body];

	// 커넥션 시작
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];
	
	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_FROM_ID;
		
		return YES;
	}
	return NO;	
}


//// 추천메뉴 전송시작
//- (BOOL)startReceivingRecommendationMenuFromId:(int)id
//{
//	NSString *idString = [NSString stringWithFormat:@"%d", id];
//	
//	// post 명령 사용
//	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
//	[request setURL:[NSURL URLWithString:JBFOOD_DATABASE_URL_RECOMMENDATION_MENU]];
//	[request setHTTPMethod:@"POST"];
//	
//	// body 작성
//	NSMutableData *body = [NSMutableData data];
//	[body appendData:[[NSString stringWithFormat:@"param=%@&param2=tb_sfood", idString] dataUsingEncoding:-2147481280]];
//	[request setHTTPBody:body];
//	
//	// 커넥션 시작
//	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];
//	
//	if (con)
//	{
//		// 데이터 객체 만들기
//		[self makeMutableData];
//		
//		// 커넥션 타입 설정.
//		self.connectionType = CONNECTION_MENU_FROM_ID;
//		
//		return YES;
//	}
//	return NO;	
//}


// 모든 맛집 전송시작
- (BOOL)startReceivingAllRestaurants
{
	//NSString *param = @"";
	
	// URL
	NSString *url_string = JBFOOD_DATABASE_URL_ALL_RESTURANT;
	
	// post 명령 사용
	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
	[request setURL:[NSURL URLWithString:url_string]];
	[request setHTTPMethod:@"POST"];
	
	// body 작성
	NSMutableData *body = [NSMutableData data];
	//[body appendData:[[NSString stringWithFormat:@"area=%@&parser=JSON", param] dataUsingEncoding:-2147481280]];
	[request setHTTPBody:body];
	
	
	// 커넥션 시작
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];
	
	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_ALL_RESTAURANT;
		
		return YES;
	}
	return NO;
}


// 한줄평 리뷰 전송시작
- (BOOL)startSendingUserReivew:(NSUInteger)id Phone:(NSString*)phoneNumber Review:(NSString*)review Star:(NSUInteger)star
{
	NSString *idString = [NSString stringWithFormat:@"%d", id];
	NSString *starString = [NSString stringWithFormat:@"%d", star];
	
	NSString *url_string = JBFOOD_DATABASE_URL_USER_REVIEW;
	
	// post 명령 사용
	NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
	[request setURL:[NSURL URLWithString:url_string]];
	[request setHTTPMethod:@"POST"];
	
	// body 작성
	NSMutableData *body = [NSMutableData data];
	[body appendData:[[NSString stringWithFormat:@"table=TB_MSHOP_TOK&passwd=71004&mshopid=%@&F_RATE=%@&F_SCONTENT=%@&F_STEL=%@", idString, starString, review, phoneNumber] dataUsingEncoding:-2147481280]];
	[request setHTTPBody:body];
	
	
	// 커넥션 시작
	NSURLConnection *con = [[NSURLConnection alloc] initWithRequest:request delegate:self];
	
	if (con)
	{
		// 데이터 객체 만들기
		[self makeMutableData];
		
		// 커넥션 타입 설정.
		self.connectionType = CONNECTION_USER_REVIEW_WRITE;
		
		return YES;
	}
	return NO;	
}


- (void)makeMutableData
{
	// 기존에 데이터객체가 있다면 삭제한다.
	if (self.receivedData)
	{
		self.receivedData = nil;
	}
	
	// 새로 생성한다.
	NSMutableData *data = [[NSMutableData alloc] init];
	self.receivedData = data;
	[data release];
}

#pragma mark -
#pragma mark 델리게이트 삭제
- (void)removeAllDelegateReferenceToNil
{
	// 데이터전송 델리게이트
	self.receiveDataDelegate = nil;
	
	// 맛집 array 델리게이트
	self.arrayRestaurantsFromAreaDelegate = nil;
	
	// 맛집 array 델리게이트
	self.arrayRestaurantsFromNameDelegate = nil;
	
	// 맛집 array 델리게이트
	self.arrayRestaurantsFromSituationDelegate = nil;
	
	// 맛집 데이터 델리게이트
	self.restaurantFromIdDelegate = nil;
	
	// 맛집의 추천메뉴 델리게이트
	//self.recommendationMenuFromIdDelegate = nil;
	
	// 모든 맛집 데이터 델리게이트
	self.allRestaurantsDelegate = nil;
	
	// 한줄평 리뷰 데이터 델리게이트
	self.userReviewDelegate = nil;
}


#pragma mark -
#pragma mark NSURLConnection 콜백메소드
// 이 콜백메소드가 호출되면 DatabaseConnector 델리게이트메소드를 호출한다.
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{	
	switch (self.connectionType)
	{
		case CONNECTION_DATA:
		{
			[self.receiveDataDelegate didReceive:self withResponse:response];
		}	break;
			
		case CONNECTION_FROM_AREA:
		{
			[self.receivedData setLength:0];
		}	break;
			
		case CONNECTION_FROM_ID:
		{
			[self.receivedData setLength:0];
		}	break;
			
		case CONNECTION_FROM_NAME:
		{
			[self.receivedData setLength:0];
		}	break;
			
		case CONNECTION_FROM_SITUATION:
		{
			[self.receivedData setLength:0];
		}	break;			
			
//		case CONNECTION_MENU_FROM_ID:
//		{
//			[self.receivedData setLength:0];
//		}	break;
			
		case CONNECTION_ALL_RESTAURANT:
		{
			[self.receivedData setLength:0];
		}	break;
			
		case CONNECTION_USER_REVIEW_WRITE:
		{
			[self.receivedData setLength:0];
		}	break;
	}
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{	
	switch (self.connectionType)
	{
		case CONNECTION_DATA:
		{
			[self.receiveDataDelegate didReceivingData:self withData:data];
		}	break;
			
		case CONNECTION_FROM_AREA:
		{
			[self.receivedData appendData:data];
			[self.arrayRestaurantsFromAreaDelegate arrayDidReceivingData:self withData:data];
		}	break;
			
		case CONNECTION_FROM_ID:
		{
			[self.receivedData appendData:data];
			[self.restaurantFromIdDelegate restaurantDidReceivingData:self withData:data];
		}	break;
			
		case CONNECTION_FROM_NAME:
		{
			[self.receivedData appendData:data];
			[self.arrayRestaurantsFromNameDelegate arrayDidReceivingData:self withData:data];
		}	break;
			
		case CONNECTION_FROM_SITUATION:
		{
			[self.receivedData appendData:data];
			[self.arrayRestaurantsFromSituationDelegate arrayDidReceivingData:self withData:data];
		}	break;
			
//		case CONNECTION_MENU_FROM_ID:
//		{
//			[self.receivedData appendData:data];
//			[self.recommendationMenuFromIdDelegate recommendationMenuDidReceivingData:self withData:data];
//		}	break;
			
		case CONNECTION_ALL_RESTAURANT:
		{
			[self.receivedData appendData:data];
			[self.allRestaurantsDelegate allRestaurantsDidReceivingData:self withData:data];
		}	break;
			
		case CONNECTION_USER_REVIEW_WRITE:
		{
			[self.receivedData appendData:data];
			[self.userReviewDelegate sendUserReviewDidReceivingData:self withData:data];
		}	break;
	}
	
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	[connection release];
	
	switch (self.connectionType)
	{
		case CONNECTION_DATA:
		{
			[self.receiveDataDelegate didFail:self withError:error];
		}	break;
			
		case CONNECTION_FROM_AREA:
		{
			[self.arrayRestaurantsFromAreaDelegate arrayDidFail:self withError:error];
		}	break;
			
		case CONNECTION_FROM_ID:
		{
			[self.restaurantFromIdDelegate restaurantDidFail:self withError:error];
		}	break;
			
		case CONNECTION_FROM_NAME:
		{
			[self.arrayRestaurantsFromNameDelegate arrayDidFail:self withError:error];
		}	break;			
			
		case CONNECTION_FROM_SITUATION:
		{
			[self.arrayRestaurantsFromSituationDelegate arrayDidFail:self withError:error];
		}	break;
			
//		case CONNECTION_MENU_FROM_ID:
//		{
//			[self.recommendationMenuFromIdDelegate recommendationMenuDidFail:self withError:error];
//		}	break;

		case CONNECTION_ALL_RESTAURANT:
		{
			[self.allRestaurantsDelegate allRestaurantsDidFail:self withError:error];
		}	break;

		case CONNECTION_USER_REVIEW_WRITE:
		{
			[self.userReviewDelegate sendUserReviewDidFail:self withError:error];
		}	break;
	}
}


- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	[connection release];
	
	switch (self.connectionType)
	{
		case CONNECTION_DATA:
		{
			[self.receiveDataDelegate didFinishReceivingData:self];
		}	break;
			
		case CONNECTION_FROM_AREA:
		{
			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
			NSMutableArray *array = [self getRestaurantsArrayFromDatabaseByJSonParser:dataString];
			[dataString release];
			[self.arrayRestaurantsFromAreaDelegate arrayDidFinish:self withArrayRestaurants:array];
		}	break;
			
		case CONNECTION_FROM_ID:
		{
			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
			Restaurant *restaurant = [self getRestaurantDataByJSonParser:dataString];
			[dataString release];
			[self.restaurantFromIdDelegate restaurantDidFinish:self withRestaurant:restaurant];
		}	break;
			
		case CONNECTION_FROM_NAME:
		{
			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
			NSMutableArray *array = [self getRestaurantsArrayFromDatabaseByJSonParser:dataString];
			[dataString release];
			[self.arrayRestaurantsFromNameDelegate arrayDidFinish:self withArrayRestaurants:array];
		}	break;			
			
		case CONNECTION_FROM_SITUATION:
		{
			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
			NSMutableArray *array = [self getRestaurantsArrayFromDatabaseByJSonParser:dataString];
			[dataString release];
			[self.arrayRestaurantsFromSituationDelegate arrayDidFinish:self withArrayRestaurants:array];
		}	break;
			
//		case CONNECTION_MENU_FROM_ID:
//		{
//			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
//			NSArray *array = [self getRecommendationMenuArrayFromDatabaseByJSonParser:dataString];
//			[dataString release];
//			[self.recommendationMenuFromIdDelegate recommendationMenuDidFinish:self withMenuArray:array];
//		}	break;
			
		case CONNECTION_ALL_RESTAURANT:
		{
			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
			NSMutableArray *array = [self getRestaurantsArrayFromDatabaseByJSonParser:dataString];
			[dataString release];
			[self.allRestaurantsDelegate allRestaurantsDidFinish:self withArray:array];
		}	break;
			
		case CONNECTION_USER_REVIEW_WRITE:
		{
			NSString *dataString = [[NSString alloc] initWithData:self.receivedData encoding:-2147481280];
			[self.userReviewDelegate sendUserReviewDidFinish:self withResult:[self getUserReviewResultFromDatabaseByJSonParser:dataString]];
			[dataString release];
		}	break;
	}
}


@end

