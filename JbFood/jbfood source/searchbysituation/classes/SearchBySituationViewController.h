#import <UIKit/UIKit.h>
#import "SHARED_DEFINE.h"
#import "DatabaseConnector.h"

// -------------------------------------------- 
//	SearchBySituation을 위한 주제별 딕셔너리 파일명
// --------------------------------------------
#define kSearchBySituationTypeListFileName		@"SearchBySituationTypeList"

// 주제별 피커뷰의 구성
typedef enum
{
	SITUATION_PICKER_TYPE_MAIN,
	SITUATION_PICKER_TYPE_SUB,
	SITUATION_PICKER_TYPE_TOTAL_COUNT
}	SITUATION_PICKER_TYPE;


@interface SearchBySituationViewController : UIViewController
<
UIPickerViewDelegate, UIPickerViewDataSource,
UIAlertViewDelegate,
receiveArrayRestaurantsFromSituationDelegate
>
{
	// UI
	UIPickerView				*situationPickerView;
	UIButton					*searchButton;
	UIAlertView					*alertViewForDB;
	UIActivityIndicatorView		*indicatorView;
	
	// 데이터베이스 커넥터
	DatabaseConnector			*dbConnector;
	
	
	// 주제별 분류 딕셔너리
	NSDictionary				*situationDictionary;
	
	// 주제별 분류 딕셔너리의 전체 키 array
	NSArray						*situationKeyArray;
	
	// 현재 선택된 메인딕셔너리의 서브딕셔너리
	NSDictionary				*selectedDictionary;
	
	// 현재 선택된 메인딕셔너리의 서브딕셔너리
	NSArray						*selectedKeyArray;
}


@property (nonatomic, retain) IBOutlet	UIPickerView				*situationPickerView;
@property (nonatomic, retain) IBOutlet	UIButton					*searchButton;
@property (nonatomic, retain)			UIAlertView					*alertViewForDB;
@property (nonatomic, retain)			UIActivityIndicatorView		*indicatorView;

@property (nonatomic, retain)			DatabaseConnector			*dbConnector;

@property (nonatomic, retain)			NSDictionary				*situationDictionary;
@property (nonatomic, retain)			NSArray						*situationKeyArray;
@property (nonatomic, retain)			NSDictionary				*selectedDictionary;
@property (nonatomic, retain)			NSArray						*selectedKeyArray;


#pragma mark -
#pragma mark 버튼 이벤트
- (IBAction)touchSearchButton:(id)sender;

#pragma mark -
#pragma mark 주제별 열거자 가져오기
- (SUBJECT_SECTION)getSubjectSectionFromString:(NSString*)type;


#pragma mark -
#pragma mark 알람뷰 생성 메소드
// 알람뷰 생성 - 주제별 딕셔너리 생성실패
- (void)makeAlertViewForDictionaryFail;
// 알람뷰 생성하기
- (void)makeAlertView:(NSString*)title;


@end
