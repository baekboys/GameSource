#pragma once

#include "ObjectManager.h"

class CStageManager
{
	CObjectManager* m_Objmanager;
	irr::core::array<irr::core::vector3df> m_RespawnPos;

	bool start;
	irr::s32 m_respawnTimeMin;
	irr::s32 m_respawnTimeMax;

public:
	CStageManager(void);
	~CStageManager(void);

	void setObjectManager(CObjectManager* obj_manager) { m_Objmanager = obj_manager; }
	void setRespawnTime(irr::s32 timeMin, irr::s32 timeMax) { m_respawnTimeMin = timeMin; m_respawnTimeMax = timeMax; }
	void addRespawnNode(irr::scene::ISceneNode* pNode);

	void Update(irr::f32 fDelta);
};
