#import <UIKit/UIKit.h>

#import "DatabaseConnector.h"

@interface DatabaseTestViewController : UIViewController
<
receiveArrayRestaurantsFromAreaDelegate, 
receiveRestaurantFromIdDelegate, 
receiveArrayRestaurantsFromNameDelegate,
receiveArrayRestaurantsFromSituationDelegate,
//receiveRecommendationMenuFromIdDelegate,
receiveAllRestaurantsDelegate,
sendUserReviewDelegate
>
{
	UIButton *button0;
	UIButton *button1;
	UIButton *button2;
	UIButton *button3;
	UIButton *button4;
	UIButton *button5;
	UIButton *button6;	
	UITextField *textField;
	UITextView *textView;
	UIImageView *imageView;
	
	DatabaseConnector *dbConnector;
	NSMutableData *receivedData;
}

@property (nonatomic, retain)	IBOutlet	UIButton *button0;
@property (nonatomic, retain)	IBOutlet	UIButton *button1;
@property (nonatomic, retain)	IBOutlet	UIButton *button2;
@property (nonatomic, retain)	IBOutlet	UIButton *button3;
@property (nonatomic, retain)	IBOutlet	UIButton *button4;
@property (nonatomic, retain)	IBOutlet	UIButton *button5;
@property (nonatomic, retain)	IBOutlet	UIButton *button6;
@property (nonatomic, retain)	IBOutlet	UITextField *textField;
@property (nonatomic, retain)	IBOutlet	UITextView *textView;
@property (nonatomic, retain)	IBOutlet	UIImageView *imageView;
@property (nonatomic, retain)	DatabaseConnector *dbConnector;
@property (nonatomic, retain)	NSMutableData *receivedData;

- (IBAction)touchButtons:(id)sender;

@end
