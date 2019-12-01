#include "GameApp.h"
#include "irrFactory.h"
#include "Object.h"
#include "App.h"
#include "irrMath.h"
#include "bulletEngine.h"
#include "BallCameraAnimator.h"
#include "IrrXMLReader.h"

// 미션 성공, 실패, 올클리어 GUI에 관한 애니메이션 움직임 값
const irr::s32 gui_animation_move_value = 16;					// 아이폰3용
const irr::s32 gui_animation_move_value_iPhone4 = 32;			// 아이폰4용 - 화면사이즈가 2배

// App.cpp 파일에 정의된 bool변수
// 아이폰 4인지 구분
extern bool isIphone4;

CGameApp::CGameApp(irr::IrrlichtDevice* pDevice)
: IApplication(pDevice), 
m_uGameStauts(GAME_INIT), m_uSubStatus(0)  ,m_bIsActiveGUI(false), m_bIsClear(true), m_fGameTimer(0), m_uCurrentMapNumber(0),
m_pBulletPhysicsFactory(NULL), m_pMainMapNode(NULL), m_pMapNode(NULL), m_pWorldAnimator(NULL), m_pCamNode(NULL),
m_spBallObject(NULL), m_pCurrentStageSceneNode(NULL), m_pNextStageSceneNode(NULL)

{
	Init(NULL);
}


CGameApp::~CGameApp(void)
{
	// 모든 오브젝트 소멸
	Clear();

	if(m_spBallObject)
		delete m_spBallObject;

	// 신매니저 드롭
	if(m_pSmgr)
	{
		m_pSmgr->drop();
		m_pSmgr = NULL;
	}

	// 물리월드 애니메이터 드롭
	if(m_pWorldAnimator)
		m_pWorldAnimator->drop();

	if(m_pBulletPhysicsFactory)
		m_pBulletPhysicsFactory->drop();
}

