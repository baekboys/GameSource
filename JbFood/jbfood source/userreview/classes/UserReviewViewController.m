#import "UserReviewViewController.h"
#import "SHARED_DEFINE.h"
#import "Restaurant.h"

@implementation UserReviewViewController


@synthesize label_star;
@synthesize label_review;
@synthesize button_star0;
@synthesize button_star1;
@synthesize button_star2;
@synthesize button_star3;
@synthesize button_star4;
@synthesize label_star0;
@synthesize label_star1;
@synthesize label_star2;
@synthesize label_star3;
@synthesize label_star4;
@synthesize textField_review;
@synthesize button_send;
@synthesize arrayLabels;
@synthesize indicatorView;
@synthesize starValue;
@synthesize restaurantId;
@synthesize dbConnector;
@synthesize userReviewWriteDelegate;

// 초기화 코드
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withRestaurantId:(NSUInteger)newRestaurantId
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) 
	{
		self.title = @"맛집 평가";
		
		self.starValue = 1;
		self.restaurantId = newRestaurantId;
		self.dbConnector = nil;
		self.userReviewWriteDelegate = nil;
    }
	
    return self;
}


- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	NSArray *array = [[[NSArray alloc] initWithObjects:
					  self.label_star0,
					  self.label_star1,
					  self.label_star2,
					  self.label_star3,
					  self.label_star4,
					  nil
					  ] autorelease];
	self.arrayLabels = array;
	
	// 배경이미지 설정
	UIImage *image = [UIImage imageNamed:JBFOOD_VIEW_BG_FILENAME];
	self.view.backgroundColor = [UIColor colorWithPatternImage:image];
	
	// 레이블 폰트 설정
	self.label_star.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[self.label_star.font pointSize]];
	self.label_review.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[self.label_review.font pointSize]];
	
	// 평가 버튼 폰트 설정
	self.button_send.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[self.button_send.font pointSize]];
	
	// 평가 필드 폰트 설정
	self.textField_review.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:[self.textField_review.font pointSize]];
	
	// 액티비티 인티케이터 뷰 생성
	UIActivityIndicatorView *aiView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	self.indicatorView = aiView;
	[aiView release];
	self.indicatorView.frame = kActivityIndicatorViewFrameSizeOnNavi;
	self.indicatorView.hidesWhenStopped = YES;
	self.indicatorView.hidden = YES;
	[self.navigationController.navigationBar addSubview:self.indicatorView];
}


- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
	
}


- (void)viewDidUnload 
{
	self.label_star = nil;
	self.label_review = nil;
	self.button_star0 = nil;
	self.button_star1 = nil;
	self.button_star2 = nil;
	self.button_star3 = nil;
	self.button_star4 = nil;
	self.label_star0 = nil;
	self.label_star1 = nil;
	self.label_star2 = nil;
	self.label_star3 = nil;
	self.label_star4 = nil;
	self.textField_review = nil;
	self.arrayLabels = nil;
	self.button_send = nil;
	self.dbConnector = nil;
}


- (void)dealloc 
{
	self.label_star = nil;
	self.label_review = nil;
	self.button_star0 = nil;
	self.button_star1 = nil;
	self.button_star2 = nil;
	self.button_star3 = nil;
	self.button_star4 = nil;
	self.label_star0 = nil;
	self.label_star1 = nil;
	self.label_star2 = nil;
	self.label_star3 = nil;
	self.label_star4 = nil;
	self.textField_review = nil;
	self.arrayLabels = nil;
	self.button_send = nil;
	self.dbConnector = nil;
	
    [super dealloc];
}


- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	
	// 네비게이션바의 배경화면 설정
	UIImage *navigatinoBarBgImage = [UIImage imageNamed:JBFOOD_NAVIGATION_BAR_BG_FILENAME];
	CGRect bgRect = self.navigationController.navigationBar.bounds;
	UIImageView *navigatinoBarBgImageView = [[UIImageView alloc] initWithFrame:bgRect];
	navigatinoBarBgImageView.image = navigatinoBarBgImage;
	[self.navigationController.navigationBar insertSubview:navigatinoBarBgImageView atIndex:0];
	[navigatinoBarBgImageView release];
	
	
	// 네비게이션바의 타이틀뷰 설정
	UILabel *titleLabel = [[UILabel alloc] init];
	titleLabel.font = [UIFont fontWithName:JBFOOD_FONT_NAME size:JBFOOD_NAVIGATION_TITLE_FONT_SIZE];
	titleLabel.text = self.title;
	titleLabel.backgroundColor = [UIColor clearColor];
	[titleLabel sizeToFit];
	self.navigationItem.titleView = titleLabel;
	[titleLabel release];	
}



- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	// 뷰가 사라질때 데이터베이스가 연결되어있다면 델리게이트를 없애 에러를 방지한다.
	if (self.dbConnector)
	{
		[self.dbConnector removeAllDelegateReferenceToNil];
		self.dbConnector = nil;
	}
}


// 별 버튼 이벤트
- (IBAction)touchStarButton:(id)sender
{
	if (sender == self.button_star0)
	{
		[self drawActiveStar:0];
	}
	
	else if (sender == self.button_star1)
	{
		[self drawActiveStar:1];
	}
	
	else if (sender == self.button_star2)
	{
		[self drawActiveStar:2];
	}

	else if (sender == self.button_star3)
	{
		[self drawActiveStar:3];
	}

	else if (sender == self.button_star4)
	{
		[self drawActiveStar:4];
	}
}
		


