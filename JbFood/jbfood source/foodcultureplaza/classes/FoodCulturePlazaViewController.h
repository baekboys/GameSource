#import <UIKit/UIKit.h>

// 전북 음식문화 플라자 URL
#import "SHARED_DEFINE.h"

#pragma mark -
#pragma mark define

// 종료이벤트 델리게이트 선언
@protocol FoodCulturePlazaViewControllerDelegate;


@interface FoodCulturePlazaViewController : UIViewController
{
	// 종료 이벤트 델리게이트
	id <FoodCulturePlazaViewControllerDelegate>	delegate;
	
	// 웹뷰
	UIWebView			*webView;
	
	// 메인으로 가기 버튼
	UIBarButtonItem		*barButton_main;
	
	// 연결할 URL
	NSString			*urlString;
}

#pragma mark -
#pragma mark property
@property (nonatomic, assign) id <FoodCulturePlazaViewControllerDelegate> delegate;
@property (nonatomic, retain) IBOutlet	UIWebView			*webView;
@property (nonatomic, retain) IBOutlet	UIBarButtonItem		*barButton_main;
@property (nonatomic, copy)				NSString			*urlString;


#pragma mark -
#pragma mark 초기화
-(id)initWithURL:(NSString*)newURLString withTitle:(NSString*)newTitle;



#pragma mark -
#pragma mark 버튼 이벤트
- (IBAction)touchButton:(id)sender;


@end


@protocol FoodCulturePlazaViewControllerDelegate
// 뷰 컨트롤러 종료 델리게이트 메소드
- (void)FoodCulturePlazaViewControllerDidFinish:(FoodCulturePlazaViewController *)controller;
@end


