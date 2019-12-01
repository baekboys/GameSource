#pragma once

#include "CharacterObject.h"
#include "NonCharacterObject.h"
#include "AppDataStructure.h"
#include "ObjectManager.h"
#include "StageManager.h"


typedef std::tr1::shared_ptr<IFSMObject> SP_IFSMObject;

class CApp : public ggf::oop::Singleton<CApp>, public irr::IEventReceiver
{
public :

	//////////////////////////////////////////////
	//											//
	//	게임에 필요한 구조체와 스마트포인터		//
	//											//
	//////////////////////////////////////////////

	//--------------------
	// 시스템데이터 구조체
	//--------------------
	SystemDATA m_systemData;
	
	//------------------------------------------
	// 주인공데이터 구조체와 스마트 포인터 객체
	//------------------------------------------
	SP_IFSMObject m_spHeroObject;
	ObjectData m_sHeroData;

	//---------------------
	// 캐릭터데이터 구조체
	//---------------------
	ObjectData m_sZombieData;
	ObjectData m_sHL2ZombieData;

	//-------------------
	// 무기데이터 구조체
	//-------------------
	ObjectData m_sPlasmaRifleData;

	// 파티클 데이터 구조체
	ObjectData m_sParticleData;
	
	//------------------------------------------------
	// 게임에 쓰일 사운드파일 이름과 사운드소스 구조체
	//------------------------------------------------
	SoundSourceDATA m_sSoundSourceData;
	SoundFileListDATA m_sSoundFileListData;


	//////////////////////////////
	//							//
	//	엔진을 위한 멤버변수들	//
	//							//
	//////////////////////////////

	//------------------
	// 일리히트 멤버변수
	//------------------
	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;

	//-------------------------
	// 일리히트 사운드 멤버변수
	//-------------------------
	irrklang::ISoundEngine* m_SoundEngine;

	//------------------
	// 물리엔진 멤버변수
	//------------------
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;

	// 크로스헤어 GUI
	irr::gui::IGUIImage* m_pCrossHairImage;

	irr::scene::ILightSceneNode* m_pLightNode;

	bool gameStart;

	bool FPSMode;

	
	//////////////////////////
	//						//
	//		데이터 관리		//
	//						//
	//////////////////////////

	// 오브젝트 및 로딩된 신노드 관리를 위한 오브젝트 매니저 객체
	CObjectManager m_objectManager;

	// 스테이지 관리 매지너 객체
	CStageManager m_stageManager;

	
	//////////////////////
	//					//
	//		초기화		//
	//					//
	//////////////////////

	bool Init();
	bool InitFactoryClass();
	bool InitGui();
	bool InitWorld();
	bool InitDynamicObject();
	bool InitAnimationDynamicObject();
	bool InitHero();
	bool InitEnemy();
	bool InitCamera();
	bool InitItem();
	bool InitSoundSource();

	void recursiveCreateDynamicObject(irr::scene::ISceneNode *pStartNode);
	void recursiveCreateAnimationDynamicObject(irr::scene::ISceneNode *pStartNode);

	bool InitStage();
	bool loadScene();
	bool loadXML();

	void CreateDieWindow();
	void CreateVictoryWindow();

	//어플리케이션 업데이트
	void Update();

	//렌더링
	void Render();

	//게임루프
	void GameLoop();

	CApp(void);
	~CApp(void);

	//이벤트 핸들러
	virtual bool OnEvent(const irr::SEvent& event);

	irr::video::ITexture* m_numberTexture[10];
	irr::gui::IGUIImage* m_image[2];
	irr::gui::IGUIInOutFader* fader;
	void DrawShellCount(bool visible, irr::s16 count);
	void FadeOutDamage() { fader->setColor(irr::video::SColor(0, 255, 0, 0)); fader->fadeIn(500); }
	void FadeOutScene() { fader->setColor(irr::video::SColor(0, 0, 0, 0)); fader->fadeOut(5000); }
	void FadeOutDieScene() { fader->setColor(irr::video::SColor(0, 255, 0, 0)); fader->fadeOut(5000); }
	void FadeInScene() { fader->setColor(irr::video::SColor(0, 0, 0, 0)); fader->fadeIn(7000); }
	void PlayBgm();

	irrklang::ISound* bgmSound;
};