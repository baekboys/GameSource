// -------------------------------------------- //
//												//
//			맛집 어플에 쓰이는 공유되는 정의			//
//												//
// -------------------------------------------- //


// -----------------------------------------
//
//	전북 음식문화 플라자 URL과 네이버카페 URL
//
//	FoodCulturePlazaViewController 클래스 사용
//	
// ------------------------------------------
#define FOOD_CULTURE_PLAZA_TITLE				@"음식문화 프라자"
#define FOOD_CULTURE_PLAZA_URL					@"http://jbfood.go.kr/mshop/jbfood_home/index.html"
#define NAVER_CAFE_TITLE						@"맛집 카페"
#define	NAVER_CAFE_URL							@"http://m.cafe.daum.net/jbfood2011"



// --------------------------------
//
//	맛집 데이터베이스 URL과 열거자 선언
//
//	DatabaseConnector 클래스 사용
//	
// --------------------------------


// ---------------------
//	주제별 맛집 데이터베이스
// ---------------------

// 주제별 열거자
typedef enum
{
	SUBJECT_INTERIA,
	SUBJECT_PRICE,
	SUBJECT_AGE,
	SUBJECT_MOOD,
	SUBJECT_THEME,
}	SUBJECT_SECTION;

// 주제별 이름
#define kSUBJECT_INTERIA_NAME					@"인테리어별"
#define kSUBJECT_PRICE_NAME						@"가격대별"
#define kSUBJECT_AGE_NAME						@"연령대별"
#define kSUBJECT_MOOD_NAME						@"분위기별"
#define kSUBJECT_THEME_NAME						@"테마별"

// 주제별 검색 URL
#define JBFOOD_DATABASE_URL_SITUATION			@"http://jbfood.go.kr/mshop/jbfood/feature_list.php"
//#define JBFOOD_DATABASE_URL_INTERIA				@"http://jbfood.go.kr/mshop/jbfood/sinte_list.php"
//#define JBFOOD_DATABASE_URL_PRICE				@"http://jbfood.go.kr/mshop/jbfood/sprice_list.php"
//#define JBFOOD_DATABASE_URL_AGE					@"http://jbfood.go.kr/mshop/jbfood/sage_list.php"
//#define JBFOOD_DATABASE_URL_MOOD				@"http://jbfood.go.kr/mshop/jbfood/satm_list.php"
//#define JBFOOD_DATABASE_URL_THEME				@"http://jbfood.go.kr/mshop/jbfood/spart_list.php"



// 상호명, 메뉴, 타이틀 별 검색 URL
#define JBFOOD_DATABASE_URL_NAME				@"http://jbfood.go.kr/mshop/jbfood/name_search.php"
// 상호명, 메뉴, 타이틀 별 검색 URL
#define JBFOOD_DATABASE_URL_NAME_MENU_TITLE		@"http://jbfood.go.kr/mshop/jbfood/intel_search2_3.php"


// --------------------
//	맛집 전체 데이터베이스
// --------------------
#define JBFOOD_DATABASE_URL_ALL_RESTURANT		@"http://jbfood.go.kr/mshop/jbfood/all_list3.php"


// -----------------------
//	맛집 추천메뉴 데이터베이스
// -----------------------
#define JBFOOD_DATABASE_URL_RECOMMENDATION_MENU	@"http://jbfood.go.kr/mshop/jbfood/menu_list.php"


// -----------------------
//	맛집 평가하기 데이터베이스
// -----------------------
#define JBFOOD_DATABASE_URL_USER_REVIEW			@"http://jbfood.go.kr/mshop/jbfood/tok_ins.php"


// ---------------------
//	지역별 맛집 데이터베이스
// ---------------------

// 지역별 열거자
typedef enum
{
	AREA_GOCHANG,
	AREA_GUNSAN,
	AREA_KIMJE,
	AREA_NAMWON,
	AREA_MUJU,
	AREA_BUAN,
	AREA_SUNCHANG,
	AREA_WANJU,
	AREA_IKSAN,
	AREA_IMSIL,
	AREA_JANGSU,
	AREA_JEONJU,
	AREA_JEONGEB,
	AREA_JINAN,
	AREA_TOTAL_COUNT
}  AREA_LIST;

// 지역별 이름
#define kAREA_GOCHANG							@"고창"
#define kAREA_GUNSAN							@"군산"
#define kAREA_KIMJE								@"김제"
#define kAREA_NAMWON							@"남원"
#define kAREA_MUJU								@"무주"
#define kAREA_BUAN								@"부안"
#define kAREA_SUNCHANG							@"순창"
#define kAREA_WANJU								@"완주"
#define kAREA_IKSAN								@"익산"
#define kAREA_IMSIL								@"임실"
#define kAREA_JANGSU							@"장수"
#define kAREA_JEONJU							@"전주"
#define kAREA_JEONGEB							@"정읍"
#define kAREA_JINAN								@"진안"

// 지역별 검색 URL
#define JBFOOD_DATABASE_URL_AREA				@"http://jbfood.go.kr/mshop/jbfood/all_list3_1.php"


// --------------------
//	맛집 세부 데이터베이스
// --------------------
#define JBFOOD_DATABASE_URL_DATA				@"http://jbfood.go.kr/mshop/jbfood/get_data3_2.php"



// -----------------------------
//	맛집 이미지와 섬네일이 저장된 URL
// -----------------------------
#define JBFOOD_DATABASE_URL_IMAGEDATA			@"http://jbfood.go.kr/mshop/datafolder/foodimg/"
#define JBFOOD_DATABASE_URL_THUMBNAILDATA		@"http://jbfood.go.kr/mshop/datafolder/thumbnail/"

// -------------
//	알람뷰 태그값
// -------------
typedef enum
{
	ALERT_DB_CONNECTOR_ARRAY,
	ALERT_DB_CONNECTOR_ARRAY_NOT_FOUND,
	ALERT_DB_CONNECTOR_RESTAURANT,
	ALERT_REVERSE_GEOCODER,
	ALERT_DB_CONNECTOR_ALL_RESTAURANT
}	ALERT_TYPE;


// ----------------
//	배경이미지 파일명
// ----------------
#define JBFOOD_MAIN_BG_FILENAME					@"mainbg.png"
#define JBFOOD_VIEW_BG_FILENAME					@"bg.png"
#define JBFOOD_NAVIGATION_BAR_BG_FILENAME		@"navigationBarBg.png"

#define JBFOOD_BLANK_SMALL_IMG_FILENAME			@"blank_img_small.png"
#define JBFOOD_BLANK_NORMAL_IMG_FILENAME		@"blank_img_normal.png"


// ----------
//	폰트 이름
// ----------
#define JBFOOD_FONT_NAME						@"HeummNemogulim132"
#define JBFOOD_NAVIGATION_TITLE_FONT_SIZE		27


// ----------------------------------------
//	각각 테이블뷰는 기본이되는 메인뷰를 가지고있다.
//	이유는 배경화면 때문.
//	따라서 메인뷰의 사이즈 정의
// ----------------------------------------
#define kMAINVIEW_WIDTH							320
#define kMAINVIEW_HEGIHT						460 // 상태바를 가지고 있기 때문에 480-20의 사이즈.

// 네비게이션바에 붙인 액티비티 인디케이터 뷰의 프레임
#define kActivityIndicatorViewFrameSizeOnNavi	CGRectMake(280, 8, 30, 30);


// ---------------------
//  단골맛집 sqlite 파일명
// ---------------------
#define JBFOOD_SQLTIE_FILENAME					@"FavoriteRestaurant.sqlite"