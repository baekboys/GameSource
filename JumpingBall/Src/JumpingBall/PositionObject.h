#ifndef POSITION_OBJECT_H
#define POSITION_OBJECT_H

#include "FSMObject.h"

class CPositionObject : public IFSMObject
{
	enum STATUS { READY, ACTIVE, CLEAR ,DIE };

	// ���� �ų��
	irr::scene::ISceneNode* m_pMainNode;

	// �� ��ü
	IFSMObject* m_pBallObj;

	// ������ �Ÿ��� �����ϴ� ��
	irr::f32 m_fActiveDistanceFromBall;

	// �ð������ ���� ����
	irr::f32 m_fClearTime;

public:
	CPositionObject(void);
	virtual ~CPositionObject(void);

	inline irr::core::vector3df getPosition()
	{
		return m_pMainNode->getAbsolutePosition();
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
		return m_pMainNode;
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