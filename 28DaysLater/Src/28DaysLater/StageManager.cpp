#include "StageManager.h"
#include "App.h"

extern int random(int min, int max);

CStageManager::CStageManager(void)
: start (false), m_respawnTimeMin (7), m_respawnTimeMax(12)
{
}

CStageManager::~CStageManager(void)
{
}

void CStageManager::addRespawnNode(irr::scene::ISceneNode *pNode)
{
	m_RespawnPos.push_back(pNode->getAbsolutePosition());
}

void CStageManager::Update(irr::f32 fDelta)
{
	static irr::f32 survivalTime = 90;
	static irr::f32 survivalTick;
	static irr::f32 lightOn_time;
	static irr::f32 lightOff_time;
	static bool lightOn;
	static irr::f32 light_tick;
	static irr::f32 tick;
	static int respawn_time;

	light_tick += fDelta;
	tick += fDelta;
	survivalTick += fDelta;

	if(!start)
	{
		CApp::Get().PlayBgm();
		CApp::Get().bgmSound->setIsPaused(false);

		int i;
		for(i = 0 ; i < m_RespawnPos.size() ; i++)
		{
			irr::core::vector3df pos = m_RespawnPos[i];
			pos += irr::core::vector3df(random(1, 10), 0, random(1, 10));
			m_Objmanager->createObject(CObjectManager::OBJ_ZOMBIE, pos);
		}

		start = true;

		/*respawn_time = random(m_respawnTimeMin, m_respawnTimeMax);
		lightOn_time = random(1, 3) / 2.f;
		lightOff_time = random(1, 3) / 10.f;
		lightOn = true;
		light_tick = 0;*/
	}
	else
	{
		/*if(tick > respawn_time)
		{
			m_Objmanager->createObject(CObjectManager::OBJ_ZOMBIE, m_RespawnPos[random(0, m_RespawnPos.size())]);

			tick = 0;
			respawn_time = random(m_respawnTimeMin,  m_respawnTimeMax);
		}*/

		/*if(lightOn)
		{
			if(lightOn_time < light_tick)
			{
				CApp::Get().m_pLightNode->setVisible(false);
				lightOn = false;
				lightOn_time = random(1, 3) / 1.5f;
				light_tick = 0;
			}
		}
		else
		{
			if(lightOff_time < light_tick)
			{
				CApp::Get().m_pLightNode->setVisible(true);
				lightOn = true;
				lightOff_time = random(1, 3) / 50.f;
				light_tick = 0;
			}
		}*/

		/*if(survivalTick > survivalTime)
		{
			CApp::Get().gameStart = false;
			CApp::Get().FadeOutScene();
			CApp::Get().m_pDevice->getCursorControl()->setVisible(true);
			CApp::Get().m_pCrossHairImage->setVisible(false);
			CApp::Get().CreateVictoryWindow();
		}*/
	}
}