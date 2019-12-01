#import <UIKit/UIKit.h>


// 셀 식별자
#define InfoCellIdentifier	@"InfoCellIdentifier"

// 셀의 높이
#define kInfoCellHeight		50

@interface InfoCell : UITableViewCell 
{
	//UIImageView		*titleImageView;
	UILabel			*label_title;
	UILabel			*label_contents;
}


//@property (nonatomic, retain) IBOutlet	UIImageView		*titleImageView;
@property (nonatomic, retain) IBOutlet	UILabel			*label_title;
@property (nonatomic, retain) IBOutlet	UILabel			*label_contents;


@end
