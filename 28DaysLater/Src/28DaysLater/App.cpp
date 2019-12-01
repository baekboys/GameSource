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
	//::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //메모리릭 탐지
	//메모리릭 발생원인 탐지.
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
	//	시스템 파일 읽어오기	//
	//							//
	//////////////////////////////
	
	//------------------
	// XML리더 객체 생성
	//------------------
	CXmlReader reader;

	//-------------------------
	// 시스템XML파일을 읽는다.
	//-------------------------
	//std::cout << "시스템XML파일 로딩 시도" << std::endl;
	if (!reader.readSystemDATA("../../res/System/XML/system.xml", m_systemData))
	{
		//// 단순히 컴파일만을 위한 상태이면
		//if ( m_systemData.onlyCompile )
		//	std::cout << "단순히 컴파일만 하는 모드상태입니다." << std::endl;
		//// 시스템XML파일을 읽는데 실패하였다면
		//else
		//	std::cout << "시스템XML파일을 읽는데 실패하였습니다. XML포맷형식을 확인하세요" << std::endl;
		
		return false;
	}
	//std::cout << "시스템XML파일 로딩 성공" << std::endl;



	//////////////////////////////
	//							//
	//	엔진 디바이스 초기화	//
	//							//
	//////////////////////////////
	{
		//일리히트엔진
		m_pDevice = irr::createDevice(
			m_systemData.deviceType,
			irr::core::dimension2du(m_systemData.screen_size[0],m_systemData.screen_size[1]), //1.6버전용
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

		//마우스 포인터 숨김
		m_pDevice->getCursorControl()->setVisible(m_systemData.mouse_visible);

		//사운드엔진
		m_SoundEngine = irrklang::createIrrKlangDevice();
		m_SoundEngine->setDefault3DSoundMinDistance(m_systemData.soundMinDistance);
		m_SoundEngine->setDefault3DSoundMaxDistance(200);
	}

	//콘솔창의 명령을 위한 쓰래드 생성
	/*if(m_systemData.consoleCMD)
	{
		g_hThread_console = ::CreateThread(NULL,0,ConsoleThread,0,0,&g_ThreadID_console);
	}*/

	// 랜덤함수 시드 설정
	{
		srand(static_cast<irr::u32>(time(NULL)));
	}


	//////////////////////////
	//						//
	//	게임 초기화 모드	//
	//						//
	//////////////////////////

	//---------------------------
	// 초기화를 실행하지 않는다면
	//---------------------------
	if ( !m_systemData.gameInit )
	{
		//std::cout << "지금은 초기화를 하지 않는 모드로 선택되어있습니다." << std::endl;

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

	//씬그라프 전체 리프레쉬
	m_pSmgr->getRootSceneNode()->OnAnimate(0);

	gameStart = true;

	return true;
}

bool CApp::InitFactoryClass()
{
	//사용자 추가 확장 펙토리등록
	irr::scene::jz3d::CJZ3DSceneNodeFactory *factory = new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(factory);
	factory->drop();

	CIrrKlangSceneNodeFactory* irrkangFactory = new CIrrKlangSceneNodeFactory(m_SoundEngine, m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(irrkangFactory);
	irrkangFactory->drop();

	//물리애니메이터 펙토리등록
	m_pBulletPhysicsFactory = new irr::scene::CBulletAnimatorManager(m_pSmgr);

	if ( factory != NULL && m_pBulletPhysicsFactory != NULL)
		return true;
	return false;
}

bool CApp::InitWorld()
{
	//---------------------
	// 월드 애니메이터 추가
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
	// 물리지형처리 초기화
	//--------------------
	
	{
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("land_physics");
		irr::scene::jz3d::CBulletPhysicsObjectNode* pLandNode = static_cast<irr::scene::jz3d::CBulletPhysicsObjectNode*>(pNode);
		if ( pLandNode != NULL)
		{
			// 물리 애니메이터 붙이기
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

	// 노멀맵 초기화
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
	// 주인공 만들기
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
	//	// 좀비 객체 만들기
	//	//--------------------
	//	m_objectManager.createObject(CObjectManager::OBJ_ZOMBIE, irr::core::vector3df(random(-300, 300),10,random(-300, 300)));
	//}

	//irr::scene::ISceneNode *pStartPos = m_pSmgr->getSceneNodeFromName("usr/trigger/respawn");
	//	테스트 하기
	//for ( int i = 0; i < 1 ; i++)
	//{
	//	//--------------------
	//	// 좀비 객체 만들기
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
	// 플라즈마라이플 객체 만들기
	//----------------------------
	m_objectManager.createObject(CObjectManager::OBJ_PLASMA_RIFLE, pStartPos->getAbsolutePosition() + irr::core::vector3df(0,0,50));
 
	return true;
}

bool CApp::InitGui()
{
	////프레임 레이트 표시용 유아이
	//irr::gui::IGUIStaticText *pstextFPS = m_pGuiEnv->addStaticText(L"Frame rate",irr::core::rect<irr::s32>(0,0,100,20),true,true,0,100,true);

	
	irr::gui::IGUIImage* pGui = m_pGuiEnv->addImage(m_pVideo->getTexture("System/GUI/crosshair 5.png"), irr::core::vector2d<s32>(368,268), true, 0, 101, 0);
	m_pCrossHairImage = pGui;

	// 페이더 GUI
	{
		irr::core::recti size(0,0,m_pVideo->getScreenSize().Width,m_pVideo->getScreenSize().Height);
		fader = m_pGuiEnv->addInOutFader(&size, 0, 0);
		fader->setVisible(true);
	}

	irr::u32 center = m_pVideo->getScreenSize().Width / 2;
	irr::u32 down = m_pVideo->getScreenSize().Height;

	// 타이머 버튼 10자리 - 버튼 setEnable(false) 단순히 이미지를 그리기 위한 용도
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

		// 포인터 저장
		m_image[0] = image;
	}

	// 타이머 버튼 1자리 - 버튼 setEnable(false) 단순히 이미지를 그리기 위한 용도
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

		// 포인터 저장
		m_image[1] = image;
	}

	// 텍스쳐 불러오기
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
	// 주인공 사운드소스
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.heroList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.heroList[i].c_str());
			m_sSoundSourceData.hero.push_back(source);
		}
	}


	// 플라즈마라이플 사운드소스
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.plasmarifleList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.plasmarifleList[i].c_str());
			m_sSoundSourceData.plasmarifle.push_back(source);
		}
	}

	// 좀비 사운드소스
	{
		int i;
		for(i = 0 ; i < m_sSoundFileListData.zombieList.size() ; i++)
		{
			irrklang::ISoundSource* source = m_SoundEngine->addSoundSourceFromFile(m_sSoundFileListData.zombieList[i].c_str());
			m_sSoundSourceData.zombie.push_back(source);
		}
	}

	// 스테이지 사운드 소스
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
	// 스테이지 신 로딩
	//-----------------
	{
		bool loading = m_pSmgr->loadScene(m_systemData.stage_fileName.c_str());
		if ( !loading )
		{
			//std::cout << "스테이지 신노드 로딩 실패"<< std::endl;
			return false;
		}
		//std::cout << "스테이지 신노드 로딩 성공"<< std::endl;
	}


	//----------------
	// 주인공 신 로딩
	//----------------
	{
		bool sceneLoading = m_pSmgr->loadScene(m_systemData.hero_fileName.c_str());
		if ( !sceneLoading )
		{
			//std::cout << "주인공 신노드 로딩 실패"<< std::endl;
			return false;
		}
		//std::cout << "주인공 신노드 로딩 성공"<< std::endl;

		// 오브젝트 매니저에게 신노드 정보 삽입
		m_objectManager.addSceneNode(CObjectManager::OBJ_HERO, m_pSmgr->getSceneNodeFromName(m_systemData.hero_SceneName.c_str()));
	}


	//--------------
	// 좀비 신 로딩
	//--------------
	{
		bool sceneLoading = m_pSmgr->loadScene(m_systemData.zombie_fileName.c_str());
		if ( !sceneLoading )
		{
			//std::cout << "좀비 신노드 로딩 실패"<< std::endl;
			return false;
		}
		//std::cout << "좀비 신노드 로딩 성공"<< std::endl;

		// 오브젝트 매니저에게 신노드 정보 삽입
		m_objectManager.addSceneNode(CObjectManager::OBJ_ZOMBIE, m_pSmgr->getSceneNodeFromName(m_systemData.zombie_SceneName.c_str()));
	}

	//------------------------
	// 플라즈마 라이플 신 로딩
	//------------------------
	{
		bool sceneLoading = m_pSmgr->loadScene(m_systemData.plasmarifle_fileName.c_str());
		if ( !sceneLoading )
		{
			//std::cout << "플라즈마 라이플 신노드 로딩 실패"<< std::endl;
			return false;
		}

		// 오브젝트 매니저에게 신노드 정보 삽입
		m_objectManager.addSceneNode(CObjectManager::OBJ_PLASMA_RIFLE, m_pSmgr->getSceneNodeFromName(m_systemData.plasmarifle_SceneName.c_str()));
	}

	// 빛 신노드 로딩
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
	// 주인공 XML파일 로딩
	//--------------------
	{
		//std::cout << "주인공XML파일 읽기시도" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.heroXML.c_str(), m_sHeroData);

		if ( !xmlLoading )
			return false;
	}


	//--------------------
	// 좀비 XML파일 로딩
	//--------------------
	{
		//std::cout << "좀비XML파일 읽기시도" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.zombieXML.c_str(), m_sZombieData);

		if ( !xmlLoading )
			return false;
	}

	//----------------------------
	// 플라즈마라이플 XML파일 로딩
	//----------------------------
	{
		//std::cout << "플라즈마라이플XML파일 읽기시도" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.plasmarifleXML.c_str(), m_sPlasmaRifleData);

		if ( !xmlLoading )
			return false;
	}

	//----------------------------
	// 파티클 XML파일 로딩
	//----------------------------
	{
		//std::cout << "파티클XML파일 읽기시도" << std::endl;
		bool xmlLoading = reader.readObjectData(m_systemData.particleXML.c_str(), m_sParticleData);

		if ( !xmlLoading )
			return false;
	}

	//----------------------------
	// 사운드파일관련 XML파일 로딩
	//----------------------------
	{
		//std::cout << "사운드파일리스관련 XML파일 읽기시도" << std::endl;
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
	irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f; //델타값 구하기
	uLastTick = uTick;

	if(!FPSMode)
	{
		// 마우스 위치 보정
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

	//프레임레이트 갱신
	{
		/*wchar_t wszbuf[256];
		swprintf(wszbuf,L"Frame rate : %d\n TriAngle: %d",m_pVideo->getFPS(),
			m_pVideo->getPrimitiveCountDrawn());			
		m_pGuiEnv->getRootGUIElement()->getElementFromId(100)->setText(wszbuf);*/
	}

	if(gameStart)
	{
		//------------------------------
		// 스테이지 매니저 업데이트
		//------------------------------
		{
			m_stageManager.Update(fDelta);
		}

		//-------------------------
		// 오브젝트 매니저 업데이트
		//-------------------------
		{
			m_objectManager.Update(fDelta);
		}

		//----------
		// 적분처리
		//----------
		{
			irr::u32 dt = (irr::u32)(fDelta * 1000000.f); //백만분의 일초단위의 틱값					
			m_pBulletPhysicsFactory->OnUpdate(dt);
		}
	}
	
	////-----------------
	//// 콘솔명령어 처리
	////-----------------
	//{

	//	// 명령어가 비어있지 않을 경우만 처리
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
	//				std::cout << "주인공 위치 : " << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
	//			}
	//			else 
	//			{
	//				irr::scene::ISceneNode* pNode =  m_pSmgr->getSceneNodeFromName(token[1].c_str());
	//				
	//				if(pNode)
	//				{
	//					irr::core::vector3df pos = pNode->getAbsolutePosition();
	//					std::cout << pNode->getName() << "의 위치 : " << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
	//					
	//					irr::core::list<irr::scene::ISceneNodeAnimator*> pAni =  pNode->getAnimators();
	//					
	//					if(pAni.getSize() > 0)
	//					{
	//						irr::core::list<irr::scene::ISceneNodeAnimator*>::Iterator it = pAni.begin();
	//						irr::core::vector3df p_pos =  static_cast<CBulletChracterAnimator*>((*it))->getPosition();
	//						std::cout << pNode->getName() << "의 물리위치 : " << p_pos.X << ", " << p_pos.Y << ", " << p_pos.Z << std::endl;
	//					}
	//				}
	//				else
	//				{
	//					std::cout << "신노드를 찾을 수 없습니다. " << std::endl;
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
	//				std::cout << token[1] << "가 성공적으로 만들어 졌습니다." << std::endl;
	//			}
	//			else if(token[1] == "plasmarifle")
	//			{
	//				irr::f32 x = atof(token[2].c_str());
	//				irr::f32 y = atof(token[3].c_str());
	//				irr::f32 z = atof(token[4].c_str());

	//				m_objectManager.createObject(CObjectManager::OBJ_PLASMA_RIFLE, irr::core::vector3df(x, y, z));
	//				std::cout << token[1] << "가 성공적으로 만들어 졌습니다." << std::endl;
	//			}
	//			else
	//			{
	//				std::cout << token[1] << "의 오브젝트가 존재하지 않습니다." << std::endl;
	//			}
	//		}
	//		else
	//		{
	//			std::cout << g_strMessage << " : 잘못된 명령어입니다." << std::endl;
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