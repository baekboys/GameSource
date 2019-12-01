#pragma once

#include "WeaponObject.h"

class CPlasmaRifleObject : public IWeaponObject
{
	irr::f32 m_maxZoomAngle;
	irr::f32 m_minZoomAngle;
	irr::f32 m_basicZoomAngle;
	irr::f32 m_zoomSpeedValue;

	irr::f32 m_reactionHorizontalAngle;
	irr::f32 m_reactionVerticalAngle;
	irr::f32 m_recoveryAngle;

	// 무기의 화염효과와 광원효과를 위한 노드와 변수들
	irr::scene::ICameraSceneNode* m_pCamNode;
	irr::scene::ISceneNode* m_pFireSpotNode;
	irr::video::ITexture* m_pFlashTexture;
	irr::core::dimension2df m_flashSize;
	irr::f32 m_flashDurationTime;
	irr::f32 m_flashTick;
	irr::video::SLight m_light;
	irr::scene::IBillboardSceneNode* m_pFlashEffectNode;
	irr::scene::ILightSceneNode* m_pLightEffectNode;


	// 신매니저
	irr::scene::ISceneManager* m_pSmgr;
	
	// 무기의 사운드소스
	irrklang::ISoundEngine* m_SoundEngine;
	irrklang::ISoundSource* m_sFire;
	irrklang::ISoundSource* m_sReload1;
	irrklang::ISoundSource* m_sReload2;
	irrklang::ISoundSource* m_sDryFire;
	irrklang::ISoundSource* m_sSelect;
	irrklang::ISoundSource* m_sPutaway;
	irrklang::ISoundSource* m_sZoom;

	bool m_isZoom;
	
	// 무기의 상태 열거자
	enum STATUS { WORLD, SELECT, STAND, MOVE, RUN, ATTACK, DRYFIRE, RELOAD, RELOAD2, PUTAWAY, ZOOMIN, ZOOMOUT, DIE };

	inline bool isVisibleFalshEffect()
	{
		if(m_pLightEffectNode->isVisible())
			return true;
		return false;
	}

	inline void setVisibleFlashEffect(bool flash, bool light)
	{
		m_pFlashEffectNode->setVisible(flash);
		m_pLightEffectNode->setVisible(light);
	}

	void makeCartrige();

	ObjectData* m_xmlData;

public :
	CPlasmaRifleObject()
		: m_pCamNode(NULL), m_pFireSpotNode(NULL), m_pSmgr(NULL), m_pFlashTexture(NULL), m_SoundEngine(NULL),
		m_maxZoomAngle(5), m_minZoomAngle(20), m_basicZoomAngle(72), m_zoomSpeedValue(0.001f), m_isZoom(false),
		m_flashDurationTime(100), m_reactionHorizontalAngle(0), m_reactionVerticalAngle(0), m_recoveryAngle(0),
		m_flashTick(100)
	{
		SetStatus(WORLD);
	}
	virtual ~CPlasmaRifleObject()
	{
	}

	void attack(std::vector<PickingResultDATA> &pickingResult);
	void Update(irr::f32 fDelta);
	void selectThisWeapon();
	void deselectThisWeapon();
	bool zoomIN(irr::f32 fDelta);
	bool zoomOUT(irr::f32 fDelta);
	bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);
	bool canAttack();
	bool canReload();
	void putawayThisWeapon(irr::scene::ISceneNode* pNode, irr::core::vector3df pos);

	void playEffectSound(STATUS type);

	bool canPutaway()
	{
		return !m_isZoom;
	}

	inline bool isZoom()
	{
		return m_isZoom;
	}

	inline bool isDie()
	{
		if ( GetStatus() == DIE )
			return true;
		return false;
	}

	inline void move()
	{
		if(GetStatus() == STAND || GetStatus() == RUN)
		{
			SetStatus(MOVE);
		}
	}

	inline void run()
	{
		if(GetStatus() == STAND || GetStatus() == MOVE)	
		{
			SetStatus(RUN);
		}
	}

	inline bool canSpecialSkill()
	{
		if (GetStatus() != ZOOMIN || GetStatus() != ZOOMOUT)
			return true;
		return false;
	}

	inline void reloadThisWeapon()
	{
		if (canReload())
			SetStatus(RELOAD);
	}

	inline void stand()
	{
		SetStatus(STAND);
	}

	inline bool isLimitedShell()
	{
		return true;
	}

	inline irr::u32 hasSpecialSkill()
	{
		return 1;
	}

	inline irr::u32 specialSkill()
	{
		playEffectSound(ZOOMIN);

		if ( m_isZoom )
		{
			SetStatus(ZOOMOUT);
			return 0;
		}

		SetStatus(ZOOMIN);
		return 1;
	}
};