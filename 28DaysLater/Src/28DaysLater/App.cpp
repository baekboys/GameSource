#include "App.h"
#include "ObjectList.h"

int random(int min, int max)
{
	return (rand()%(max-min)) + min;
}

HANDLE g_hThread_console;
DWORD g_ThreadID_console;
std::string g_strMessage;

//DWORD WINAPI ConsoleThread(LPVOID lpPram)
//{
//	while(1)
//	{	
//		char str[256];
//		std::cin.getline(str,256);					
//		g_strMessage = str;		
//
//		if(g_strMessage == "exit")
//			return 0;
//	}
//	return 0;
//}

void recursivePhysicsReg(irr::scene::ISceneManager *pSmgr,irr::s32 worldID,irr::scene::ISceneNode *pStartNode)
{
	if(pStartNode == 0)
	{
		pStartNode = pSmgr->getRootSceneNode();
	}

	irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pStartNode->getChildren().begin();

	for(;it != pStartNode->getChildren().end();it++)
	{
		recursivePhysicsReg(pSmgr,worldID,*it);


		switch( (*it)->getType())
		{
		case irr::scene::jz3d::CCollusionMngNode::TypeID:
			{
			}
			break;
		case irr::scene::jz3d::CBulletPhysicsObjectNode::TypeID:
			{
				irr::scene::CBulletAnimatorManager *ani_factory =
					static_cast<irr::scene::CBulletAnimatorManager *>(ggf::util::findAnimatorFactory(pSmgr,
					irr::scene::ESNAT_BULLET_OBJECT));

				irr::scene::jz3d::CBulletPhysicsObjectNode *pNode = irr::scene::jz3d::CBulletPhysicsObjectNode::upcast((*it));	
				pNode->register2BulletPhysicsWorld(ani_factory->getBulletWorldByID(worldID)->getWorld());
			}
			break;

		case irr::scene::jz3d::CTiledPlaneNormalMapNode::TypeID:
			{
				irr::scene::jz3d::CTiledPlaneNormalMapNode *pNode = irr::scene::jz3d::CTiledPlaneNormalMapNode::upcast((*it));
				pNode->makeNormalMapping();
			}
			break;

		case irr::scene::ESNT_MESH:
		case irr::scene::jz3d::CTiledPlaneNode::TypeID:			
			break;
		}


	}
}

void CApp::recursiveCreateDynamicObject(irr::scene::ISceneNode *pStartNode)
{
	irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pStartNode->getChildren().begin();

	for(;it != pStartNode->getChildren().end();it++)
	{
		recursiveCreateDynamicObject(*it);

		switch( (*it)->getType())
		{
		case irr::scene::jz3d::CBulletPhysicsObjectNode::TypeID:
			{
				m_objectManager.createDynamicObject( static_cast<irr::scene::jz3d::CBulletPhysicsObjectNode*>(*it) );
			}
			break;
		}
	}
}

void CApp::recursiveCreateAnimationDynamicObject(irr::scene::ISceneNode *pStartNode)
{
	irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pStartNode->getChildren().begin();

	for(;it != pStartNode->getChildren().end();it++)
	{
		recursiveCreateAnimationDynamicObject(*it);

		switch( (*it)->getType())
		{
		case irr::scene::jz3d::CBulletPhysicsObjectNode::TypeID:
			{
				m_objectManager.createAnimationDynamicObject( static_cast<irr::scene::jz3d::CBulletPhysicsObjectNode*>(*it) );
			}
			break;
		}
	}
}

CApp::CApp(void)
{
	//::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //�޸𸮸� Ž��
	//�޸𸮸� �߻����� Ž��.
	//_CrtSetBreakAlloc(125);

	//m_objectMgr.InitHashTable(257);
}

CApp::~CApp(void)
{

	if ( !m_systemData.onlyCompile )
	{
		m_pDevice->drop();

		if ( m_systemData.gameInit )
		{
			m_pWorldAnimator->drop();
			m_pBulletPhysicsFactory->drop();
		}
	}
}

