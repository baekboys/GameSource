#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H


#include "FSMObject.h"
#include "GameApp.h"
#include "AppDataStructure.h"

// ���� ����, ������, �̵��ӵ�, ũ�� ���� ������ ���� ����ü
struct SBallParam
{
	// ���� ���� �Ķ����
	irr::scene::CBulletObjectAnimatorParams physicsParams;

	// Collision shape
	irr::scene::CBulletObjectAnimatorGeometry geometry;

	// ���� ������
	irr::f32 fBallSize;

	// ���� ������
	irr::f32 fJumpForce;

	// ���� �ִ� �̵��ӵ�
	irr::f32 fMaxLinearVelocity;
	
	// ���� �ִ� ���ӵ�
	irr::f32 fMaxAngularVelocity;

	// ���� �ؽ��� �̸�
	irr::core::stringc textureFileName;
	
	// sound resource file name
	//irr::core::stringc boundSoundFileName;
	irr::core::stringc jumpSoundFileName;
};

class CBallObject : public IFSMObject
{
public:

	//-----------------------------------------
	// ���� �Ӽ��� ���� ������� - private
	//-----------------------------------------

	// ���� Ÿ��
	enum BALL_TYPE 
	{ 
		BALL_TYPE_BASIC,
		BALL_TYPE_beach,
		BALL_TYPE_IRON,
		BALL_TYPE_TENNIS,
		BALL_TYPE_WOOD,
		BALL_TYPE_COUNT 
	};

	// XML������ �ʱ�ȭ - ���� �Ķ���� ������ �ִ� XML�����ʹ� �̸� �ε��� �δ� ���� ����.
	// ���� �ѹ� �ε��صθ� ���α׷��� ������ ���� �� ������ �ϳ��� ���� �����Ƿ� ����ƽ����-�Լ��� Ȱ���ϸ� ����.
	// ���� �� �۾��� ����ü�� ���鶧 �ϴ� �ͺ��� �ϸ���Ʈ �ʱ�ȭ �κп��� �ϴ� ���� ȿ�����̴�. 
	// CAppŬ������ �ϸ���Ʈ�� �ʱ�ȭ �ϴ� ������ CBallObject::InitBallParamFromXMLData() �Լ��� ȣ���Ѵ�.
	static bool InitBallParamFromXMLData();


	//-------------------
	// �����ڿ� �Ҹ���
	//-------------------
	CBallObject(void);
	virtual ~CBallObject(void);


	//-----------------
	// �����Լ� ����
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
	// �����Ǵ� ��ü�� ����ü���� �ƴ����� �����ϴ� �Լ�
	//-----------------------------------------------------------
	bool isBall()
	{
		return true;
	}


	//------------------------------------------------------------------------------------------------------------
	// ���� ������ ������ ��ü�� ���� Ÿ������ ��� �̺�Ʈ �˻縦 �Ѵ�. ���� Ÿ������ �����ִ� �Լ�
	// ��Ŭ�����̹Ƿ� �ڵ带 ������ �ʴ´�.
	//------------------------------------------------------------------------------------------------------------
	void setTargetObject(IFSMObject* obj)
	{

	}


	//---------------
	// �ʱ�ȭ �Լ�
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
	// ��  ��  ��
	//------------
	// ���� ����
	enum MAIN_STATUS { BALL_OBJ_READY, BALL_OBJ_COLLISION, BALL_OBJ_AIR, BALL_OBJ_JUMP, BALL_OBJ_ZERO_GRAVITY, BALL_OBJ_DIE };
	// �浹 ����
	enum COLLISION_DIRECTION { COL_UP, COL_DOWN, COL_LEFT, COL_RIGHT };


	//-----------------------------------------
	// ���� �Ӽ��� ���� ������� - private
	//-----------------------------------------
	// XML�κ��� �о���� irr::core::array<irr::f32>�� irr::core::array<irr::core::stringc>���� ����üSBallParam�� �����ϴ� �Լ�
	static void InputSBallParamFromXMLData(SBallParam &param, irr::core::array<irr::f32> &fData, irr::core::array<irr::core::stringc> &cData);
	// ���� ���� Ÿ��
	BALL_TYPE m_eBallType;
	// ���� ������
	irr::f32 m_fBallSize;
	// ���� ������
	irr::f32 m_fJumpForce;
	// ���� ���� Ÿ�̸�
	irr::f32 m_fJumpTimer;
	// ���� �ִ� �̵��ӵ�
	irr::f32 m_fMaxLinearVelocity;
	// ���� �ִ� ���ӵ�
	irr::f32 m_fMaxAngularVelocity;
	
	
	// ���� ������ ȿ���� ������ �ִ°�?
	bool m_bHasItemEffect;
	// ���� ������ ȿ�� Ÿ��
	irr::u32 m_uItemType;
	// ���� ������ ȿ�� ��
	irr::f32 m_fItemValue;
	// ���� ȿ�� ���ڿ�
	irr::core::stringw m_sItemEffect;
	// ���� ������ȿ�� ���ӽð� ����
	irr::f32 m_fItemEffectDurationTime;
	// ���ӽð� Ÿ�̸�
	irr::f32 m_fItemEffectTimer;
	
	// reset
	irr::core::vector3df m_startPos;
	irr::u32 m_uStartType;

	// ���ӵ����� ��
	irr::core::vector3df m_AccelValue;
	irr::f32 m_fAccel_Z_Value;
	

	//----------------------
	// �ϸ���Ʈ �������
	//----------------------
	
	// �ų��� ������
	irr::scene::ISceneNode* m_pMainNode;
	irr::scene::IMeshSceneNode* m_pSphereNode;
	irr::scene::IParticleSystemSceneNode *m_pItemEffectNode;
	irr::scene::CBulletChracterAnimator* m_pAnimator;
	btRigidBody* m_pRigidBody;
	
	// �ŸŴ����� �Ҹ����� ������
	irr::scene::ISceneManager* m_pSmgr;
	irr::video::IVideoDriver* m_pVideo;
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory;
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;
	

	//---------------------------------------------
	// �ܺ� ��ü�� ������ ��ȯ�� ���� ������
	//---------------------------------------------
	// GameApp
	CGameApp* m_pGameApp;


	//---------------
	// �̺�Ʈ ����
	//---------------
	// ����
	bool jump();
	// ���������� �������� üũ
	bool CheckPossibleJump();
	// �������� ȹ���Ͽ��� �� ��ƼŬ ����
	void CreateParticle(irr::u32 time);
	// Ÿ�Կ� ���� ���� ����
	void CreateBallObjFromSBallParam(BALL_TYPE type, irr::f32 size);
	// �浹üũ�ϱ�
	bool CollisionCheck();
	// ������ ȿ�� ���ֱ�
	void ResetItemEffect();
	
	// ���� ������ ����
	void setMoveVelocity(irr::core::vector3df* accelValue);
	
	void preLoadSound();
};

//--------------------------------------------------------------
// ���� �������� Ÿ���� ���鿡 ���� �Ӽ��� - ����ƽ ����
// ����ƽ �����̹Ƿ� �ܺο��� �����Ѵ�.
//--------------------------------------------------------------
static SBallParam m_sBallParam[CBallObject::BALL_TYPE_COUNT];

#endif