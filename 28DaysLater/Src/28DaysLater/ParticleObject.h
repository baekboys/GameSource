#pragma once

#include "NonCharacterObject.h"

class CParticleObject : public INonCharacterObject
{
	struct SParticle
	{
		irr::scene::ISceneNode* spotNode;
		irr::scene::CBulletObjectAnimator* animator;
		irr::f32 durationTime;
		irr::core::vector3df position;
		irr::core::vector3df direction;
	};

	enum STATUS
	{
		ACTIVE,
		DIE
	};

	enum TYPE
	{
		PARTICLE_METAL,
		PARTICLE_BLOOD,
	};

	irr::f32 m_fTimer;

	irr::core::list<SParticle> m_ParticleList;

	SParticle CreateParticleObj(irr::core::vector3df pos, irr::core::vector3df dir, irr::u32 type);
	
public:
	CParticleObject(void);
	~CParticleObject(void);

	// 순수가상함수
	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);
	virtual bool isDie();

	// 초기화
	bool Init(	irr::core::vector3df pos, irr::core::vector3df dir, irr::u32 type);
};