bool CApp::Init()
{
	//////////////////////////////
	//							//
	//	�ý��� ���� �о����	//
	//							//
	//////////////////////////////
	
	//------------------
	// XML���� ��ü ����
	//------------------
	CXmlReader reader;

	//-------------------------
	// �ý���XML������ �д´�.
	//-------------------------
	//std::cout << "�ý���XML���� �ε� �õ�" << std::endl;
	if (!reader.readSystemDATA("../../res/System/XML/system.xml", m_systemData))
	{
		//// �ܼ��� �����ϸ��� ���� �����̸�
		//if ( m_systemData.onlyCompile )
		//	std::cout << "�ܼ��� �����ϸ� �ϴ� �������Դϴ�." << std::endl;
		//// �ý���XML������ �дµ� �����Ͽ��ٸ�
		//else
		//	std::cout << "�ý���XML������ �дµ� �����Ͽ����ϴ�. XML���������� Ȯ���ϼ���" << std::endl;
		
		return false;
	}
	//std::cout << "�ý���XML���� �ε� ����" << std::endl;



	//////////////////////////////
	//							//
	//	���� ����̽� �ʱ�ȭ	//
	//							//
	//////////////////////////////
	{
		//�ϸ���Ʈ����
		m_pDevice = irr::createDevice(
			m_systemData.deviceType,
			irr::core::dimension2du(m_systemData.screen_size[0],m_systemData.screen_size[1]), //1.6������
			m_systemData.bits,
			m_systemData.full_screen,
			m_systemData.stencilBuffer,
			m_systemData.vsync,
			this
			);

		m_pVideo = m_pDevice->getVideoDriver();
		m_pSmgr = m_pDevice->getSceneManager();
		m_pGuiEnv = m_pDevice->getGUIEnvironment();

		m_pDevice->getFileSystem()->changeWorkingDirectoryTo(m_systemData.working_directory.c_str());

		//���콺 ������ ����
		m_pDevice->getCursorControl()->setVisible(m_systemData.mouse_visible);

		//���忣��
		m_SoundEngine = irrklang::createIrrKlangDevice();
		m_SoundEngine->setDefault3DSoundMinDistance(m_systemData.soundMinDistance);
		m_SoundEngine->setDefault3DSoundMaxDistance(200);
	}

	//�ܼ�â�� ����� ���� ������ ����
	/*if(m_systemData.consoleCMD)
	{
		g_hThread_console = ::CreateThread(NULL,0,ConsoleThread,0,0,&g_ThreadID_console);
	}*/

	// �����Լ� �õ� ����
	{
		srand(static_cast<irr::u32>(time(NULL)));
	}


	//////////////////////////
	//						//
	//	���� �ʱ�ȭ ���	//
	//						//
	//////////////////////////

	//---------------------------
	// �ʱ�ȭ�� �������� �ʴ´ٸ�
	//---------------------------
	if ( !m_systemData.gameInit )
	{
		//std::cout << "������ �ʱ�ȭ�� ���� �ʴ� ���� ���õǾ��ֽ��ϴ�." << std::endl;

		return false;
	}

	InitFactoryClass();

	loadXML();

	loadScene();

	InitSoundSource();

	InitWorld();

	InitCamera();

	InitHero();

	InitDynamicObject();

	InitAnimationDynamicObject();

	InitStage();

	//InitEnemy();

	InitItem();

	InitGui();

	//���׶��� ��ü ��������
	m_pSmgr->getRootSceneNode()->OnAnimate(0);

	gameStart = true;

	return true;
}

