#ifndef PORTAL_OBJECT_H
#define PORTAL_OBJECT_H

#include "FSMObject.h"

class CPortalObject : public IFSMObject
{
	enum STATUS { READY, ACTIVE, DIE };

	// 메인 포탈 노드. empty노드
	irr::scene::ISceneNode* m_pMainPortalNode;

	// 입구, 출구포탈 노드
	irr::scene::ISceneNode* m_pInPortalNode;
	irr::scene::ISceneNode* m_pOutPortalNode;

	// 공 객체
	IFSMObject* m_pBallObj;

	// 공과의 거리를 설정하는 값
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