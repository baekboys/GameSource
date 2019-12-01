#pragma once

#include "NonCharacterObject.h"

class CCartrigeObject : public INonCharacterObject
{
	enum STATUS
	{
		AIR, LAND, DIE
	};

	// 탄피가 어느정도 힘으로 튀어오를지 정해주는 값
	irr::f32 m_force;

	// 탄피의 사이즈
	irr::f32 m_radius;
	irr::f32 m_length;

	// 탄피의 지속시간
	irr::f32 m_durationTime;
	irr::f32 m_timer;

	irr::scene::ISceneNode* m_pMainNode;
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;
	irrklang::ISoundSource* m_pCartrigeSound;
	irrklang::ISoundEngine* m_pSoundEngine;
	irr::scene::ISceneNode* CreateCartrigeScene(irr::f32 radius, irr::f32 length, irr::video::SColor color);

	// 충돌체크
	bool checkCollision();

public:
	CCartrigeObject(void);
	~CCartrigeObject(void);

	// 순수가상함수
	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);
	virtual bool isDie();
	virtual irr::core::vector3df getPosition() { return m_pMainNode->getAbsolutePosition(); }

	// 초기화 코드
	bool Init(irr::core::vector3df startPosition);
	inline void applyImpulse(irr::core::vector3df vDir) 
	{
		m_pAnimator->applyImpulse(vDir * m_force, irr::core::vector3df(0, 0, 0.5f));
	}
};