bool CApp::InitFactoryClass()
{
	//����� �߰� Ȯ�� ���丮���
	irr::scene::jz3d::CJZ3DSceneNodeFactory *factory = new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(factory);
	factory->drop();

	CIrrKlangSceneNodeFactory* irrkangFactory = new CIrrKlangSceneNodeFactory(m_SoundEngine, m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(irrkangFactory);
	irrkangFactory->drop();

	//�����ִϸ����� ���丮���
	m_pBulletPhysicsFactory = new irr::scene::CBulletAnimatorManager(m_pSmgr);

	if ( factory != NULL && m_pBulletPhysicsFactory != NULL)
		return true;
	return false;
}

bool CApp::InitWorld()
{
	//---------------------
	// ���� �ִϸ����� �߰�
	//---------------------
	irr::scene::ISceneNode *pNode = m_pSmgr->addEmptySceneNode();
	pNode->setName("usr/scene/physics/world/1");
	irr::scene::CBulletWorldAnimatorParams worldParams;		
	worldParams.bSoft = false;

	m_pWorldAnimator =
		m_pBulletPhysicsFactory->createBulletWorldAnimator(
		m_pSmgr,
		pNode,
		&worldParams
		);

	pNode->addAnimator(m_pWorldAnimator);


	//--------------------
	// ��������ó�� �ʱ�ȭ
	//--------------------
	
	{
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("land_physics");
		irr::scene::jz3d::CBulletPhysicsObjectNode* pLandNode = static_cast<irr::scene::jz3d::CBulletPhysicsObjectNode*>(pNode);
		if ( pLandNode != NULL)
		{
			// ���� �ִϸ����� ���̱�
			irr::scene::CBulletObjectAnimator *pAnim = m_pBulletPhysicsFactory->createBulletObjectAnimator(
				m_pSmgr,
				pLandNode,
				m_pWorldAnimator->getID(),
				&(pLandNode->m_GeometryInfo),
				&(pLandNode->m_PhysicsParam)
				);

			pLandNode->addAnimator(pAnim);
			pAnim->drop();
		}
	}

	{
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("static_obj");
		recursivePhysicsReg(m_pSmgr, m_pWorldAnimator->getID() ,pNode);
	}

	// ��ָ� �ʱ�ȭ
	{
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("normalmap");
		irr::scene::jz3d::CTiledPlaneNormalMapNode* pNormalNode = static_cast<irr::scene::jz3d::CTiledPlaneNormalMapNode*>(pNode);
		pNormalNode->makeNormalMapping();
	}

	return true;
}

bool CApp::InitHero()
{
	//---------------
	// ���ΰ� �����
	//---------------
	irr::scene::ISceneNode *pStartPos = m_pSmgr->getSceneNodeFromName("usr/trigger/start");
	m_spHeroObject = m_objectManager.createObject(CObjectManager::OBJ_HERO, pStartPos->getAbsolutePosition());
	return true;
}

bool CApp::InitDynamicObject()
{
	irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("dynamic_obj");

	recursiveCreateDynamicObject(pNode);

	return true;
}

bool CApp::InitAnimationDynamicObject()
{
	irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("dynamic_animation");

	recursiveCreateAnimationDynamicObject(pNode);

	return true;
}

bool CApp::InitEnemy()
{
	if(!m_objectManager.makedHero())
		return false;

	//for ( int i = 0; i < 3 ; i++)
	//{
	//	//--------------------
	//	// ���� ��ü �����
	//	//--------------------
	//	m_objectManager.createObject(CObjectManager::OBJ_ZOMBIE, irr::core::vector3df(random(-300, 300),10,random(-300, 300)));
	//}

	//irr::scene::ISceneNode *pStartPos = m_pSmgr->getSceneNodeFromName("usr/trigger/respawn");
	//	�׽�Ʈ �ϱ�
	//for ( int i = 0; i < 1 ; i++)
	//{
	//	//--------------------
	//	// ���� ��ü �����
	//	//--------------------
	//	m_objectManager.createObject(CObjectManager::OBJ_ZOMBIE, pStartPos->getAbsolutePosition());
	//}

	//m_objectManager.createObject(CObjectManager::OBJ_HL2ZOMBIE, pStartPos->getAbsolutePosition());


	return true;
}

bool CApp::InitStage()
{
	m_stageManager.setObjectManager(&m_objectManager);
	m_stageManager.setRespawnTime(m_systemData.data[0], m_systemData.data[1]);

	irr::scene::ISceneNode *pRespawnNode = m_pSmgr->getSceneNodeFromName("usr/trigger/respawn");
	irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pRespawnNode->getChildren().begin();

	for(;it != pRespawnNode->getChildren().end();it++)
	{
		m_stageManager.addRespawnNode( (*it) );
	}

	return true;
}

bool CApp::InitItem()
{
	if(!m_objectManager.makedHero())
		return false;

	irr::scene::ISceneNode *pStartPos = m_pSmgr->getSceneNodeFromName("usr/trigger/start");

	//----------------------------
	// �ö�������� ��ü �����
	//----------------------------
	m_objectManager.createObject(CObjectManager::OBJ_PLASMA_RIFLE, pStartPos->getAbsolutePosition() + irr::core::vector3df(0,0,50));
 
	return true;
}

bool CApp::InitGui()
{
	////������ ����Ʈ ǥ�ÿ� ������
	//irr::gui::IGUIStaticText *pstextFPS = m_pGuiEnv->addStaticText(L"Frame rate",irr::core::rect<irr::s32>(0,0,100,20),true,true,0,100,true);

	
	irr::gui::IGUIImage* pGui = m_pGuiEnv->addImage(m_pVideo->getTexture("System/GUI/crosshair 5.png"), irr::core::vector2d<s32>(368,268), true, 0, 101, 0);
	m_pCrossHairImage = pGui;

	// ���̴� GUI
	{
		irr::core::recti size(0,0,m_pVideo->getScreenSize().Width,m_pVideo->getScreenSize().Height);
		fader = m_pGuiEnv->addInOutFader(&size, 0, 0);
		fader->setVisible(true);
	}

	irr::u32 center = m_pVideo->getScreenSize().Width / 2;
	irr::u32 down = m_pVideo->getScreenSize().Height;

	// Ÿ�̸� ��ư 10�ڸ� - ��ư setEnable(false) �ܼ��� �̹����� �׸��� ���� �뵵
	{
		irr::gui::IGUIImage* image = m_pGuiEnv->addImage(
			irr::core::recti(center - 40 , down - 55, center, down -5),
			0,
			0,
			0
			);

		image->setVisible(false);
		image->setScaleImage(true);
		image->setUseAlphaChannel(true);

		// ������ ����
		m_image[0] = image;
	}

	// Ÿ�̸� ��ư 1�ڸ� - ��ư setEnable(false) �ܼ��� �̹����� �׸��� ���� �뵵
	{
		irr::gui::IGUIImage* image = m_pGuiEnv->addImage(
			irr::core::recti(center, down - 55, center + 40, down -5),
			0,
			0,
			0
			);

		image->setVisible(false);
		image->setScaleImage(true);
		image->setUseAlphaChannel(true);

		// ������ ����
		m_image[1] = image;
	}

	// �ؽ��� �ҷ�����
	{
		irr::core::stringc directory("System/GUI/");
		irr::core::stringc extension(".bmp");

		irr::u32 i;
		for(i = 0 ; i < 10 ; i++)
		{
			irr::core::stringc fileName = directory + irr::core::stringc(i) + extension;
			irr::video::ITexture* texture = m_numberTexture[i] = m_pVideo->getTexture(fileName.c_str());
			if(!texture)
				return false;
		}
	}

	return true;
}

bool CApp::InitCamera()
{
	irr::scene::ICameraSceneNode *pCam = m_pSmgr->addCameraSceneNode(0,irr::core::vector3df(0,10,-10),irr::core::vector3df(0,0,1));

	return true;
}

bool CApp::InitSoundSource()
{
	// ���ΰ� ����ҽ�
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.heroList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.heroList[i].c_str());
			m_sSoundSourceData.hero.push_back(source);
		}
	}


	// �ö�������� ����ҽ�
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.plasmarifleList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.plasmarifleList[i].c_str());
			m_sSoundSourceData.plasmarifle.push_back(source);
		}
	}

	// ���� ����ҽ�
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.zombieList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.zombieList[i].c_str());
			m_sSoundSourceData.zombie.push_back(source);
		}
	}

	// �������� ���� �ҽ�
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.stageList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.stageList[i].c_str());
			m_sSoundSourceData.stage.push_back(source);
		}
	}

	return true;
}