bool CGameApp::OnEvent(const irr::SEvent& event)
{
	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{
			irr::s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{
			case irr::gui::EGET_BUTTON_CLICKED:
				{
					switch(id)
					{
					case GAME_GUI_RESTART :
						{
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							Reset();
						}
						break;

					case GAME_GUI_MAINMENU :
						{
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							GotoMainMenu();
						}
						break;

					case GAME_GUI_JUMP_BUTTON :
						{
							m_spBallObject->Signal("jump", NULL);
						}
						break;

					case GAME_GUI_ZOOM_BUTTON:
						{
							m_pCamAnimator->zoomOut();
						}
						break;

					case GAME_GUI_SUCCESS_MESSEGEBOX_YESBUTTON:
						{
							RemoveMessageGUI();
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							CApp::Get().DrawLoadingGUI(true);
							irr::core::stringw stageName(L"Stage : ");
							stageName += m_arrayMapData[m_uCurrentMapNumber+1].map_fileName;
							CApp::Get().DrawLoadingText(stageName.c_str());
							preLoadNextScene();
							CApp::Get().DrawLoadingGUI(false);
							startNextStage();
						}
						break;

					case GAME_GUI_SUCCESS_MESSEGEBOX_NOBUTTON:
						{
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							GotoMainMenu();
							RemoveMessageGUI();
						}
						break;

					case GAME_GUI_FAIL_MESSEGEBOX_YESBUTTON:
						{
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							Reset();
							RemoveMessageGUI();
						}
						break;

					case GAME_GUI_FAIL_MESSEGEBOX_NOBUTTON:
						{
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							GotoMainMenu();
							RemoveMessageGUI();
						}
						break;

					case GAME_GUI_MAINMENU_MESSEGEBOX_OK:
						{
							CApp::Get().NativePlaySound("res/sound/click.mp3");
							GotoMainMenu();
							m_pMessageGUI->remove();
						}
						break;
					}
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
			if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
			{
				m_bTouch = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
			{
				m_bTouch = false;
			}
		}
		break;

	case irr::EET_USER_EVENT:
		{

		}
		break;
	}

	return false;
}


void CGameApp::UpDate(irr::f32 fDelta)
{
	switch(getStatus())
	{


	case GAME_INIT :
		{

		}
		break;


	case GAME_START :
		{
			// physics world update
			irr::u32 dt = (irr::u32)(fDelta * 1000000.f); //백만분의 일초단위의 틱값					
			m_pBulletPhysicsFactory->OnUpdate(dt);

			// IFSMObject update
			{
				// ball object update
				m_spBallObject->Update(fDelta);

				// other objects update
				irr::core::list<IFSMObject*>::Iterator it = m_listObjectManager.begin();
				while(it != m_listObjectManager.end())
				{
					(*it)->Update(fDelta);
					it++;
				}
			}

			// timer update
			{
				m_fGameTimer -= fDelta;

				// 클리어시간내에 성공하지 못하였다면 (테스트 모드는 실패 없음)
				if(m_fGameTimer <= 0)
				{
					CApp::Get().NativePlaySound("res/sound/failed.mp3", 1, false);
					CreateMissionMessage(false);
					setStatus(GAME_GUI_ANIMATION);
				}
				// 화면에 남은시간 그리기
				else
				{
					DrawRemaindTime(m_fGameTimer);
				}
			}
		}
		break;

	case GAME_GUI_ANIMATION:
		{
			switch (getSubStatus()) 
			{
			case 0:
				{
					irr::core::recti pos = m_pMessageGUI->getRelativePosition();
					if(pos.getCenter() != irr::core::position2di(m_pVideo->getScreenSize().Width, m_pVideo->getScreenSize().Height/2))
					{
						if(isIphone4)
							pos.UpperLeftCorner.X += gui_animation_move_value_iPhone4;
						else
							pos.UpperLeftCorner.X += gui_animation_move_value;

						m_pMessageGUI->setRelativePosition(pos.UpperLeftCorner);
					}
					else
					{
						incSubStatus();
					}					
				}
				break;

			case 1:
				{
					irr::core::recti pos = m_pMessageGUI->getRelativePosition();
					if(pos.getCenter() != irr::core::position2di(m_pVideo->getScreenSize().Width/2, m_pVideo->getScreenSize().Height/2))
					{
						if(isIphone4)
							pos.UpperLeftCorner.X -= gui_animation_move_value_iPhone4;
						else
							pos.UpperLeftCorner.X -= gui_animation_move_value;

						m_pMessageGUI->setRelativePosition(pos.UpperLeftCorner);
					}
					else
					{
						setStatus(GAME_END);
					}					
				}
				break;
			}
		}
		break;

	case GAME_END :
		{

		}
		break;

	}
}

void CGameApp::Render()
{
	m_pVideo->beginScene(true, true, m_arrayMapData[m_uCurrentMapNumber].map_background_color);
	m_pSmgr->drawAll();
	m_pGuiEnv->drawAll();
	m_pVideo->endScene();
}

bool CGameApp::Init(void* param)
{
	if(!InitMapXmlDataFile())
		return false;

	CreateGUI();

	InitIrrFactory();

	InitBulletFactory();

	InitWorld();

	InitScene();

	InitBall();

	InitCamera();

	preLoadFirstScene();

	setStatus(GAME_INIT);

	return true;
}

void CGameApp::InitIrrFactory()
{
	//사용자 추가 확장 펙토리등록
	irr::scene::jz3d::CJZ3DSceneNodeFactory *factory = new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(factory);
	factory->drop();
}

void CGameApp::InitBulletFactory()
{
	//물리애니메이터 팩토리등록
	m_pBulletPhysicsFactory = new irr::scene::CBulletAnimatorManager(m_pSmgr);
}

void CGameApp::InitScene()
{
	m_pMainMapNode = m_pSmgr->addEmptySceneNode();
	m_pMainMapNode->setName("MainMap");
}

void CGameApp::InitWorld()
{
	//---------------------
	// 월드 애니메이터 추가
	//---------------------
	irr::scene::ISceneNode *pNode = m_pSmgr->addEmptySceneNode();
	pNode->setName("PhysicsWorld");		
	irr::scene::CBulletWorldAnimator::Params worldParams;

	m_pWorldAnimator =
		m_pBulletPhysicsFactory->createBulletWorldAnimator(
		m_pSmgr,
		pNode,
		&worldParams
		);

	pNode->addAnimator(m_pWorldAnimator);
}

void CGameApp::InitCamera()
{
	// 자동 카메라 생성
	{
		irr::scene::ICameraSceneNode* pCamNode = m_pSmgr->addCameraSceneNode();

		pCamNode->setFarValue(500.f);
		pCamNode->setUpVector(irr::core::vector3df(0,0,1));

		CBallCameraAnimator* pCamAnimator = new CBallCameraAnimator();

		CBallObject* pObj = static_cast<CBallObject*>(m_spBallObject);
		pCamAnimator->setTargetBall(pObj);
		pCamNode->addAnimator(pCamAnimator);
		m_pCamAnimator = pCamAnimator;
		pCamAnimator->drop();


		m_pCamNode = pCamNode;
	}

	m_pSmgr->setActiveCamera(m_pCamNode);
}

void CGameApp::InitBall()
{
	CBallObject* obj = new CBallObject();
	obj->Init(m_pSmgr, m_pVideo, m_pBulletPhysicsFactory, m_pWorldAnimator, this);
	m_spBallObject = obj;
}

void CGameApp::SetTargetBallToObjects()
{
	irr::core::list<IFSMObject*>::Iterator it = m_listObjectManager.begin();
	for(it ; it != m_listObjectManager.end() ; ++it)
	{
		(*it)->setTargetObject(m_spBallObject);
	}
}

void CGameApp::SetGravity(irr::f32 scalar)
{	
	m_pWorldAnimator->setGravity( irr::core::vector3df(0,0,-1) * scalar );
}

bool CGameApp::InitMapXmlDataFile()
{
	CIrrXMLReader reader;

	// 맵 카운트 세기
	irr::u32 mapCount = 0;
	irr::u32 attributeCount = 0;
	irr::core::array<irr::core::stringc> cData;

	if( reader.LoadToStringArrayDataFromXMLFileNotNodeName("xml/mapdata/mapdata.xml", cData, mapCount, attributeCount))
	{
		irr::u32 i;
		for(i = 0 ; i < mapCount ; i++)
		{
			SMapData data;

			// 파일명
			data.map_fileName = cData[attributeCount * i];
			// 중력값
			data.map_gravity_value = static_cast<irr::f32>( atof(cData[attributeCount * i + 1].c_str()) );
			// 클리어타임
			data.map_time = static_cast<irr::f32>( atof(cData[attributeCount * i +2].c_str()) );
			// 공의 타입
			data.map_startBallType = static_cast<irr::u32>( atoi(cData[attributeCount * i +3].c_str()) );

			data.map_background_color.setRed( static_cast<irr::u32>( atoi(cData[attributeCount * i +4].c_str()) ) );
			data.map_background_color.setGreen( static_cast<irr::u32>( atoi(cData[attributeCount * i +5].c_str()) ) );
			data.map_background_color.setBlue( static_cast<irr::u32>( atoi(cData[attributeCount * i +6].c_str()) ) );
			data.map_background_color.setAlpha( static_cast<irr::u32>( atoi(cData[attributeCount * i +7].c_str()) ) );

			data.map_bgm_fileName = cData[attributeCount * i + 8];

			// 저장하기
			m_arrayMapData.push_back(data);
		}

		return true;
	}

	return false;
}


//bool CGameApp::LoadAllMapScenesFromSMapdata()
//{
//	irr::scene::ISceneNode* pRootNode = m_pSmgr->getRootSceneNode();
//
//	// init a scene graph
//	{
//		m_pMainMapNode = m_pSmgr->addEmptySceneNode();
//		m_pMainMapNode->setName("MainMap");
//		pRootNode->addChild(m_pMainMapNode);
//	}
//
//	/*irr::scene::ISceneNode *pNode;
//	irr::core::stringc sceneFileName;
//	irr::u32 i;
//	for(i = 0 ; i < m_arrayMapData.size() ; ++i)
//	{		
//		irr::core::stringc fileName("map/");
//		fileName.append(m_arrayMapData[i].map_fileName);
//		fileName.append(".irr");
//
//		if( !m_pSmgr->loadScene( fileName.c_str() ) )
//			return false;
//
//		pNode = m_pSmgr->getSceneNodeFromName( m_arrayMapData[i].map_fileName.c_str() );
//		pNode->setVisible(false);
//		m_pMainMapNode->addChild(pNode);
//		m_arrayMapSceneNodeData.push_back(pNode);
//	}*/
//
//	return true;
//}

void CGameApp::CreateGUI()
{
	// 페이더 GUI
	{
		irr::core::recti size(irr::core::position2di(0,0), m_pVideo->getScreenSize());
		m_pInOutFader = m_pGuiEnv->addInOutFader(&size, 0, GAME_GUI_FADER);
		m_pInOutFader->setVisible(false);
		m_pGUIElements.push_back(m_pInOutFader);
	}

	// 리스타트 버튼
	{
		irr::core::recti button_rect;
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di(20, 20);
			button_rect.LowerRightCorner = irr::core::position2di(180, 180);
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di(10, 10);
			button_rect.LowerRightCorner = irr::core::position2di(80, 80);
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			0,
			GAME_GUI_RESTART,
			0, 0
			);

		button->setVisible(false);
		button->setImage(m_pVideo->getTexture("gui/restart.png"));
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
		m_pGUIElements.push_back(button);
	}

	// 메인메뉴 버튼
	{
		irr::core::recti button_rect;
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di(800, 20);
			button_rect.LowerRightCorner = irr::core::position2di(940, 160);
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di(400, 10);
			button_rect.LowerRightCorner = irr::core::position2di(470, 80);
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			0,
			GAME_GUI_MAINMENU,
			0, 0
			);

		button->setVisible(false);
		button->setImage(m_pVideo->getTexture("gui/mainmenu.png"));
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
		m_pGUIElements.push_back(button);
	}

	// jump button
	{
		irr::core::recti button_rect;
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di(10, 430);
			button_rect.LowerRightCorner = irr::core::position2di(210, 630);
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di(5, 215);
			button_rect.LowerRightCorner = irr::core::position2di(105, 315);
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			0,
			GAME_GUI_JUMP_BUTTON,
			0, 0
			);

		button->setVisible(false);
		button->setImage(m_pVideo->getTexture("gui/jump.png"));
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
		m_pGUIElements.push_back(button);
	}

	// zoom button
	{
		irr::core::recti button_rect;
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di(780, 460);
			button_rect.LowerRightCorner = irr::core::position2di(940, 620);
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di(390, 230);
			button_rect.LowerRightCorner = irr::core::position2di(470, 310);
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			0,
			GAME_GUI_ZOOM_BUTTON,
			0, 0
			);

		button->setVisible(false);
		button->setImage(m_pVideo->getTexture("gui/zoom.png"));
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
		m_pGUIElements.push_back(button);
	}

	// limit timer
	{
		irr::core::recti rect;
		if(isIphone4)
		{
			rect.UpperLeftCorner = irr::core::position2di(160, 20);
			rect.LowerRightCorner = irr::core::position2di(800, 180);
		}
		else
		{
			rect.UpperLeftCorner = irr::core::position2di(80, 10);
			rect.LowerRightCorner = irr::core::position2di(400, 60);
		}

		m_pTxtTimer = m_pGuiEnv->addStaticText(L" ", rect, false);
		m_pTxtTimer->setOverrideColor(irr::video::SColor(200,255,174,201));

		if(isIphone4)
			m_pTxtTimer->setOverrideFont(m_pGuiEnv->getFont("font/sharkbite_big.xml"));
		else
			m_pTxtTimer->setOverrideFont(m_pGuiEnv->getFont("font/sharkbite.xml"));

		m_pTxtTimer->setVisible(true);
		m_pTxtTimer->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		m_pGUIElements.push_back(m_pTxtTimer);
	}

	// Status
	{
		irr::core::recti rect;
		if(isIphone4)
		{
			rect.UpperLeftCorner = irr::core::position2di(160, 540);
			rect.LowerRightCorner = irr::core::position2di(800, 620);
		}
		else
		{
			rect.UpperLeftCorner = irr::core::position2di(80, 270);
			rect.LowerRightCorner = irr::core::position2di(400, 310);
		}

		m_pTxtStatus = m_pGuiEnv->addStaticText(L" ", rect, false);
		m_pTxtStatus->setOverrideColor(irr::video::SColor(255,249,192,0));

		if(isIphone4)
			m_pTxtStatus->setOverrideFont(m_pGuiEnv->getFont("font/snidely2.xml"));
		else
			m_pTxtStatus->setOverrideFont(m_pGuiEnv->getFont("font/snidely.xml"));

		m_pTxtStatus->setVisible(true);
		m_pTxtStatus->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		m_pGUIElements.push_back(m_pTxtStatus);
	}
}


