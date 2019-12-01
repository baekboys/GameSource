#ifndef ITEM_OBJECT_H
#define ITEM_OBJECT_H

#include "FSMObject.h"

class CItemObject : public IFSMObject
{
	enum STATUS { READY, ACTIVE, DIE };

	// 충돌처리를 위한 박스 신노드
	irr::scene::ISceneNode* m_pMainBoxNode;

	// 박스신노드의 자식. 즉 아이템 신노드
	irr::scene::ISceneNode* m_pItemSceneNode;

	// 공 객체
	IFSMObject* m_pBallObj;

	// 아이템 속성 및 값
	//irr::core::stringw m_cItemTypeName;
	irr::s32 m_sItemType;
	irr::f32 m_fItemValue1;
	irr::f32 m_fDurationTime;

	// 공과의 거리를 설정하는 값
	irr::f32 m_fActiveDistanceFromBall;
	

public:
	CItemObject(void);
	virtual ~CItemObject(void);

	inline irr::core::vector3df getPosition()
	{
		return m_pMainBoxNode->getAbsolutePosition();
	}

	inline bool isDie()
	{
		if(GetStatus() == DIE)
			return true;
		return false;
	}

	inline btRigidBody* getRigidBody()
	{
		return NULL;
	}
	
	inline irr::scene::ISceneNode* getSceneNode()
	{
		return m_pMainBoxNode;
	}
	
	inline irr::scene::CBulletChracterAnimator* getAnimator()
	{
		return NULL;
	}

	inline bool isBall()
	{
		return false;
	}

	inline void setTargetObject(IFSMObject* obj)
	{
		m_pBallObj = obj;
	}

	bool Init(irr::scene::ISceneNode* pNode, void* pParam);
	void Signal(irr::core::stringc strSignal,void *pParam);
	void Update(irr::f32 fTick);
	void reset()
	{
		m_pMainBoxNode->setVisible(true);
		SetStatus(READY);
	}
};

#endif