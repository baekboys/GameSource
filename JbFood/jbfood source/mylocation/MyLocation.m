#import "MyLocation.h"
#import <MapKit/MapKit.h>

@implementation MyLocation

#pragma mark -
#pragma mark synthesize
@synthesize address;
@synthesize coordinate;

#pragma mark -
#pragma mark <MKAnnotation> 프로토콜 구현
- (NSString *)title 
{
    return kMyLocationName;
}

- (NSString *)subtitle 
{
	return address;
}

#pragma mark -
- (void)dealloc 
{
    self.address = nil;
    [super dealloc];
}

#pragma mark -
#pragma mark <NSCoding> 프로토콜 구현
- (void) encodeWithCoder: (NSCoder *)encoder
{
    [encoder encodeObject: self.address forKey: @"address"];
    [encoder encodeDouble:self.coordinate.latitude forKey:@"latitude"];
    [encoder encodeDouble:self.coordinate.longitude	forKey:@"longitude"];
}
- (id) initWithCoder: (NSCoder *)decoder  
{
    if (self = [super init]) 
	{
		self.address = [decoder decodeObjectForKey:@"address"];
		
		CLLocationCoordinate2D coord;
		coord.latitude = [decoder decodeDoubleForKey:@"latitude"];
		coord.longitude = [decoder decodeDoubleForKey:@"longitude"];
		
		self.coordinate = coord;
    }
    return self;
}
@end