void CGameApp::CreateMissionMessage(bool missionClear)
{
	ActiveGUI(false);

	irr::gui::IGUIWindow* window; 
	irr::video::ITexture* textureForImage, *textureForYesButton, *textureForNoButton;
	irr::s32 windowID, imageID, yesButtonID, noButtonID;

	// set ID & textures
	if(missionClear)
	{
		windowID = GAME_GUI_SUCCESS_MESSEGEBOX;
		imageID = GAME_GUI_SUCCESS_MESSEGEBOX_IMAGE;
		yesButtonID = GAME_GUI_SUCCESS_MESSEGEBOX_YESBUTTON;
		noButtonID = GAME_GUI_SUCCESS_MESSEGEBOX_NOBUTTON;
		textureForImage = m_pVideo->getTexture("gui/clear.png");
	}
	else 
	{
		windowID = GAME_GUI_FAIL_MESSEGEBOX;
		imageID = GAME_GUI_FAIL_MESSEGEBOX_IMAGE;
		yesButtonID = GAME_GUI_FAIL_MESSEGEBOX_YESBUTTON;
		noButtonID = GAME_GUI_FAIL_MESSEGEBOX_NOBUTTON;
		textureForImage = m_pVideo->getTexture("gui/failed.png");
	}
	textureForYesButton = m_pVideo->getTexture("gui/yes.png");
	textureForNoButton = m_pVideo->getTexture("gui/no.png");

	// create window
	{
		irr::core::recti rect;
		if(isIphone4)
		{
			rect.UpperLeftCorner = irr::core::position2di(-512, 160);
			rect.LowerRightCorner = irr::core::position2di(0, 480);
		}
		else
		{
			rect.UpperLeftCorner = irr::core::position2di(-256, 80);
			rect.LowerRightCorner = irr::core::position2di(0, 240);
		}

		window = m_pGuiEnv->addWindow(
			rect,
			true,
			0,
			0,
			windowID);
		window->getCloseButton()->setVisible(false);
		window->setDrawBackground(false);
		window->setDrawTitlebar(false);
		window->setDraggable(false);
	}

	// add the image
	{
		irr::gui::IGUIImage* image = m_pGuiEnv->addImage(
			irr::core::recti(irr::core::position2di(0,0), isIphone4 ? textureForImage->getOriginalSize() * 2 : textureForImage->getOriginalSize()),
			window,
			imageID,
			0
			);	
		image->setImage(textureForImage);
		image->setScaleImage(true);
		image->setUseAlphaChannel(true);
	}

	// add YES button
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(irr::core::position2di(0, isIphone4 ? 256 : 128), isIphone4 ? textureForYesButton->getOriginalSize() * 2 : textureForYesButton->getOriginalSize()),
			window,
			yesButtonID,
			0, 
			0
			);
		button->setImage(textureForYesButton);
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}

	// add No button
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(isIphone4 ? irr::core::position2di(256, 256) : irr::core::position2di(128,128), isIphone4 ? textureForNoButton->getOriginalSize() * 2 : textureForNoButton->getOriginalSize()),
			window,
			noButtonID,
			0, 
			0
			);
		button->setImage(textureForNoButton);
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}

	m_pMessageGUI = static_cast<irr::gui::IGUIElement*> (window);
}