bool CApp::loadScene()
{
	//-----------------
	// �������� �� �ε�
	//-----------------
	{
		bool loading = m_pSmgr->loadScene(m_systemData.stage_fileName.c_str());
		if ( !loading )
		{
			//std::cout << "�������� �ų�� �ε� ����"<< std::endl;
			return false;
		}
		//std::cout << "�������� �ų�� �ε� ����"<< std::endl;
	}


	//----------------
	// ���ΰ� �� �ε�
	//----------------
	{
		bool sceneLoading = m_pSmgr->loadScene(m_systemData.hero_fileName.c_str());
		if ( !sceneLoading )
		{
			//std::cout << "���ΰ� �ų�� �ε� ����"<< std::endl;
			return false;
		}
		//std::cout << "���ΰ� �ų�� �ε� ����"<< std::endl;

		// ������Ʈ �Ŵ������� �ų�� ���� ����
		m_objectManager.addSceneNode(CObjectManager::OBJ_HERO, m_pSmgr->getSceneNodeFromName(m_systemData.hero_SceneName.c_str()));
	}


	//--------------
	// ���� �� �ε�
	//--------------
	{
		bool sceneLoading = m_pSmgr->loadScene(m_systemData.zombie_fileName.c_str());
		if ( !sceneLoading )
		{
			//std::cout << "���� �ų�� �ε� ����"<< std::endl;
			return false;
		}
		//std::cout << "���� �ų�� �ε� ����"<< std::endl;

		// ������Ʈ �Ŵ������� �ų�� ���� ����
		m_objectManager.addSceneNode(CObjectManager::OBJ_ZOMBIE, m_pSmgr->getSceneNodeFromName(m_systemData.zombie_SceneName.c_str()));
	}

	//------------------------
	// �ö�� ������ �� �ε�
	//------------------------
	{
		bool sceneLoading = m_pSmgr->loadScene(m_systemData.plasmarifle_fileName.c_str());
		if ( !sceneLoading )
		{
			//std::cout << "�ö�� ������ �ų�� �ε� ����"<< std::endl;
			return false;
		}

		// ������Ʈ �Ŵ������� �ų�� ���� ����
		m_objectManager.addSceneNode(CObjectManager::OBJ_PLASMA_RIFLE, m_pSmgr->getSceneNodeFromName(m_systemData.plasmarifle_SceneName.c_str()));
	}

	// �� �ų�� �ε�
	{
		irr::scene::ILightSceneNode* pLightNode = static_cast<irr::scene::ILightSceneNode*> (m_pSmgr->getSceneNodeFromId(150));
		m_pLightNode = pLightNode;
	}
	

	return true;
}

