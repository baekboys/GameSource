#ifndef DIRECTION_OBJECT_H
#define DIRECTION_OBJECT_H

#include "FSMObject.h"
#include "DirectionAnimator.h"

class CDirectionObject : public IFSMObject
{
	// ���� �ų��
	irr::scene::IMeshSceneNode* m_pMainNode;

	// �� �Ŵ���
	irr::scene::ISceneManager* m_pSmgr;

	// �ִϸ�����
	CDirectionAnimator* m_pDirectionAnimator;

	// �޽��� ������
	irr::f32 m_fSize;

	// �� ��ü
	IFSMObject* m_pBallObj;

public:
	CDirectionObject(void);
	~CDirectionObject(void);

	inline void setSize(irr::f32 size)
	{
		m_fSize = size;
	}

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

	bool Init(
		irr::scene::ISceneManager *pSmgr,
		CBallObject* pObj,
		irr::f32 size, irr::f32 farValueFromBall, irr::f32 farValueFromBackground,
		irr::core::vector3df endPosition
		);

	void Signal(irr::core::stringc strSignal,void *pParam);
	void Update(irr::f32 fTick);
	void reset(){}
};

#endif