void CGameApp::CreateAllClearMessage()
{
	ActiveGUI(false);

	irr::gui::IGUIWindow* window; 
	irr::video::ITexture* textureForImage, *textureForMainmenuButton;
	irr::s32 windowID, imageID, mainmenuButtonID;

	// set ID & textures
	windowID = GAME_GUI_MAINMENU_MESSEGEBOX;
	imageID = GAME_GUI_MAINMENU_MESSEGEBOX_IMAGE;
	mainmenuButtonID = GAME_GUI_MAINMENU_MESSEGEBOX_OK;
	textureForImage = m_pVideo->getTexture("gui/allclear.png");
	textureForMainmenuButton = m_pVideo->getTexture("gui/allclearbutton.png");

	// create window
	{
		irr::core::recti rect;
		if(isIphone4)
		{
			rect.UpperLeftCorner = irr::core::position2di(-512, 160);
			rect.LowerRightCorner = irr::core::position2di(0, 480);
		}
		else
		{
			rect.UpperLeftCorner = irr::core::position2di(-256, 80);
			rect.LowerRightCorner = irr::core::position2di(0, 240);
		}

		window = m_pGuiEnv->addWindow(
			rect,
			true, 
			0,
			0,
			windowID
			);
		window->getCloseButton()->setVisible(false);
		window->setDrawBackground(false);
		window->setDrawTitlebar(false);
		window->setDraggable(false);
	}

	// add the image
	{
		irr::gui::IGUIImage* image = m_pGuiEnv->addImage(
			irr::core::recti(irr::core::position2di(0,0), isIphone4 ? textureForImage->getOriginalSize() * 2 : textureForImage->getOriginalSize()),
			window,
			imageID,
			0
			);	
		image->setImage(textureForImage);
		image->setScaleImage(true);
		image->setUseAlphaChannel(true);
	}

	// add mainmenu button
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(isIphone4 ? irr::core::position2di(0,256) : irr::core::position2di(0,128), isIphone4 ? textureForMainmenuButton->getOriginalSize() * 2 :textureForMainmenuButton->getOriginalSize()),
			window,
			mainmenuButtonID,
			0, 
			0);
		button->setImage(textureForMainmenuButton);
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}

	m_pMessageGUI = static_cast<irr::gui::IGUIElement*> (window);
}


