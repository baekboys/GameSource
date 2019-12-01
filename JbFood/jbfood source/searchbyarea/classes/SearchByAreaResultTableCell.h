#import <UIKit/UIKit.h>


#define kSearchByAreaResultTableCellHieght 60


@interface SearchByAreaResultTableCell : UITableViewCell 
{
	UIImageView		*restaurantImageView;
	UILabel			*nameLabel;
	UILabel			*menuLabel;
	UILabel			*starLabel;
}

@property (nonatomic, retain) IBOutlet UIImageView		*restaurantImageView;
@property (nonatomic, retain) IBOutlet UILabel			*nameLabel;
@property (nonatomic, retain) IBOutlet UILabel			*menuLabel;
@property (nonatomic, retain) IBOutlet UILabel			*starLabel;
@end
