#import "RestaurantInfoTabbarController.h"
#import "SearchByAreaTableViewController.h"		// 지역별 맛집 테이블 컨트롤러
#import "SearchBySurroundingRootController.h"	// 주변 맛집 루트뷰 컨트롤러
#import "SearchMainViewController.h"			// 검색 메인 뷰 컨트롤러
#import "FavoriteResultTableViewController.h"	// 단골 맛집 뷰 컨트롤러


@implementation RestaurantInfoTabbarController


- (id)init
{
	if (self = [super init])
	{
		self.delegate = self;
		[self makeAllViewController];
	}
	
	return self;
}


- (void)viewDidLoad
{
	
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
	
}

- (void)viewDidUnload 
{
    [super viewDidUnload];
	
}


- (void)dealloc 
{
	
    [super dealloc];
}


// 탭바에 들어가 모든 뷰컨트롤러를 생성한다.
- (void)makeAllViewController
{	
	UIViewController *viewController[TABBAR_TOTAL_COUNT];
	NSMutableArray *arrayForViewController = [[NSMutableArray alloc] initWithCapacity:TABBAR_TOTAL_COUNT];
	
	NSUInteger index;
	for (index = 0 ; index < TABBAR_TOTAL_COUNT ; ++index)
	{
		viewController[index] = [self makeViewControllerForTabbar:index];
		[arrayForViewController addObject:viewController[index]];
	}
	
	self.viewControllers = [NSArray arrayWithArray:arrayForViewController];
	[arrayForViewController release];
}


// 각 뷰 컨트롤러 생성
- (UIViewController*)makeViewControllerForTabbar:(RESTAURANT_INFO_TABBAR_LIST)viewType
{
	UIViewController *makedViewController;
	
	// 타입에 맞게 뷰컨트롤러 생성
	switch (viewType)
	{
			
		case TABBAR_SERACH_BY_AREA:				// 지역별 맛집
		{
			SearchByAreaTableViewController *viewController = [[SearchByAreaTableViewController alloc] init];
			UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
			viewController.tabBarItem.image = [UIImage imageNamed:TABBAR_SERACH_BY_AREA_IMAGE_NAME];
			
			makedViewController = naviController;
		}	break;
			
		case TABBAR_SERACH_BY_SURROUNDING:		// 주변 맛집
		{
			SearchBySurroundingRootController *viewController = [[SearchBySurroundingRootController alloc] initWithNibName:@"SearchBySurroundingRootController" bundle:nil];
			UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
			viewController.tabBarItem.image = [UIImage imageNamed:TABBAR_SERACH_BY_SURROUNDING_IMAGE_NAME];
			
			makedViewController = naviController;
		}	break;
			
		case TABBAR_SERACH_MAIN:				// 검색 하기
		{
			SearchMainViewController *viewController = [[SearchMainViewController alloc] initWithNibName:@"SearchMainViewController" bundle:nil];
			UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
			viewController.tabBarItem.image = [UIImage imageNamed:TABBAR_SERACH_MAIN_IMAGE_NAME];		

			makedViewController = naviController;
		}	break;
			
		case TABBAR_FAVORITE:					// 단골 맛집
		{
			FavoriteResultTableViewController *viewController = [[FavoriteResultTableViewController alloc] init];
			UINavigationController *naviController = [[UINavigationController alloc] initWithRootViewController:viewController];
			viewController.tabBarItem.image = [UIImage imageNamed:TABBAR_FAVORITE_IMAGE_NAME];
			
			makedViewController = naviController;
		}	break;
			
		case TABBAR_HOME:						// 메인으로 가기
		{
			UIViewController *viewController = [[UIViewController alloc] init];
			viewController.title = @"홈";
			viewController.tabBarItem.image = [UIImage imageNamed:TABBAR_HOME_IMAGE_NAME];		

			makedViewController = viewController;
		}	break;
	}
	
	return makedViewController;
}


- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
	// 메인으로 이동
	if (self.selectedIndex == TABBAR_HOME)
	{
		[self dismissModalViewControllerAnimated:YES];
	}
	// 선택된 뷰컨트롤러를 다시 생성 - 이유는 뷰컨트롤러의 맨 상위로 가기 위해 다시 만든다.
	else
	{
		// 선택된 인덱스 값
		NSUInteger index = self.selectedIndex;
		
		// 탭바의 뷰컨트롤러 배열을 뮤터블배멸에 재 저장.
		NSMutableArray *mutableArray = [NSMutableArray arrayWithArray:self.viewControllers];
		
		// 선택된 뷰컨트롤러 삭제R
		[mutableArray removeObjectAtIndex:index];
		
		// 인덱스번호에 해당하는 뷰 컨트롤러를 재생성하여 다시 넣는다.
		[mutableArray insertObject:[self makeViewControllerForTabbar:self.selectedIndex] atIndex:index];
		
		// 탭바의 뷰컨트롤러를 삭제
		self.viewControllers = nil;
		
		// 배열로 만들어 탭바의 뷰컨트롤러로 넣는다.
		self.viewControllers = [NSArray arrayWithArray:mutableArray];

		// 뷰컨트롤러 배열이 삭제되었으므로 다시 선택번호를 설정한다.
		self.selectedIndex = index;
		
		// 뷰 애니메이션 활성화
		[UIView beginAnimations:nil context:NULL];
		[UIView setAnimationDuration:0.5f];
		[UIView setAnimationTransition:UIViewAnimationTransitionFlipFromRight forView:self.view cache:YES];
		[UIView commitAnimations];
	}
}


@end