void CGameApp::DrawRemaindTime(irr::f32 time)
{
	wchar_t tmp_text[1024];
	swprintf(tmp_text, 1024, L"TIME LIMITE : %d", static_cast<irr::u32> (time) );
	m_pTxtTimer->setText(tmp_text);

	// warning effect
	if(time <= 21.f)
	{
		if(m_pTxtTimer->getOverrideColor() != irr::video::SColor(255,255,0,0))
			m_pTxtTimer->setOverrideColor(irr::video::SColor(255,255,0,0));

		if( time - (static_cast<irr::u32>(time)) < 0.01f && static_cast<irr::u32>(time)%4 == 0 && time > 1.f )
		{
			CApp::Get().NativePlaySound("res/sound/warnning.mp3");
		}

		/*if( time - (static_cast<irr::u32>(time)) < 0.5f)
		{
			m_pTxtTimer->setVisible(false);
		}
		else
		{
			m_pTxtTimer->setVisible(true);
		}*/
	}
}

void CGameApp::DrawBallStatus(const wchar_t* text)
{
	wchar_t tmp_text[1024];
	swprintf(tmp_text, 1024, text);
	m_pTxtStatus->setText(tmp_text);
}


void CGameApp::Start(void* param)
{
	StartGameByMapNumber(m_uCurrentMapNumber);
}