bool CApp::loadXML()
{
	CXmlReader reader;

	//--------------------
	// ���ΰ� XML���� �ε�
	//--------------------
	{
		//std::cout << "���ΰ�XML���� �б�õ�" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.heroXML.c_str(), m_sHeroData);

		if ( !xmlLoading )
			return false;
	}


	//--------------------
	// ���� XML���� �ε�
	//--------------------
	{
		//std::cout << "����XML���� �б�õ�" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.zombieXML.c_str(), m_sZombieData);

		if ( !xmlLoading )
			return false;
	}

	//----------------------------
	// �ö�������� XML���� �ε�
	//----------------------------
	{
		//std::cout << "�ö��������XML���� �б�õ�" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.plasmarifleXML.c_str(), m_sPlasmaRifleData);

		if ( !xmlLoading )
			return false;
	}

	//----------------------------
	// ��ƼŬ XML���� �ε�
	//----------------------------
	{
		//std::cout << "��ƼŬXML���� �б�õ�" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.particleXML.c_str(), m_sParticleData);

		if ( !xmlLoading )
			return false;
	}

	//----------------------------
	// �������ϰ��� XML���� �ε�
	//----------------------------
	{
		//std::cout << "�������ϸ������� XML���� �б�õ�" << std::endl;
		bool xmlLoading = reader.readSoundFileDATA(m_systemData.soundfileXML.c_str(), m_sSoundFileListData);

		if ( !xmlLoading )
			return false;
	}
	
	
	return true;
}

