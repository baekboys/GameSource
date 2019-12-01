#import <UIKit/UIKit.h>
#import "ARViewController.h"
#import "MultiMapViewController.h"

@interface SearchBySurroundingRootController : UIViewController <ARViewControllerDelegate, MultiMapViewControllerDelegate>
{
	UIButton					*button_map;
	UIButton					*button_ar;	
}

@property (nonatomic, retain) IBOutlet	UIButton					*button_map;
@property (nonatomic, retain) IBOutlet	UIButton					*button_ar;

- (IBAction)touchButton:(id)sender;

@end
