#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>

#pragma mark -
#pragma mark define
#import "SHARED_DEFINE.h"							// 지역별 맛집위해 헤더파일 추가
#import "DatabaseConnector.h"						// 맛집 데이터베이스 연동을 위한 클래스


#define kSearchByAreaTableViewFontSize		25		// 테이블뷰의 폰트 사이즈

#define kJeonbukLatitude					35.718643
#define kJeonbukLongitude					127.154388
#define kJeonbukInitSpan					79500

#define kJeonbukAreaPlistFileName			@"SearchByAreaTypeList"

@interface SearchByAreaTableViewController : UIViewController
<receiveArrayRestaurantsFromAreaDelegate, UIAlertViewDelegate, UITableViewDelegate, UITableViewDataSource, MKMapViewDelegate>
{
	UITableView					*areaTableView;
	UIAlertView					*alertViewForDB;
	UIActivityIndicatorView		*indicatorView;
	MKMapView					*areaMapView;

	NSString					*areaName;
	DatabaseConnector			*dbConnector;
	
	// 지역별 분류 딕셔너리
	NSDictionary				*areaDictionary;
	
	// 지역별 이름 array
	NSArray						*areaArray;
	
	// 지역별 annotation
	NSMutableArray				*areaAnnotaionArray;
}


@property (nonatomic, retain)	UITableView					*areaTableView;
@property (nonatomic, retain)	UIAlertView					*alertViewForDB;
@property (nonatomic, retain)	UIActivityIndicatorView		*indicatorView;
@property (nonatomic, retain)	MKMapView					*areaMapView;
@property (nonatomic, copy)		NSString					*areaName;
@property (nonatomic, retain)	DatabaseConnector			*dbConnector;
@property (nonatomic, retain)	NSDictionary				*areaDictionary;
@property (nonatomic, retain)	NSArray						*areaArray;
@property (nonatomic, retain)	NSMutableArray				*areaAnnotaionArray;

#pragma mark -
#pragma mark 테이블뷰 구현 메소드
// SHARED_DEFINE에 정의된 지역별 맛집 열거자를 넣어 지역이름 얻어오기
- (NSString*)getAreaNameFromAREALIST:(AREA_LIST)list;
// 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;


#pragma mark -
#pragma mark 맵뷰 구현 메소드
// 전북지역을 맵뷰에 추가하기 - self.areaArray의 인덱스번호를 통하여 차후에 모든 이벤트를 전달한다.
- (void)addMapLocationWithArea:(NSInteger)areaIndex withCoord:(CLLocationCoordinate2D)newCoordiate withImageFileName:(NSString*)newFileName;
// 지역명을 눌렀을때 이벤트 처리 메소드
- (void)touchButtonOfAnnotation:(id)sender;

#pragma mark -
#pragma mark 별점 정렬 메소드
NSInteger sortByStarAvarage(id restaurant1, id restaurant2, void *context);

@end
