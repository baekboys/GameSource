#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H


#include "FSMObject.h"
#include "GameApp.h"
#include "AppDataStructure.h"

// 공의 물리, 점프력, 이동속도, 크기 등의 정보를 가진 구조체
struct SBallParam
{
	// 공의 물리 파라미터
	irr::scene::CBulletObjectAnimatorParams physicsParams;

	// Collision shape
	irr::scene::CBulletObjectAnimatorGeometry geometry;

	// 공의 사이즈
	irr::f32 fBallSize;

	// 공의 점프력
	irr::f32 fJumpForce;

	// 공의 최대 이동속도
	irr::f32 fMaxLinearVelocity;
	
	// 공의 최대 각속도
	irr::f32 fMaxAngularVelocity;

	// 공의 텍스쳐 이름
	irr::core::stringc textureFileName;
	
	// sound resource file name
	//irr::core::stringc boundSoundFileName;
	irr::core::stringc jumpSoundFileName;
};

class CBallObject : public IFSMObject
{
public:

	//-----------------------------------------
	// 공의 속성에 관한 멤버변수 - private
	//-----------------------------------------

	// 공의 타입
	enum BALL_TYPE 
	{ 
		BALL_TYPE_BASIC,
		BALL_TYPE_beach,
		BALL_TYPE_IRON,
		BALL_TYPE_TENNIS,
		BALL_TYPE_WOOD,
		BALL_TYPE_COUNT 
	};

	// XML데이터 초기화 - 공의 파라미터 값들이 있는 XML데이터는 미리 로딩해 두는 것이 좋다.
	// 또한 한번 로딩해두면 프로그램이 끝날때 까지 이 데이터 하나로 쓸수 있으므로 스태틱변수-함수를 활용하면 좋다.
	// 따라서 이 작업은 공객체를 만들때 하는 것보다 일리히트 초기화 부분에서 하는 것이 효율적이다. 
	// CApp클래스의 일리히트를 초기화 하는 곳에서 CBallObject::InitBallParamFromXMLData() 함수를 호출한다.
	static bool InitBallParamFromXMLData();


	//-------------------
	// 생성자와 소멸자
	//-------------------
	CBallObject(void);
	virtual ~CBallObject(void);


	//-----------------
	// 가상함수 구현
	//-----------------
	irr::core::vector3df getPosition();
	bool isDie();
	void Signal(irr::core::stringc strSignal,void *pParam);
	void Update(irr::f32 fTick);	
	btRigidBody* getRigidBody();
	irr::scene::ISceneNode* getSceneNode();
	irr::scene::CBulletChracterAnimator* getAnimator();

	void reset();
	
	void gameStartByPosionAndType(irr::core::vector3df position, irr::u32 type)
	{
		m_startPos = position;
		m_uStartType = type;
		reset();
	}


	//-----------------------------------------------------------
	// 생성되는 객체가 공객체인지 아닌지를 구분하는 함수
	//-----------------------------------------------------------
	bool isBall()
	{
		return true;
	}


	//------------------------------------------------------------------------------------------------------------
	// 공을 제외한 나머지 객체는 공을 타겟으로 삼아 이벤트 검사를 한다. 공을 타겟으로 정해주는 함수
	// 공클래스이므로 코드를 만들지 않는다.
	//------------------------------------------------------------------------------------------------------------
	void setTargetObject(IFSMObject* obj)
	{

	}


	//---------------
	// 초기화 함수
	//---------------
	bool Init(
		irr::scene::ISceneManager* pSmgr, 
		irr::video::IVideoDriver* pVideo,
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory, 
		irr::scene::CBulletWorldAnimator* pWorldAnimator,
		CGameApp *gameApp
		);
		
private:

	//------------
	// 열  거  자
	//------------
	// 공의 상태
	enum MAIN_STATUS { BALL_OBJ_READY, BALL_OBJ_COLLISION, BALL_OBJ_AIR, BALL_OBJ_JUMP, BALL_OBJ_ZERO_GRAVITY, BALL_OBJ_DIE };
	// 충돌 방향
	enum COLLISION_DIRECTION { COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT };


	//-----------------------------------------
	// 공의 속성에 관한 멤버변수 - private
	//-----------------------------------------
	// XML로부터 읽어들인 irr::core::array<irr::f32>와 irr::core::array<irr::core::stringc>값을 구조체SBallParam에 저장하는 함수
	static void InputSBallParamFromXMLData(SBallParam &param, irr::core::array<irr::f32> &fData, irr::core::array<irr::core::stringc> &cData);
	// 현재 공의 타입
	BALL_TYPE m_eBallType;
	// 공의 사이즈
	irr::f32 m_fBallSize;
	// 공의 점프력
	irr::f32 m_fJumpForce;
	// 공의 점프 타이머
	irr::f32 m_fJumpTimer;
	// 공의 최대 이동속도
	irr::f32 m_fMaxLinearVelocity;
	// 공의 최대 각속도
	irr::f32 m_fMaxAngularVelocity;
	
	
	// 현재 아이템 효과를 가지고 있는가?
	bool m_bHasItemEffect;
	// 공의 아이템 효과 타입
	irr::u32 m_uItemType;
	// 공의 아이템 효과 값
	irr::f32 m_fItemValue;
	// 공의 효과 문자열
	irr::core::stringw m_sItemEffect;
	// 공의 아이템효과 지속시간 변수
	irr::f32 m_fItemEffectDurationTime;
	// 지속시간 타이머
	irr::f32 m_fItemEffectTimer;
	
	// reset
	irr::core::vector3df m_startPos;
	irr::u32 m_uStartType;

	// 가속도센서 값
	irr::core::vector3df m_AccelValue;
	irr::f32 m_fAccel_Z_Value;
	

	//----------------------
	// 일리히트 멤버변수
	//----------------------
	
	// 신노드와 포인터
	irr::scene::ISceneNode* m_pMainNode;
	irr::scene::IMeshSceneNode* m_pSphereNode;
	irr::scene::IParticleSystemSceneNode *m_pItemEffectNode;
	irr::scene::CBulletChracterAnimator* m_pAnimator;
	btRigidBody* m_pRigidBody;
	
	// 신매니저와 불릿엔진 포인터
	irr::scene::ISceneManager* m_pSmgr;
	irr::video::IVideoDriver* m_pVideo;
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory;
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;
	

	//---------------------------------------------
	// 외부 객체와 데이터 교환을 위한 포인터
	//---------------------------------------------
	// GameApp
	CGameApp* m_pGameApp;


	//---------------
	// 이벤트 관련
	//---------------
	// 점프
	bool jump();
	// 점프가능한 상태인지 체크
	bool CheckPossibleJump();
	// 아이템을 획득하였을 때 파티클 생성
	void CreateParticle(irr::u32 time);
	// 타입에 따라 공을 생성
	void CreateBallObjFromSBallParam(BALL_TYPE type, irr::f32 size);
	// 충돌체크하기
	bool CollisionCheck();
	// 아이템 효과 없애기
	void ResetItemEffect();
	
	// 공의 움직임 제어
	void setMoveVelocity(irr::core::vector3df* accelValue);
	
	void preLoadSound();
};

//--------------------------------------------------------------
// 각종 여러가지 타입의 공들에 대한 속성값 - 스태틱 변수
// 스태틱 변수이므로 외부에서 선언한다.
//--------------------------------------------------------------
static SBallParam m_sBallParam[CBallObject::BALL_TYPE_COUNT];

#endif