//------------------------------------------
// 맛십장소 어노테이션
//
// 맛집의 이름, 주소, 위치정보를 가지고 있는 클래스
//
//------------------------------------------
#import <Foundation/Foundation.h>
#import <MapKit/MapKit.h>

#pragma mark -
#pragma mark define
// 내 위치 어노테이션의 이름
#define kMyLocationName				@"현재 위치"

@interface MyLocation : NSObject <MKAnnotation, NSCoding> 
{
	// 맛집의 주소
	NSString				*address;
	
	// 맛집의 위도, 경도
    CLLocationCoordinate2D	coordinate;
}

#pragma mark -
#pragma mark property
@property (nonatomic, copy)			NSString				*address;
@property (nonatomic, readwrite)	CLLocationCoordinate2D	coordinate;

@end
