#import "DatabaseTestViewController.h"
#import "Restaurant.h"
#import "SHARED_DEFINE.h"
#import "RecommendationMenu.h"
#import "SQLiteDatabaseConnector.h"
#import "SQLiteDB.h"

@implementation DatabaseTestViewController

@synthesize button0;
@synthesize button1;
@synthesize button2;
@synthesize button3;
@synthesize button4;
@synthesize button5;
@synthesize button6;
@synthesize textField;
@synthesize textView;
@synthesize imageView;
@synthesize dbConnector;
@synthesize receivedData;

// The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil 
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) 
	{
        self.dbConnector = [[DatabaseConnector alloc] init];
		self.receivedData = nil;
    }
    return self;
}


/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations.
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning 
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload 
{
    [super viewDidUnload];

	self.button0 = nil;
	self.button1 = nil;
	self.button2 = nil;
	self.button3 = nil;
	self.button4 = nil;
	self.button5 = nil;
	self.button6 = nil;
	self.textField = nil;
	self.textView = nil;
	self.imageView = nil;
	self.dbConnector = nil;
	self.receivedData = nil;
}


- (void)dealloc
{
	self.button0 = nil;
	self.button1 = nil;
	self.button2 = nil;
	self.button3 = nil;
	self.button4 = nil;
	self.button5 = nil;
	self.button6 = nil;
	self.textField = nil;
	self.textView = nil;
	self.imageView = nil;
	self.dbConnector = nil;
	self.receivedData = nil;
    [super dealloc];
}

- (IBAction)touchButtons:(id)sender
{	
	if (sender == self.button0)
	{
		SQLiteDatabaseConnector *sqliteDBConnector = [[SQLiteDatabaseConnector alloc] init];
		if ([sqliteDBConnector.sqliteDB isGoodConnection])
		{
			NSMutableArray *restaurantArray = [sqliteDBConnector getRestaurantArrayFromDB];
			
			NSMutableString *resultString = [NSMutableString string];
			
			for (Restaurant *restaurant in restaurantArray)
			{
				[resultString appendFormat:@"id : %d 이름 : %@\n", restaurant.id, restaurant.name];
			}
			
			self.textView.text = resultString;
		}
		[sqliteDBConnector release];
	}
	
	else if (sender == self.button1)
	{
		SQLiteDatabaseConnector *sqliteDBConnector = [[SQLiteDatabaseConnector alloc] init];
		if ([sqliteDBConnector.sqliteDB isGoodConnection])
		{
			NSMutableString *resultString = [NSMutableString string];
			BOOL result = [sqliteDBConnector removeRestaurantFromSQliteDBWithID:[self.textField.text intValue]];
			
			if (result)
			{
				[resultString appendString:@"성공적으로 삭제되었습니다."];
			}
			else
			{
				[resultString appendString:@"삭제실패"];
			}
			self.textView.text = resultString;
		}
		[sqliteDBConnector release];
	}
	
	else if (sender == self.button2)
	{
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		connector.arrayRestaurantsFromSituationDelegate = self;
		
		if ([connector startReceivingArrayRestaurantsFromSituation:SUBJECT_INTERIA withParam:self.textField.text])
		{
			self.textView.text = @"연결을 시작합니다.";
		}				
	}
	
	else if (sender == self.button3)
	{
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		connector.allRestaurantsDelegate = self;
		
		if ([connector startReceivingAllRestaurants])
		{
			self.textView.text = @"연결을 시작합니다. 모든 맛집 데이터를 받아옵니다.";
		}		
	}
	
	else if (sender == self.button4)
	{		
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		connector.restaurantFromIdDelegate = self;
		
		if ([connector startReceivingRestaurantFromId:[self.textField.text intValue]])
		{
			self.textView.text = @"연결을 시작합니다.";
		}
	}
	
	else if (sender == self.button5)
	{
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		connector.arrayRestaurantsFromAreaDelegate = self;
		
		if ([connector startReceivingArrayRestaurantsFromArea:self.textField.text])
		{
			self.textView.text = @"연결을 시작합니다.";
		}
	}
	
	else if (sender == self.button6)
	{
		DatabaseConnector *connector = [[DatabaseConnector alloc] init];
		connector.userReviewDelegate = self;

		NSString *review = @"좋아요~~!!!";
		NSUInteger star = 4;
		//UIDevice *device = [UIDevice currentDevice];
		//NSString *phone = device.uniqueIdentifier;
		//[device release];
		
		NSString *phone = @"010xxxxyyyy";
		
		if ([connector startSendingUserReivew:[self.textField.text intValue] Phone:phone Review:review Star:star])
		{
			self.textView.text = @"연결을 시작합니다.";
		}		
	}
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[self.textField resignFirstResponder];
	return YES;
}


- (void)arrayDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{
	self.textView.text = @"데이터를 받고 있습니다.";
}


