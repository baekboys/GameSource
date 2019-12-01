#pragma once

#include "FSMObject.h"

#include "BtEngine.h"
#include "UtilHeader.h"
#include "FactoryHeader.h"




//////////////////////////////////////////////
//											//
// 비캐릭터 클래스							//
// 캐릭터가 아닌 모든 클래스는				//
// 아래의 비캐릭터클래스를 상속 받는다.		//
//											//
//////////////////////////////////////////////

class INonCharacterObject : public IFSMObject
{
protected :
	
	//////////////////////
	//	비캐릭터의 특성	//
	//////////////////////

	// 질량
	irr::f32 m_mass;

	// 마찰계수
	irr::f32 m_friction;



	//////////////////////
	//	비캐릭터의 노드	//
	//////////////////////


	//////////////////
	//	노		드	//
	//////////////////
	
	// 비캐릭터의 노드
	irr::scene::jz3d::CFormatedAnimationNode *m_pFNode;
	
	// 비캐릭터가 리스폰될 위치정보가 있는 위치벡터
	irr::core::vector3df m_startPosition;

	// 비캐릭터의 애니메이터
	irr::scene::CBulletObjectAnimator *m_pAnimator;

	// 충돌정보관리 노드
	irr::scene::jz3d::CCollusionMngNode *m_pCollMngNode;

public :
	// 생성자와 소멸자
	INonCharacterObject()
	: m_pFNode (NULL), m_pAnimator (NULL), m_pCollMngNode (NULL)
	{
	}
	virtual ~INonCharacterObject() {}

	// 순수가상함수
	virtual void Signal(std::string strSignal,void *pParam) = 0;	
	virtual void Update(irr::f32 fDelta) = 0;
	virtual bool isDie() = 0;
	
	virtual inline irr::scene::jz3d::CFormatedAnimationNode* getFormatedAnimationNode()
	{
		return m_pFNode;
	}
	
	// 강체 얻어오기
	virtual inline  btRigidBody* getRigidBody() 
	{
		return m_pAnimator->getRigidBody();
	}

	// 노드의 위치 얻어오기
	virtual inline irr::core::vector3df getPosition()
	{
		return m_pFNode->getAbsolutePosition();
	}

	//노드의 애니메이터 얻기
	virtual inline irr::scene::CBulletObjectAnimator* getAnimator()
	{
		return m_pAnimator;
	}
};