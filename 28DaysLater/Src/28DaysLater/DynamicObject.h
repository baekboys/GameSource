#pragma once

#include "NonCharacterObject.h"

class CDynamicObject : public INonCharacterObject
{
	enum TYPE
	{
		DYNAMIC_METAL,
		DYNAMIC_WOOD,
		DYNAMIC_BALL
	};

	TYPE m_type;
	irr::scene::ISceneNode* m_pMainNode;

	irrklang::ISoundSource* m_pCollisionSound;
	irrklang::ISoundEngine* m_pSoundEngine;

public:
	CDynamicObject(void);
	~CDynamicObject(void);

	// 순수가상함수
	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);
	virtual bool isDie();

	virtual irr::core::vector3df getPosition() { return m_pMainNode->getAbsolutePosition(); }

	bool Init(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode);
};
