#ifndef PORTAL_OBJECT_H
#define PORTAL_OBJECT_H

#include "FSMObject.h"

class CPortalObject : public IFSMObject
{
	enum STATUS { READY, ACTIVE, DIE };

	// ���� ��Ż ���. empty���
	irr::scene::ISceneNode* m_pMainPortalNode;

	// �Ա�, �ⱸ��Ż ���
	irr::scene::ISceneNode* m_pInPortalNode;
	irr::scene::ISceneNode* m_pOutPortalNode;

	// �� ��ü
	IFSMObject* m_pBallObj;

	// ������ �Ÿ��� �����ϴ� ��
	irr::f32 m_fActiveDistanceFromBall;

public:
	CPortalObject(void);
	virtual ~CPortalObject(void);

	inline irr::core::vector3df getPosition()
	{
		return m_pMainPortalNode->getAbsolutePosition();
	}

	inline bool isDie()
	{
		return false;
	}

	inline btRigidBody* getRigidBody()
	{
		return NULL;
	}
	
	inline irr::scene::ISceneNode* getSceneNode()
	{
		return m_pMainPortalNode;
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
	void reset(){}
};

#endif