void CApp::Update()
{
	static irr::u32 uLastTick=0;
	irr::u32 uTick = m_pDevice->getTimer()->getTime();      
	irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f; //��Ÿ�� ���ϱ�
	uLastTick = uTick;

	if(!FPSMode)
	{
		// ���콺 ��ġ ����
		irr::core::position2di mouse_pos = m_pDevice->getCursorControl()->getPosition();
		irr::core::dimension2du screensize = m_pVideo->getScreenSize();

		if(mouse_pos.X < 0)
			m_pDevice->getCursorControl()->setPosition(0, mouse_pos.Y);
		else if (mouse_pos.X > screensize.Width)
			m_pDevice->getCursorControl()->setPosition(screensize.Width, mouse_pos.Y);

		if(mouse_pos.Y < 0)
			m_pDevice->getCursorControl()->setPosition(mouse_pos.X, 0);
		else if (mouse_pos.Y > screensize.Height)
			m_pDevice->getCursorControl()->setPosition(mouse_pos.X, screensize.Height);


		m_pCrossHairImage->setRelativePosition( m_pDevice->getCursorControl()->getPosition() - irr::core::position2di(33,33));
	}

	//�����ӷ���Ʈ ����
	{
		/*wchar_t wszbuf[256];
		swprintf(wszbuf,L"Frame rate : %d\n TriAngle: %d",m_pVideo->getFPS(),
			m_pVideo->getPrimitiveCountDrawn());			
		m_pGuiEnv->getRootGUIElement()->getElementFromId(100)->setText(wszbuf);*/
	}

	if(gameStart)
	{
		//------------------------------
		// �������� �Ŵ��� ������Ʈ
		//------------------------------
		{
			m_stageManager.Update(fDelta);
		}

		//-------------------------
		// ������Ʈ �Ŵ��� ������Ʈ
		//-------------------------
		{
			m_objectManager.Update(fDelta);
		}

		//----------
		// ����ó��
		//----------
		{
			irr::u32 dt = (irr::u32)(fDelta * 1000000.f); //�鸸���� ���ʴ����� ƽ��					
			m_pBulletPhysicsFactory->OnUpdate(dt);
		}
	}
	
	////-----------------
	//// �ָܼ�ɾ� ó��
	////-----------------
	//{

	//	// ��ɾ ������� ���� ��츸 ó��
	//	if(g_strMessage != "")
	//	{

	//		std::vector<std::string> token;

	//		tokenize(g_strMessage, token, " ");

	//		if(token[0] == "cmd")
	//		{
	//			std::cout << "pos, poschange, make zombie, make plasmarifle" << std::endl;
	//		}
	//		else if(token[0] == "pos")
	//		{
	//			if(token[1] == "hero")
	//			{
	//				irr::core::vector3df pos =  m_spHeroObject->getPosition();
	//				std::cout << "���ΰ� ��ġ : " << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
	//			}
	//			else 
	//			{
	//				irr::scene::ISceneNode* pNode =  m_pSmgr->getSceneNodeFromName(token[1].c_str());
	//				
	//				if(pNode)
	//				{
	//					irr::core::vector3df pos = pNode->getAbsolutePosition();
	//					std::cout << pNode->getName() << "�� ��ġ : " << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
	//					
	//					irr::core::list<irr::scene::ISceneNodeAnimator*> pAni =  pNode->getAnimators();
	//					
	//					if(pAni.getSize() > 0)
	//					{
	//						irr::core::list<irr::scene::ISceneNodeAnimator*>::Iterator it = pAni.begin();
	//						irr::core::vector3df p_pos =  static_cast<CBulletChracterAnimator*>((*it))->getPosition();
	//						std::cout << pNode->getName() << "�� ������ġ : " << p_pos.X << ", " << p_pos.Y << ", " << p_pos.Z << std::endl;
	//					}
	//				}
	//				else
	//				{
	//					std::cout << "�ų�带 ã�� �� �����ϴ�. " << std::endl;
	//				}
	//			}
	//		}
	//		else if(token[0] == "poschange")
	//		{
	//			irr::f32 x = atof(token[1].c_str());
	//			irr::f32 y = atof(token[2].c_str());
	//			irr::f32 z = atof(token[3].c_str());

	//			static_cast<CHeroObject*>(m_spHeroObject.get())->setPosition(irr::core::vector3df(x, y, z));
	//		}
	//		else if(token[0] == "make")
	//		{
	//			if(token[1] == "zombie")
	//			{
	//				irr::f32 x = atof(token[2].c_str());
	//				irr::f32 y = atof(token[3].c_str());
	//				irr::f32 z = atof(token[4].c_str());

	//				m_objectManager.createObject(CObjectManager::OBJ_ZOMBIE, irr::core::vector3df(x, y, z));
	//				std::cout << token[1] << "�� ���������� ����� �����ϴ�." << std::endl;
	//			}
	//			else if(token[1] == "plasmarifle")
	//			{
	//				irr::f32 x = atof(token[2].c_str());
	//				irr::f32 y = atof(token[3].c_str());
	//				irr::f32 z = atof(token[4].c_str());

	//				m_objectManager.createObject(CObjectManager::OBJ_PLASMA_RIFLE, irr::core::vector3df(x, y, z));
	//				std::cout << token[1] << "�� ���������� ����� �����ϴ�." << std::endl;
	//			}
	//			else
	//			{
	//				std::cout << token[1] << "�� ������Ʈ�� �������� �ʽ��ϴ�." << std::endl;
	//			}
	//		}
	//		else
	//		{
	//			std::cout << g_strMessage << " : �߸��� ��ɾ��Դϴ�." << std::endl;
	//		}

	//		std::cout<<std::endl;
	//		g_strMessage = "";
	//	}
	//}
}