- (void)arrayDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{
	[connector release];
	self.textView.text = @"데이터 전송에 실패하였습니다.";
}

- (void)arrayDidFinish:(DatabaseConnector *)connector withArrayRestaurants:(NSMutableArray *)array
{
	[connector release];
	
	NSMutableString *string = [NSMutableString string];
	[string appendString:@"데이터 전송에 성공하였습니다.\n"];

	for (Restaurant *item in array)
	{
		[string appendFormat:@"id : %d 이름 : %@\n",  item.id, item.name];
	}
	
	self.textView.text = string;
}


- (void)restaurantDidReceivingData:(DatabaseConnector *)connector withData:(NSData *)data
{
	self.textView.text = @"데이터를 받고 있습니다.";
}

- (void)restaurantDidFail:(DatabaseConnector *)connector withError:(NSError *)data
{
	[connector release];
	self.textView.text = @"데이터 전송에 실패하였습니다.";
}

- (void)restaurantDidFinish:(DatabaseConnector *)connector withRestaurant:(Restaurant *)newRestaurant
{
	[connector release];
	
	NSMutableString *test =	[NSMutableString stringWithFormat:@"데이터 전송에 성공하였습니다. : %@", newRestaurant.name];
	
	if (newRestaurant) 
	{
		SQLiteDatabaseConnector *sqliteDBConnector = [[SQLiteDatabaseConnector alloc] init];
		if ([sqliteDBConnector.sqliteDB isGoodConnection])
		{
			ADD_RETURN_TYPE result = [sqliteDBConnector addRestaurantToSQliteDB:newRestaurant];
			
			switch (result)
			{
				case ADD_RETURN_ALREADY:
				{
					[test appendString:@"\n이미 저장되어 있음."];
				}	break;
					
				case ADD_RETURN_COMPLETE:
				{
					[test appendString:@"\nsqlite에 데이터삽입 성공"];
				}	break;
					
				case ADD_RETURN_FAILED:
				{
					[test appendString:@"\nsqlite에 데이터삽입 실패"];
				}	break;
					
				case ADD_RETURN_NO_CONNECTION:
				{
					[test appendString:@"\nsqlite에 연결되어 있지 않음."];
				}	break;
			}
		}
		
		[sqliteDBConnector release];
	}
	else
	{
		[test appendString:@"\n하지만 맛집 정보가 없습니다."];
	}

		
	self.textView.text = test;
	
}

//- (void)recommendationMenuDidFail:(DatabaseConnector*)connector withError:(NSError*)data
//{
//	[connector release];
//	self.textView.text = @"데이터 전송에 실패하였습니다.";
//
//}
//
//- (void)recommendationMenuDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
//{
//	self.textView.text = @"데이터를 받고 있습니다.";
//}
//
//- (void)recommendationMenuDidFinish:(DatabaseConnector*)connector withMenuArray:(NSArray*)array
//{
//	[connector release];
//	NSMutableString *string = [NSMutableString string];
//	
//	if (array)
//	{
//		for (RecommendationMenu *menu in array)
//		{
//			[string appendFormat:@"메뉴 : %@, 가격: %@\n", menu.menu, menu.price];
//		}
//	}
//	else
//	{
//		[string appendString:@"데이터가 없습니다."];
//	}
//
//	self.textView.text = [NSString stringWithFormat:@"데이터 전송에 성공하였습니다.\n%@", string];
//}


- (void)allRestaurantsDidFail:(DatabaseConnector*)connector withError:(NSError*)data
{
	[connector release];
	self.textView.text = @"데이터 전송에 실패하였습니다.";
}


- (void)allRestaurantsDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
{
	self.textView.text = @"데이터를 받고 있습니다.";
}


- (void)allRestaurantsDidFinish:(DatabaseConnector*)connector withArray:(NSMutableArray*)array
{
	[connector release];
	
	NSMutableString *string = [NSMutableString string];
	[string appendString:@"데이터 전송에 성공하였습니다.\n"];
	
	for (Restaurant *item in array)
	{
		[string appendFormat:@"id : %d 이름 : %@\n",  item.id, item.name];
	}
	
	self.textView.text = string;
}


- (void)sendUserReviewDidFail:(DatabaseConnector*)connector withError:(NSError*)data
{
	[connector release];
	self.textView.text = @"데이터 전송에 실패하였습니다.";
}


- (void)sendUserReviewDidReceivingData:(DatabaseConnector*)connector withData:(NSData*)data
{

}


- (void)sendUserReviewDidFinish:(DatabaseConnector*)connector withResult:(NSString*)result
{
	[connector release];
	
	NSMutableString *string = [NSMutableString string];
	[string appendString:@"데이터 전송에 성공하였습니다.\n"];
	[string appendString:result];
	
	self.textView.text = string;
}


@end
