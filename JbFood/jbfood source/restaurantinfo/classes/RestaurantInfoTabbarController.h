#import <UIKit/UIKit.h>


//	탭바 구성 열거자
typedef enum
{
	TABBAR_SERACH_BY_AREA,				// 지역별 맛집
	TABBAR_SERACH_BY_SURROUNDING,		// 주변 맛집
	TABBAR_SERACH_MAIN,					// 검색하기
	TABBAR_FAVORITE,					// 단골 맛집
	TABBAR_HOME,						// 홈으로
	TABBAR_TOTAL_COUNT
}	RESTAURANT_INFO_TABBAR_LIST;


//	탭바이미지 파일명
#define TABBAR_SERACH_BY_AREA_IMAGE_NAME		@"tabbar1.png"
#define TABBAR_SERACH_BY_SURROUNDING_IMAGE_NAME	@"tabbar2.png"
#define TABBAR_SERACH_MAIN_IMAGE_NAME			@"tabbar3.png"
#define TABBAR_FAVORITE_IMAGE_NAME				@"tabbar4.png"
#define TABBAR_HOME_IMAGE_NAME					@"tabbar5.png"


@interface RestaurantInfoTabbarController : UITabBarController <UITabBarControllerDelegate>
{

}

// 탭바에 들어가 모든 뷰컨트롤러를 생성한다.
- (void)makeAllViewController;
// 각 뷰 컨트롤러 생성
- (UIViewController*)makeViewControllerForTabbar:(RESTAURANT_INFO_TABBAR_LIST)viewType;

@end
