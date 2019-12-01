#import <UIKit/UIKit.h>


@interface JBFoodViewController : UIViewController
{
	// 맛집정보 버튼
	UIButton *button_foodInfo;
	
	// 전북 음식문화 프라자
	UIButton *button_culturePlaza;
	
	// 네이버카페 버튼
	UIButton	*button_naverCafe;

	// 타이틀
	UILabel *titleLabel;
	
	// 개발자 정보 레이블
	UILabel *infoLabel;
}

#pragma mark -
#pragma mark property
@property (nonatomic, retain)	IBOutlet	UIButton	*button_foodInfo;
@property (nonatomic, retain)	IBOutlet	UIButton	*button_culturePlaza;
@property (nonatomic, retain)	IBOutlet	UIButton	*button_naverCafe;
@property (nonatomic, retain)	IBOutlet	UILabel		*infoLabel;
@property (nonatomic, retain)	IBOutlet	UILabel		*titleLabel;

#pragma mark -
#pragma mark 버튼 이벤트
- (IBAction)touchButton:(id)sender;
- (IBAction)touchInfoButton:(id)sender;

@end

