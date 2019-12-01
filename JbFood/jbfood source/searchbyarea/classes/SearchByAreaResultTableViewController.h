#import <UIKit/UIKit.h>
#import "DatabaseConnector.h"

#define kSearchByResultTableViewNameFontSize		22		// 테이블뷰의 폰트 사이즈
#define kSearchByResultTableViewMenuFontSize		13		// 테이블뷰의 폰트 사이즈


@interface SearchByAreaResultTableViewController : UIViewController 
<receiveRestaurantFromIdDelegate, UIAlertViewDelegate, UITableViewDelegate, UITableViewDataSource>
{
	// UI
	UITableView					*areaResultTableView;
	UIActivityIndicatorView		*indicatorView;
	
	// 데이터베이스 연결 관련
	UIAlertView					*alertViewForDB;
	NSString					*areaName;
	DatabaseConnector			*dbConnector;
		
	// 테이블에 그려질 맛집(대략적인 정보)의 array
	NSMutableArray *arrayRestaurants;
	
	// 맛집뷰가 단골맛집 버튼을 생성할지 결정하는 플래그
	BOOL						makeJoinButtonForRestaurantView;
}


@property (nonatomic, retain)	UITableView					*areaResultTableView;
@property (nonatomic, retain)	UIActivityIndicatorView		*indicatorView;
@property (nonatomic, retain)	NSMutableArray				*arrayRestaurants;
@property (nonatomic, retain)	UIAlertView					*alertViewForDB;
@property (nonatomic, copy)		NSString					*areaName;
@property (nonatomic, retain)	DatabaseConnector			*dbConnector;
@property (nonatomic, assign)	BOOL						makeJoinButtonForRestaurantView;


#pragma mark -
#pragma mark 초기화 메소드
- (id)initWithArrayRestaurants:(NSMutableArray*)newArrayRestaurants withAreaName:(NSString*)newName;


#pragma mark -
#pragma mark 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;


#pragma mark -
#pragma mark 이름순으로 정렬 메소드
- (void)sortingArrayRestaurantsByName;

@end
