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
	//	���ӿ� �ʿ��� ����ü�� ����Ʈ������		//
	//											//
	//////////////////////////////////////////////

	//--------------------
	// �ý��۵����� ����ü
	//--------------------
	SystemDATA m_systemData;
	
	//------------------------------------------
	// ���ΰ������� ����ü�� ����Ʈ ������ ��ü
	//------------------------------------------
	SP_IFSMObject m_spHeroObject;
	ObjectData m_sHeroData;

	//---------------------
	// ĳ���͵����� ����ü
	//---------------------
	ObjectData m_sZombieData;
	ObjectData m_sHL2ZombieData;

	//-------------------
	// ���ⵥ���� ����ü
	//-------------------
	ObjectData m_sPlasmaRifleData;

	// ��ƼŬ ������ ����ü
	ObjectData m_sParticleData;
	
	//------------------------------------------------
	// ���ӿ� ���� �������� �̸��� ����ҽ� ����ü
	//------------------------------------------------
	SoundSourceDATA m_sSoundSourceData;
	SoundFileListDATA m_sSoundFileListData;


	//////////////////////////////
	//							//
	//	������ ���� ���������	//
	//							//
	//////////////////////////////

	//------------------
	// �ϸ���Ʈ �������
	//------------------
	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;

	//-------------------------
	// �ϸ���Ʈ ���� �������
	//-------------------------
	irrklang::ISoundEngine* m_SoundEngine;

	//------------------
	// �������� �������
	//------------------
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;

	// ũ�ν���� GUI
	irr::gui::IGUIImage* m_pCrossHairImage;

	irr::scene::ILightSceneNode* m_pLightNode;

	bool gameStart;

	bool FPSMode;

	
	//////////////////////////
	//						//
	//		������ ����		//
	//						//
	//////////////////////////

	// ������Ʈ �� �ε��� �ų�� ������ ���� ������Ʈ �Ŵ��� ��ü
	CObjectManager m_objectManager;

	// �������� ���� ������ ��ü
	CStageManager m_stageManager;

	
	//////////////////////
	//					//
	//		�ʱ�ȭ		//
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

	//���ø����̼� ������Ʈ
	void Update();

	//������
	void Render();

	//���ӷ���
	void GameLoop();

	CApp(void);
	~CApp(void);

	//�̺�Ʈ �ڵ鷯
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