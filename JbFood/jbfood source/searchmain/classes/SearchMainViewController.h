#import <UIKit/UIKit.h>


@interface SearchMainViewController : UIViewController 
{
	UIButton		*nameSearchButton;
	UIButton		*situationSearchButton;
}


@property (nonatomic, retain) IBOutlet	UIButton		*nameSearchButton;
@property (nonatomic, retain) IBOutlet	UIButton		*situationSearchButton;

// 터치 이벤트 메소드
- (IBAction)touchButton:(id)sender;

@end
