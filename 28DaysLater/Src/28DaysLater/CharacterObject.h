#pragma once

#include "FSMObject.h"
#include "BtEngine.h"
#include "UtilHeader.h"
#include "FactoryHeader.h"
#include "CollisionManager.h"

//////////////////////////////////////////
//										//
// 캐릭터 클래스						//
// 모든 캐릭터는 이 클래스를 상속받는다.//
//										//
//////////////////////////////////////////

class ICharacterObject : public IFSMObject
{
protected :
	
	//////////////////////
	//	캐릭터의 특성	//
	//////////////////////

	// 체력
	irr::f32 m_hp;
	
	// 총체력
	irr::f32 m_maxHp;
	
	// 이동속도
	irr::f32 m_MoveVelocity;
	
	// 달리기속도
	irr::f32 m_RunVelocity;

	// 달리기 지속시간
	irr::f32 m_runDurationTime;

	// 달리기 회복 속도 (스탠드상태)
	irr::f32 m_runRecovery_stand;

	// 달리기 회복 속도 (무브상태)
	irr::f32 m_runRecovery_move;

	// 스태미너(달리기) 퍼센트
	irr::u16 m_staminaPercent;

	// 달리기 지속시간 계산을 위한 틱값
	irr::f32 m_runDurationTimeTick;

	// 질량
	irr::f32 m_mass;

	// 마찰계수
	irr::f32 m_friction;

	// 주인공인지 판단
	bool m_hero;

	//////////////////
	//	노		드	//
	//////////////////
	
	// 캐릭터의 노드
	irr::scene::jz3d::CFormatedAnimationNode *m_pFNode;
	
	// 캐릭터가 리스폰될 위치정보가 있는 위치벡터
	irr::core::vector3df m_startPosition;

	//--------------------------
	// 이동을 위한 물리충돌노드
	//--------------------------
	irr::scene::jz3d::CCollusionMngNode *m_pCollMngNode;

	//-------------------------------------
	// rigidbody, 즉 강체를 얻기위한 포인터
	//-------------------------------------
	btCollisionObject *m_pCloBody;

	//----------------------
	// 충돌처리 관리 매니저
	//----------------------
	CCollisionManager m_colManager;

	//------------
	// 카메라 노드
	//------------
	irr::scene::ICameraSceneNode* m_pCamNode;
	irr::f32 m_farValue;

	//-----------
	// 사운드엔진
	//-----------
	irrklang::ISoundEngine* m_SoundEngine;

public:
	//////////////////////
	//	생성자 및 소멸자
	//////////////////////
	ICharacterObject()
		: IFSMObject(), 
			m_hp (0.f), m_maxHp (0.f), m_MoveVelocity (0.f), m_RunVelocity (0.f), m_runDurationTime (0.f), 
			m_runRecovery_move (0), m_runRecovery_stand (0), m_runDurationTimeTick (0), m_staminaPercent (0),
			m_hero (false), m_pFNode (NULL), m_startPosition (0,0,0), 
			m_pCollMngNode (NULL), m_pCloBody (NULL), m_SoundEngine(NULL)
	{
	}
	virtual ~ICharacterObject() {}
	
	//////////////////////////////////////////////////
	//	ICharacterObject형에서 사용가능 한 함수 구현
	//////////////////////////////////////////////////

	//-----------------------
	// 캐릭터의 위치 얻어오기
	//-----------------------
	inline irr::core::vector3df getPosition()
	{
		return m_pFNode->getAbsolutePosition();
	}

	//-------------------------
	// 캐릭터의 신노드 얻어오기
	//-------------------------
	inline irr::scene::jz3d::CFormatedAnimationNode* getFormatedAnimationNode()
	{
		return m_pFNode;
	}

	//---------------------------
	// 캐릭터의 충돌정보 얻어오기
	//---------------------------
	inline irr::scene::jz3d::CCollusionMngNode* getCollusionMngNode()
	{
		return m_pCollMngNode;
	}

	//-----------------------------
	// 캐릭터의 충돌매니저 얻어오기
	//-----------------------------
	inline CCollisionManager* getCollisionMng()
	{
		return &m_colManager;
	}

	///////////////////////////////////////////
	//	가상함수 및 구현을 위한 순수 가상함수
	///////////////////////////////////////////

	//-------------
	// 순수가상함수
	//-------------
	virtual btRigidBody* getRigidBody() = 0;
	virtual void Signal(std::string strSignal,void *pParam) = 0;	
	virtual void Update(irr::f32 fDelta) = 0;
	virtual bool isDie()=0;
	virtual bool isHero()=0;

	//------------
	// 초기화 코드
	//------------
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
	{
		if(pNode->getType() != irr::scene::jz3d::CFormatedAnimationNode::TypeID)
			return false;

		m_pFNode = (irr::scene::jz3d::CFormatedAnimationNode *) pNode;

		m_pFNode->setVisible(false);

		m_startPosition = startPosition;

		return true;
	}
};