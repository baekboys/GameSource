#ifndef ITEM_OBJECT_H
#define ITEM_OBJECT_H

#include "FSMObject.h"

class CItemObject : public IFSMObject
{
	enum STATUS { READY, ACTIVE, DIE };

	// �浹ó���� ���� �ڽ� �ų��
	irr::scene::ISceneNode* m_pMainBoxNode;

	// �ڽ��ų���� �ڽ�. �� ������ �ų��
	irr::scene::ISceneNode* m_pItemSceneNode;

	// �� ��ü
	IFSMObject* m_pBallObj;

	// ������ �Ӽ� �� ��
	//irr::core::stringw m_cItemTypeName;
	irr::s32 m_sItemType;
	irr::f32 m_fItemValue1;
	irr::f32 m_fDurationTime;

	// ������ �Ÿ��� �����ϴ� ��
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