void CApp::Render()
{
	m_pVideo->beginScene(true, true, irr::video::SColor(255,255,255,255));

	m_pSmgr->drawAll();
	m_pGuiEnv->drawAll();

	m_pVideo->endScene();
}

void CApp::GameLoop()
{
	while(m_pDevice->run())
	{
		Update();
		Render();
	}
}

bool CApp::OnEvent(const irr::SEvent &event)
{
	if ( m_spHeroObject.get() )
	{
		static_cast<CHeroObject*>(m_spHeroObject.get())->OnEvent(event);
	}

	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{
			switch(event.GUIEvent.EventType)
			{
			case irr::gui::EGET_MESSAGEBOX_OK:
				{
					m_pDevice->closeDevice();
				}
				break;

			case irr::gui::EGET_MESSAGEBOX_YES:
				{
					m_pDevice->closeDevice();
				}
				break;
			}
		}
		break;
	case irr::EET_KEY_INPUT_EVENT:
		{

		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{

		}
		break;
	case irr::EET_USER_EVENT:
		{

		}
		break;
	}

	return false;
}

void CApp::DrawShellCount(bool visible, irr::s16 count)
{
	static irr::s16 number = 0;
	
	int i;
	for(i = 0 ; i < 2 ; i++)
	{
		m_image[i]->setVisible(visible);
	}

	if(visible)
	{
		if(number != count)
		{
			irr::core::array<irr::s16> split_num;
			irr::s16 spit_count = count;
			while(spit_count)
			{
				split_num.push_back(spit_count%10);
				spit_count /= 10;
			}

			if(split_num.size() == 2)
			{
				m_image[0]->setImage(m_numberTexture[split_num[1]]);
				m_image[1]->setImage(m_numberTexture[split_num[0]]);

			}
			else if(split_num.size() == 1)
			{
				m_image[0]->setImage(m_numberTexture[0]);
				m_image[1]->setImage(m_numberTexture[split_num[0]]);
			}
			else
			{
				m_image[0]->setImage(m_numberTexture[0]);
				m_image[1]->setImage(m_numberTexture[0]);
			}

			number = count;
		}
	}
}

void CApp::CreateDieWindow()
{
	irr::gui::IGUIWindow* window = m_pGuiEnv->addMessageBox(
		L"Mission Failed !!",
		L"Mission Failed!! ",
		true,
		irr::gui::EMBF_OK ,
		0,
		0
		);
	window->getCloseButton()->setVisible(false);
}

void CApp::CreateVictoryWindow()
{
	irr::gui::IGUIWindow* window = m_pGuiEnv->addMessageBox(
		L"Mission success !!",
		L"Mission suceess you are survived!! ",
		true,
		irr::gui::EMBF_YES ,
		0,
		0
		);
	window->getCloseButton()->setVisible(false);
}

void CApp::PlayBgm()
{
	if(random(1,10)%2)
		bgmSound = m_SoundEngine->play2D("sound/bgm1.ogg", true, true, false);
	else
		bgmSound = m_SoundEngine->play2D("sound/bgm2.ogg", true, true, false);

	bgmSound->setVolume(0.5f);
}