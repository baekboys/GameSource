#import <UIKit/UIKit.h>


#define kSearchByAreaTableCellHeight 50


@interface SearchByAreaTableCell : UITableViewCell 
{
	UIImageView		*areaImageView;
	UILabel			*nameLabel;
}


@property (nonatomic, retain) IBOutlet UIImageView					*areaImageView;
@property (nonatomic, retain) IBOutlet UILabel						*nameLabel;


@end