void CGameApp::Clear()
{
	// (1) delete IFSMObject
	{
		irr::core::list<IFSMObject*>::Iterator it = m_listObjectManager.begin();
		while(it != m_listObjectManager.end())
		{
			//(*it)->reset();
			delete (*it);

			it++;
		}
		m_listObjectManager.clear();
	}

	// (2) 스테이지 신 클리어
	if(m_pCurrentStageSceneNode)
	{
		m_pCurrentStageSceneNode->remove();
		m_pCurrentStageSceneNode = NULL;
	}

	// (3) 클리어 상태 플래그
	m_bIsClear = true;

	// 스테이지 클리어상태
	//isStageClear = false;

	// (4)
	setStatus(GAME_INIT);
}



bool CGameApp::StartGameByMapNumber(irr::u32 number)
{
	// (1) Clear 실시
	if(!m_bIsClear)
	{
		Clear();
	}

	// loading a stage scene
	{
		/*irr::core::stringc fileName("map/");
		fileName.append(m_arrayMapData[number].map_fileName);
		fileName.append(".irr");
		m_pSmgr->loadScene( fileName.c_str() );

		irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName( m_arrayMapData[number].map_fileName.c_str() );
		
		m_pCurrentStageSceneNode = pNode;
		m_pMainMapNode->addChild(pNode);*/

		m_pNextStageSceneNode->setVisible(true);
		m_pCurrentStageSceneNode = m_pNextStageSceneNode;
		m_pNextStageSceneNode = NULL;
	}

	// (2) create IFSMObject by scene
	CreateObjFromScene(m_pCurrentStageSceneNode);

	// (4) set ball to IFSMObject
	SetTargetBallToObjects();

	// set gravity
	SetGravity(m_arrayMapData[number].map_gravity_value);


	// set ball position and ball type
	static_cast<CBallObject*>(m_spBallObject)->gameStartByPosionAndType(m_pStartPositionNode->getAbsolutePosition(), m_arrayMapData[number].map_startBallType);

	// (7) GUI reset
	{
		// GUI reset
		m_pTxtTimer->setOverrideColor(irr::video::SColor(200,255,174,201));
		m_pTxtTimer->setVisible(true);
		m_pTxtStatus->setVisible(true);
	}

	// (8) set Status
	{
		setStatus(GAME_START);
	}

	// (9) scene refresh
	{
		//m_pSmgr->getRootSceneNode()->OnAnimate(0);
	}

	// (10) set clear flag
	{
		m_bIsClear = false;
	}

	// (11) fade in effect
	{
		m_pInOutFader->fadeIn(3000);
	}

	// (12) set timer
	{
		m_fGameTimer = m_arrayMapData[number].map_time;
	}

	// (13) active gui
	{
		if(!isActiveGUI())
			ActiveGUI(true);
	}

	// (14) play sound BGM
	{
		irr::core::stringc bgm_fileName("res/sound/");
		bgm_fileName.append(m_arrayMapData[number].map_bgm_fileName);
		CApp::Get().NativePlaySound(bgm_fileName.c_str(), 0.35f, true);
	}

	return true;
}

