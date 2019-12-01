#import "MapLocation.h"
#import <MapKit/MapKit.h>

@implementation MapLocation

#pragma mark -
#pragma mark synthesize
@synthesize name;
@synthesize address;
@synthesize coordinate;
@synthesize radialDistance;
@synthesize id;
@synthesize etc;

#pragma mark -
#pragma mark <MKAnnotation> 프로토콜 구현
- (NSString *)title 
{
    return name;
}

- (NSString *)subtitle 
{
	return address;
}

#pragma mark -
- (void)dealloc 
{
    self.address = nil;
	self.name = nil;
	self.etc = nil;
    [super dealloc];
}

#pragma mark -
#pragma mark <NSCoding> 프로토콜 구현
- (void) encodeWithCoder: (NSCoder *)encoder
{
	[encoder encodeObject: self.name forKey: @"name"];
    [encoder encodeObject: self.address forKey: @"address"];
    [encoder encodeDouble:self.coordinate.latitude forKey:@"latitude"];
    [encoder encodeDouble:self.coordinate.longitude	forKey:@"longitude"];
	[encoder encodeDouble:self.radialDistance forKey:@"distance"];
	[encoder encodeDouble:self.id forKey:@"id"];
}
- (id) initWithCoder: (NSCoder *)decoder  
{
    if (self = [super init]) 
	{
		self.name = [decoder decodeObjectForKey:@"name"];
		self.address = [decoder decodeObjectForKey:@"address"];
		
		CLLocationCoordinate2D coord;
		coord.latitude = [decoder decodeDoubleForKey:@"latitude"];
		coord.longitude = [decoder decodeDoubleForKey:@"longitude"];
		
		self.coordinate = coord;
		
		self.radialDistance = [decoder decodeDoubleForKey:@"distance"];
		
		self.id = [decoder decodeDoubleForKey:@"id"];
    }
    return self;
}
@end
