#import <UIKit/UIKit.h>


@interface UserReviewResultTableViewController : UITableViewController 
{
	// 테이블에 그려질 한줄평의 array
	NSArray *arrayUserReviews;
}

@property (nonatomic, retain) NSArray *arrayUserReviews;


// 한줄평 뮤터블 배열로 초기화
- (id)iniWithArrayUserReviews:(NSArray*)newArrayUserReviews;

// 평균 별점으로부터 문자열 별점으로 가져오기
- (NSString*)getStarStringFromStar:(NSUInteger)newStarAverage;


@end
