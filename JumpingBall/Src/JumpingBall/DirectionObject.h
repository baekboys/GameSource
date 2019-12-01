#ifndef DIRECTION_OBJECT_H
#define DIRECTION_OBJECT_H

#include "FSMObject.h"
#include "DirectionAnimator.h"

class CDirectionObject : public IFSMObject
{
	// 메인 신노드
	irr::scene::IMeshSceneNode* m_pMainNode;

	// 신 매니저
	irr::scene::ISceneManager* m_pSmgr;

	// 애니메이터
	CDirectionAnimator* m_pDirectionAnimator;

	// 메쉬의 사이즈
	irr::f32 m_fSize;

	// 공 객체
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