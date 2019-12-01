//------------------------------------------
// 맛십장소 어노테이션
//
// 맛집의 이름, 주소, 위치정보를 가지고 있는 클래스
//
//------------------------------------------
#import <Foundation/Foundation.h>
#import <MapKit/MapKit.h>

#pragma mark -
#pragma mark interface
@interface MapLocation : NSObject <MKAnnotation, NSCoding> 
{
	// 맛집의 이름
	NSString				*name;
	
	// 맛집의 주소
	NSString				*address;
	
	// 맛집의 위도, 경도
    CLLocationCoordinate2D	coordinate;
	
	// 맛집과 현재위치와의 거리
	double					radialDistance;
	
	// 데이터베이스 id값
	int						id;
	
	// 기타 데이터 (SearchByArea에서 지역이름의 이미지파일명 저장할때 쓰임. 다목적 용도)
	NSString				*etc;
}

#pragma mark -
#pragma mark property
@property (nonatomic, copy)			NSString				*name;
@property (nonatomic, copy)			NSString				*address;
@property (nonatomic, copy)			NSString				*etc;
@property (nonatomic, readwrite)	CLLocationCoordinate2D	coordinate;
@property (nonatomic)				double					radialDistance;
@property (nonatomic)				int						id;

@end