void CGameApp::Reset()
{
	// (1) ball object reset
	{
		m_spBallObject->reset();
	}

	// (2) Object reset
	{
		irr::core::list<IFSMObject*>::Iterator it = m_listObjectManager.begin();
		while(it != m_listObjectManager.end())
		{
			(*it)->reset();
			it++;
		}
	}

	// (3) GUI reset
	{
		// GUI reset
		if(!isActiveGUI())
			ActiveGUI(true);

		// timer-font reset
		m_pTxtTimer->setOverrideColor(irr::video::SColor(200,255,174,201));
		
		m_pTxtTimer->setVisible(true);
		m_pTxtStatus->setVisible(true);

		// fader reset
		m_pInOutFader->fadeIn(3000);
	}

	// (4) timer reset
	{
		m_fGameTimer = m_arrayMapData[m_uCurrentMapNumber].map_time;
	}

	// (5) Set Status
	{
		setStatus(GAME_START);
	}

	// (6) clear flag set
	{
		m_bIsClear = false;
	}
}

void CGameApp::CreateObjFromScene(irr::scene::ISceneNode *pMapNode)
{
	if(pMapNode == NULL)
	{
		pMapNode = m_pSmgr->getRootSceneNode();
	}

	irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = pMapNode->getChildren().begin();

	for(;it != pMapNode->getChildren().end();it++)
	{
		CreateObjFromScene(*it);


		switch( (*it)->getType())
		{
		case irr::scene::jz3d::CBlockNode::TypeID :
			{
				AddBlockObj((*it));
			}
			break;

		case irr::scene::jz3d::CItemNode::TypeID :
			{
				// 아이템 노드이면 바운딩박스를 위한 부모노드를 가져온다. 부모노드를 이용하여 아이템객체를 만든다.
				irr::scene::ISceneNode *pNode = (*it)->getParent();
				AddItemObj(pNode);
			}
			break;

		case irr::scene::jz3d::CPortalNode::TypeID :
			{
				irr::core::stringc nodeName( (*it)->getName() );

				// 입구포탈이면 그 부모를 가져온다. 부모노드를 이용하여 포탈객체를 만든다.
				if( nodeName == "InPortal" )
				{
					irr::scene::ISceneNode *pNode = (*it)->getParent();
					AddPortalObj(pNode);
				}
				// 종료포탈이면 그대로 저장하고 종료객체를 만든다
				else if ( nodeName == "EndPortal" )
				{
					m_pEndPositionNode = (*it);
					AddPositionObj( (*it) );
				}
			}
			break;

		case irr::scene::jz3d::CPositionNode::TypeID :
			{
				irr::scene::ISceneNode *pNode = (*it)->getParent();

				irr::core::stringc nodeName( pNode->getName() );

				if( nodeName == "Start" )
				{
					m_pStartPositionNode = pNode;
				}
			}
			break;
		}
	}
}

void CGameApp::AddBlockObj(irr::scene::ISceneNode* pNode)
{
	// 블럭노드로 형변환
	irr::scene::jz3d::CBlockNode *pBlockNode = static_cast<irr::scene::jz3d::CBlockNode *>( pNode );

	CBlockObject *pObj = new CBlockObject();
	pObj->Init(pBlockNode, m_pWorldAnimator);

	m_listObjectManager.push_back(pObj);
}

void CGameApp::AddItemObj(irr::scene::ISceneNode* pNode)
{
	CItemObject *pObj = new CItemObject();
	pObj->Init(pNode, NULL);

	m_listObjectManager.push_back(pObj);
}