// 별버튼의 텍스트 재 설정하기
- (void)drawActiveStar:(NSUInteger)number
{
	NSString *activeStar = @"★";
	NSString *deactiveStar = @"☆";
	
	NSUInteger index = 0;
		
	for ( UILabel *label in self.arrayLabels )
	{
		NSString *starString;

		if (index <= number)
		{
			starString = activeStar;
		}
		else
		{
			starString = deactiveStar;
		}
		
		label.text = starString;
		
		++index;
	}
	
	// 별점 값 저장.( +1을 한다.)
	self.starValue = number + 1;
}

// 평가 버튼 이벤트
- (IBAction)touchReviewButton:(id)sender
{
	// 리뷰 필드가 내용이 비어있는지 확인.
	if (!self.textField_review.text || [self.textField_review.text isEqualToString:@""])
	{
		// 알람뷰 생성하기
		UIAlertView *alertView = [[UIAlertView alloc]
								  initWithTitle:@"알림" 
								  message:@"한줄평을 쓰지 않았습니다. 귀찮더라도 써주시면 감사하겠습니다 ^ㅡ^"
								  delegate:self
								  cancelButtonTitle:@"확인"
								  otherButtonTitles:nil
								  ];
		[alertView show];
		[alertView release];

	}
	else
	{
		// 액티비티 인디케이터 뷰 애니메이션 시작
		[self.indicatorView startAnimating];

		
		// 데이터베이스 커넥터 생성
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		connector.userReviewDelegate = self;
		self.dbConnector = connector;
		[connector release];
		
		// ------------------------------------------------------------
		//	안드로이드와는 달리 아이폰에서 휴대폰번호를 직접 얻는것은 불가능하다. 
		//	따라서 기기의 고유값을 휴대폰번호 대신 전송하여 데이터베이스와 비교한다.
		// ------------------------------------------------------------
		UIDevice *device =  [UIDevice currentDevice];
		NSString *phoneId = device.uniqueIdentifier;
		
		[self.dbConnector startSendingUserReivew:self.restaurantId Phone:phoneId Review:self.textField_review.text Star:self.starValue];
	}

}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[textField resignFirstResponder];
	
	return YES;
}

#pragma mark -
#pragma mark 한줄평 리뷰 전송 델리게이트 메소드
- (void)sendUserReviewDidFail:(DatabaseConnector*)connector withError:(NSError*)data
{
	// 데이터베이스 커넥터 삭제
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];

	
	// 알람뷰 생성하기
	UIAlertView *alertView = [[UIAlertView alloc]
							  initWithTitle:@"알림" 
							  message:@"한줄평을 쓰지 않았습니다. 귀찮더라도 써주시면 감사하겠습니다 ^ㅡ^"
							  delegate:nil
							  cancelButtonTitle:@"확인"
							  otherButtonTitles:nil
							  ];
	alertView.tag = ALERT_VIEW_TAG_TYPE_NON_REVIEW;
	[alertView show];
	[alertView release];
	
}


- (void)sendUserReviewDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
{
	
}


- (void)sendUserReviewDidFinish:(DatabaseConnector*)connector withResult:(NSString*)result
{
	// 데이터베이스 커넥터 삭제
	self.dbConnector = nil;
	
	// 액티비티 인티케이터 뷰 애니메이터 종료
	[self.indicatorView stopAnimating];

	
	// 리뷰쓰기에 성공함을 알리는 델리게이트 메소드 호출
	[self.userReviewWriteDelegate userReviewWriteDidSuccess];
	
	// 상위의 뷰 컨트롤러로 이동.
	[self.navigationController popViewControllerAnimated:YES];
	
	
	// ---------------------------
	//	결과는 두가지 값으로 들어온다.
	//	리뷰 성공 : success
	//	리뷰가 이미 등록 : posted
	// ---------------------------
//	if ([result isEqualToString:@"success"])
//	{
//		// 리뷰쓰기에 성공함을 알리는 델리게이트 메소드 호출
//		[self.userReviewWriteDelegate userReviewWriteDidSuccess];
//		
//		// 상위의 뷰 컨트롤러로 이동.
//		[self.navigationController popViewControllerAnimated:YES];
//	}
//	else
//	{
//		NSString *message = [NSString stringWithFormat:@"이미 한줄평을 쓰셨습니다. 상위로 이동합니다."];
//		
//		// 알람뷰 생성하기
//		UIAlertView *alertView = [[UIAlertView alloc]
//								  initWithTitle:@"알림" 
//								  message:message
//								  delegate:self
//								  cancelButtonTitle:@"확인"
//								  otherButtonTitles:nil
//								  ];
//		
//		alertView.tag = ALERT_VIEW_TAG_TYPE_POSTED;
//		[alertView show];
//		[alertView release];		
//	}
}

#pragma mark -
#pragma mark 알람뷰 델리게이트 메소드
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	switch (alertView.tag)
	{
		case ALERT_VIEW_TAG_TYPE_NON_REVIEW:
		{
			
		}	break;
			
		case ALERT_VIEW_TAG_TYPE_POSTED:
		{
			// 상위의 뷰 컨트롤러로 이동.
			[self.navigationController popViewControllerAnimated:YES];
			
		}	break;			
	}	
}



@end

