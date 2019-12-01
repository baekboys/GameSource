#pragma once

#include "FSMObject.h"
#include "CBulletWorldAnimator.h"
#include "BlockNode.h"

class CBlockObject : public IFSMObject
{
	enum BLOCK_STATUS
	{
		READY,
		ANIMATION,
		RETURN
	};

	// ���� �ų��
	irr::scene::jz3d::CBlockNode* m_pMainSceneNode;

	irr::scene::ISceneNodeAnimator* m_pBulletBySceneAnimator;
	
	// �� �Ӽ�
	irr::u32 m_uBlockProperty;

	irr::f32 m_fBlockPropertyValue[3];

	// motion State
	btMotionState* m_pMotionState;

	// ��ü
	btRigidBody* m_pRigidBody;

	// ���忡�� ���� �Ǿ��°� Ȯ���ϴ� �÷���
	bool m_bIsRemoveFromWorld;

	// ���忡�� ����
	void removeRigidBodyFromWorld();

	// ���� �ִϸ�����
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;

	// �� ��ü
	IFSMObject* m_pBallObj;

	bool compareVectorsByEpsilon(const irr::core::vector3df& val1, const irr::core::vector3df& val2, irr::f32 epsilon);

public:
	CBlockObject(void);
	~CBlockObject(void);

	// implement abstract class
	irr::core::vector3df getPosition();
	bool isDie();
	void Signal(irr::core::stringc strSignal,void *pParam);
	void Update(irr::f32 fTick);	
	btRigidBody* getRigidBody();
	irr::scene::ISceneNode* getSceneNode();
	irr::scene::CBulletChracterAnimator* getAnimator();
	bool isBall();
	void setTargetObject(IFSMObject* obj);
	virtual void reset();

	bool Init(irr::scene::jz3d::CBlockNode* pNode, irr::scene::CBulletWorldAnimator* pWorldAnimator);
};