void CGameApp::AddPortalObj(irr::scene::ISceneNode* pNode)
{
	CPortalObject *pObj = new CPortalObject();
	pObj->Init(pNode, NULL);

	m_listObjectManager.push_back(pObj);
}

void CGameApp::AddPositionObj(irr::scene::ISceneNode* pNode)
{
	CPositionObject *pObj = new CPositionObject();
	pObj->Init(pNode, NULL);

	m_listObjectManager.push_back(pObj);
}

void  CGameApp::Signal(irr::core::stringc strSignal,void *pParam)
{
	if(strSignal == "orientationEvent")
	{
		if(m_spBallObject)
			m_spBallObject->Signal(strSignal, pParam);
	}

	else if (strSignal == "zoomOut")
	{
		m_pCamAnimator->zoomOut();
	}

	else if (strSignal == "zoomOutJump")
	{
		m_pCamAnimator->zoomOutJump();
	}

	else if (strSignal == "portalEffect")
	{
		m_pInOutFader->fadeIn(2000);
	}

	// 공으로부터  클리어 메시지 받음
	else if (strSignal == "clearStage")
	{
		ActiveGUI(false);

		if(hasNextStage())
		{
			CApp::Get().NativePlaySound("res/sound/clear.mp3", 1, false);
			CreateMissionMessage(true);
		}
		else
		{
			CApp::Get().NativePlaySound("res/sound/allclear.mp3", 1, false);
			CreateAllClearMessage();
		}
		setStatus(GAME_GUI_ANIMATION);
	}

	else if(strSignal == "directionObject on")// 방향 객체 표시 온
	{
		//m_spDirectionObject->Signal("visible on", NULL);
	}

	else if(strSignal == "directionObject off")// 방향 객체 표시 온
	{
		//m_spDirectionObject->Signal("visible off", NULL);
	}

	else if(strSignal == "time")
	{
		m_fGameTimer += *(static_cast<irr::f32*>(pParam));

		if(m_fGameTimer> 20.f)
		{
			m_pTxtTimer->setOverrideColor(irr::video::SColor(200,255,174,201));
		}
	}

	else if(strSignal == "gravity off")
	{
		SetGravity(0);
	}

	else if(strSignal == "gravity on")
	{
		SetGravity(m_arrayMapData[m_uCurrentMapNumber].map_gravity_value);
	}
}


void CGameApp::ActiveGUI(bool active)
{
	irr::u32 i;
	for(i = 0 ; i < m_pGUIElements.size() ; ++i)
	{
		m_pGUIElements[i]->setVisible(active);
	}

	m_bIsActiveGUI = active;
}

void CGameApp::RemoveMessageGUI()
{
	if(m_pMessageGUI)
		m_pMessageGUI->remove();
	m_pMessageGUI = NULL;
}

void CGameApp::GotoMainMenu()
{
	Clear();
	m_uCurrentMapNumber = 0;
	ActiveGUI(false);
	CApp::Get().DrawLoadingGUI(true);
	preLoadFirstScene();
	CApp::Get().DrawLoadingGUI(false);
	CApp::Get().ChangeAppMode(MENU);
}

bool CGameApp::hasNextStage()
{
	if( m_uCurrentMapNumber+1 < m_arrayMapData.size() )
	{
		return true;
	}

	return false;
}

void CGameApp::startNextStage()
{
	StartGameByMapNumber(++m_uCurrentMapNumber);
}

void CGameApp::preLoadSound()
{
	m_spBallObject->Signal("preLoadSound", NULL);
}

void CGameApp::preLoadNextScene()
{
	// loading a stage scene
	irr::core::stringc fileName("map/");
	fileName.append(m_arrayMapData[m_uCurrentMapNumber+1].map_fileName);
	fileName.append(".irr");
	m_pSmgr->loadScene( fileName.c_str() );

	irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName( m_arrayMapData[m_uCurrentMapNumber+1].map_fileName.c_str() );
	pNode->setVisible(false);
	m_pNextStageSceneNode = pNode;
	m_pMainMapNode->addChild(pNode);
}

void CGameApp::preLoadFirstScene()
{
	// loading a stage scene
	irr::core::stringc fileName("map/");
	fileName.append(m_arrayMapData[0].map_fileName);
	fileName.append(".irr");
	m_pSmgr->loadScene( fileName.c_str() );

	irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName( m_arrayMapData[0].map_fileName.c_str() );
	m_pNextStageSceneNode = pNode;
	m_pMainMapNode->addChild(pNode);
}