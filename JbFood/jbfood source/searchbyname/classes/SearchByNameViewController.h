#import <UIKit/UIKit.h>
#import "DatabaseConnector.h"


@interface SearchByNameViewController : UIViewController 
<
UISearchBarDelegate,
UIAlertViewDelegate,
receiveArrayRestaurantsFromNameDelegate
>
{
	// UI
	UISearchBar					*nameSearchBar;
	UIAlertView					*alertViewForDB;
	UIActivityIndicatorView		*indicatorView;
	
	// 데이터베이스 커넥터
	DatabaseConnector			*dbConnector;	
}


@property (nonatomic, retain) IBOutlet	UISearchBar					*nameSearchBar;
@property (nonatomic, retain)			UIAlertView					*alertViewForDB;
@property (nonatomic, retain)			UIActivityIndicatorView		*indicatorView;
@property (nonatomic, retain)			DatabaseConnector			*dbConnector;


#pragma mark -
#pragma mark 알람뷰 메소드
// 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;


@end

