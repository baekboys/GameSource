#pragma once

#include "NonCharacterObject.h"

class CAnimationDynamicObject : public INonCharacterObject
{
	enum TYPE
	{
		DYNAMIC_METAL,
		DYNAMIC_WOOD,
		DYNAMIC_BALL
	};

	enum STATUS
	{
		ACTIVE,
		BREAK,
		DIE
	};

	TYPE m_type;
	irr::f32 m_hp;
	irrklang::ISoundSource* m_pCollisionSound;
	irrklang::ISoundSource* m_pBreakSound;
	irrklang::ISoundEngine* m_pSoundEngine;

public:
	CAnimationDynamicObject(void);
	~CAnimationDynamicObject(void);

	// 순수가상함수
	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);
	virtual bool isDie();

	bool Init(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode);
};
