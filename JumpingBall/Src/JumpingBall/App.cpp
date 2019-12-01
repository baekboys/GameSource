#include "App.h"
#include "irrFactory.h"
#include "Singleton.h"
#include "BallObject.h"
#include "BallCameraAnimator.h"
#include "IrrXMLReader.h"

#include "IrrlichtApp.h"

bool isIphone4 = false;

CApp::CApp(void)
	: m_bIsActiveGUI(false), m_spGameApp(NULL), m_cWorkingDirectory("../../res"),
m_fButtonMoveTimer(0), m_fButtonMoveTime(0.3f), m_bButtonMoveFlag(false),
m_bIsFirstStart(true), m_sHowToNumber(0)
{

}


CApp::~CApp(void)
{
	if(m_spGameApp)
		delete m_spGameApp;
}

bool CApp::NativeInit(irr::IrrlichtDevice *device, IrrlichtApp* pIrrlichtApp)
{
	m_pIrrlichtApp = pIrrlichtApp;
	
	// init irrlicht
	if(!InitIrrlicht(device))
		return false;
	
	// create loading and draw loading on screen
	CreateLoaingGUI();
	DrawLoadingGUI(true);
	
	DrawLoadingText(L"INIT GUI");
	// GUI 만들기
	if(!CreateGUI())
		return false;

	DrawLoadingText(L"INIT XML data files");
	// XML데이터 불러오기
	if(!InitAllXmlDataFile())
		return false;

	DrawLoadingText(L"INIT texture files");
	// 텍스쳐 불러오기
	if(!InitTexture())
		return false;


	DrawLoadingText(L"INIT 3D & Physics engine");
	// CGameApp 객체 초기화
	if(!InitGameApp())
		return false;

	// 초기화가 끝났으므로 게임상태로 전환
	m_uMainStatus = MENU;
	
	// 게임상태 메뉴의 서브상태인 시작상태로 전환
	m_uSubStatus = START;
	
	// 로딩화면 없애기
	DrawLoadingGUI(false);
	ActiveGUI(true);
		
	// 초기화 성공
	return true;
}

bool CApp::InitIrrlicht(irr::IrrlichtDevice *device)
{
	m_pDevice = device;

	m_pVideo = m_pDevice->getVideoDriver();
	m_pSmgr = m_pDevice->getSceneManager();
	m_pGuiEnv = m_pDevice->getGUIEnvironment();
	
	if(!m_pDevice->getFileSystem()->changeWorkingDirectoryTo("./res"))
		return false;
		
	m_bTouch = false;

	// 아이폰4의 경우 스크린 사이즈가 아이폰4에 비하여 2배 차이를 보인다.
	// 아이폰3 : 480 * 320
	// 아이폰4 : 960 * 640
	//m_pVideo->getScreenSize().Width == 960 ? isIphone4 = true : isIphone4 = false;
	
	return true;
}

bool CApp::InitAllXmlDataFile()
{
	// 공 데이터
	if( !CBallObject::InitBallParamFromXMLData() )
		return false;

	return true;
}


bool CApp::InitTexture()
{
	CIrrXMLReader reader;
	
	// 텍스쳐 파일 리스트 불러오기
	if( !reader.LoadToStringArrayDataFromXMLFile("xml/systemdata/texturelist.xml", m_TextureList, "textureList") )
	{
		return false;
	}
	
	irr::u32 i;
	for(i = 0 ; i < m_TextureList.size() ; i++)
	{
		m_pVideo->getTexture(m_TextureList[i].c_str());
	}
	
	return true;
}

bool CApp::InitGameApp()
{
	m_spGameApp = new CGameApp(m_pDevice);
	return true;
}


void CApp::StartGameApp()
{
	m_spGameApp->Start(NULL);
	ChangeAppMode(GAME);
}

void CApp::StartThisApp()
{
	m_pIrrlichtApp->playSound("res/sound/title.mp3", 1, true);
}

void CApp::preLoadSound()
{
	// common effect sound
	NativePreLoadSound("res/sound/clear.mp3");
	NativePreLoadSound("res/sound/allclear.mp3");
	NativePreLoadSound("res/sound/failed.mp3");
	NativePreLoadSound("res/sound/itemeffect.mp3");
	NativePreLoadSound("res/sound/click.mp3");
	NativePreLoadSound("res/sound/warnning.mp3");
	NativePreLoadSound("res/sound/zoom.mp3");
	NativePreLoadSound("res/sound/portal.mp3");
	NativePreLoadSound("res/sound/spring.mp3");
	
	// ball effect sound
	m_spGameApp->preLoadSound();
	
	m_bIsFirstStart = false;
}

void CApp::NativeRender()
{
	switch( m_uMainStatus )
	{
	case MENU:
		{
			m_pVideo->beginScene(true, true, irr::video::SColor(255,0,0,0));
			m_pGuiEnv->drawAll();			
			m_pVideo->endScene();
		}
		break;

	case GAME:
		{			
			m_spGameApp->Render();
		}
		break;
	}	
}

void CApp::NativeUpDate(irr::f32 fDelta)
{
	// --- m_uMainStatus start ---
	switch( m_uMainStatus )
	{
		case MENU:
		{			
			
			// --- m_uSubStatus start ---
			switch (m_uSubStatus)
			{
					
				case START:
				{
					if(m_bIsFirstStart)
						preLoadSound();
					
					StartThisApp();
					++m_uSubStatus;
				}
					break;
					
				case UPDATE:
				{
					
				}
					break;
			} // --- m_uSubStatus end ---
		}
			break;
			
			
		case GAME:
		{			
			m_spGameApp->UpDate(fDelta);
		}
			break;
	} // --- m_uSubStatus start ---
}


bool CApp::OnEvent(const irr::SEvent &event)
{
	switch(m_uMainStatus)
	{
	//-------------------------------------------------------------------------------------------------
	case MENU:
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
							NativePlaySound("res/sound/click.mp3");
							
							switch(id)
							{
							case APP_GAME_BUTTON :
								{
									StartGameApp();
								}
								break;

							case APP_HOWTO_BUTTON :
								{
									CreateHowToGUI();
								}
								break;

							case APP_HOWTO_NEXT_BUTTON :
								{
									ChageHowToImage(true);
								}
								break;

							case APP_HOWTO_BEFORE_BUTTON :
								{
									ChageHowToImage(false);
								}
								break;

							case APP_HOWTO_EXIT_BUTTON :
								{
									RemoveHowToGUI();
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

		}
		break;

	case GAME:
		{
			m_spGameApp->OnEvent(event);
		}
		break;
	}

	return false;
}

bool CApp::CreateGUI()
{
	// 스킨 컬러값 설정
	{
		for (u32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
		{
			irr::video::SColor col = m_pGuiEnv->getSkin()->getColor((irr::gui::EGUI_DEFAULT_COLOR)i);
			col.setAlpha(0);
			m_pGuiEnv->getSkin()->setColor((irr::gui::EGUI_DEFAULT_COLOR)i, col);
		}
	}

	
	// background image
	{
		// 배경 텍스쳐의 사이즈는 스크린 사이즈만큼!
		irr::video::ITexture* texture = m_pVideo->getTexture("background/title.png");
		irr::gui::IGUIImage* titleImage = m_pGuiEnv->addImage(
															  irr::core::rect<irr::s32>(irr::core::position2di(0,0), m_pVideo->getScreenSize()),
															  0,
															  APP_BG_IMAGE
															  );
		titleImage->setImage(texture);
		titleImage->setScaleImage(true);
		m_pGUIElements.push_back(titleImage);
	}
	

	// 게임시작 버튼
	{
		irr::core::recti button_rect;
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di(300, 340);
			button_rect.LowerRightCorner = irr::core::position2di(556, 596);
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di(150, 170);
			button_rect.LowerRightCorner = irr::core::position2di(278, 298);
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
				button_rect,
				0,
				APP_GAME_BUTTON,
				0, 0
				);

		m_pStartButton = button;
		
		button->setVisible(true);
		button->setImage(m_pVideo->getTexture("gui/start.png"));
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
		m_pGUIElements.push_back(button);
	}

	// 종료 버튼
	{
		irr::core::recti button_rect;
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di(600, 240);
			button_rect.LowerRightCorner = irr::core::position2di(856, 496);
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di(300, 120);
			button_rect.LowerRightCorner = irr::core::position2di(428, 248);
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
				button_rect,
				0,
				APP_HOWTO_BUTTON,
				0, 0
				);
		
		m_pHowToButton = button;

		button->setVisible(true);
		button->setImage(m_pVideo->getTexture("gui/howto.png"));
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
		m_pGUIElements.push_back(button);
	}
	return true;
}

void CApp::CreateLoaingGUI()
{

	irr::video::ITexture* texture = m_pVideo->getTexture("background/loading.png");
	m_pLoadingGUI = m_pGuiEnv->addImage(
														  irr::core::rect<irr::s32>(irr::core::position2di(0,0), m_pVideo->getScreenSize()),
														  0,
														  APP_BG_IMAGE
														  );
	m_pLoadingGUI->setImage(texture);
	m_pLoadingGUI->setUseAlphaChannel(true);
	m_pLoadingGUI->setScaleImage(true);

	if(isIphone4)
		m_pLoadingText = m_pGuiEnv->addStaticText(L" ", irr::core::rect<irr::s32>(irr::core::position2di(0,480), irr::core::dimension2du(960,200)), false);
	else
		m_pLoadingText = m_pGuiEnv->addStaticText(L" ", irr::core::rect<irr::s32>(irr::core::position2di(0,240), irr::core::dimension2du(480,100)), false);

	m_pLoadingText->setOverrideColor(irr::video::SColor(255,228,44,154));
	m_pLoadingText->setOverrideFont(m_pGuiEnv->getFont("font/snidely.xml"));
	m_pLoadingText->setVisible(true);
	m_pLoadingText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);
}

void CApp::CreateHowToGUI()
{
	irr::gui::IGUIWindow* window = m_pGuiEnv->addWindow(
		irr::core::recti(irr::core::position2di(0,0), m_pVideo->getScreenSize()),
		false,
		0,0,
		APP_HOWTO_WINDOW
		);
	window->getCloseButton()->setVisible(false);
	window->setDraggable(false);
	window->setDrawBackground(false);
	window->setDrawTitlebar(false);

	// image
	{
		irr::video::ITexture* texture = m_pVideo->getTexture("howto/0.png");
		irr::gui::IGUIImage* image = m_pGuiEnv->addImage(
			irr::core::rect<irr::s32>(irr::core::position2di(0,0), m_pVideo->getScreenSize()),
			window,
			APP_HOWTO_WINDOW_IMAGE
			);
		image->setImage(texture);
		image->setUseAlphaChannel(true);
		image->setScaleImage(true);
	}
	
	// next button
	{
		irr::core::recti button_rect;
		irr::video::ITexture* texture = m_pVideo->getTexture("howto/nextbutton.png");
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di( (m_pVideo->getScreenSize().Width - texture->getSize().Width * 2), (m_pVideo->getScreenSize().Height/2 - texture->getSize().Height) );
			button_rect.LowerRightCorner = irr::core::position2di( m_pVideo->getScreenSize().Width, (m_pVideo->getScreenSize().Height/2 + texture->getSize().Height) );
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di( (m_pVideo->getScreenSize().Width - texture->getSize().Width) , (m_pVideo->getScreenSize().Height/2 - texture->getSize().Height/2) );
			button_rect.LowerRightCorner = irr::core::position2di( m_pVideo->getScreenSize().Width, (m_pVideo->getScreenSize().Height/2 - texture->getSize().Height/2 + texture->getSize().Height) );
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			window,
			APP_HOWTO_NEXT_BUTTON,
			0, 0
			);
		button->setVisible(true);
		button->setImage(texture);
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}

	// before button
	{
		irr::core::recti button_rect;
		irr::video::ITexture* texture = m_pVideo->getTexture("howto/beforebutton.png");
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di( 0, (m_pVideo->getScreenSize().Height/2 - texture->getSize().Height) );
			button_rect.LowerRightCorner = irr::core::position2di( (0 + texture->getSize().Width *2 ), (m_pVideo->getScreenSize().Height/2 + texture->getSize().Height) );
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di( 0, (m_pVideo->getScreenSize().Height/2 - texture->getSize().Height/2) );
			button_rect.LowerRightCorner = irr::core::position2di( (0 + texture->getSize().Width), (m_pVideo->getScreenSize().Height/2 - texture->getSize().Height/2 + texture->getSize().Height) );
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			window,
			APP_HOWTO_BEFORE_BUTTON,
			0, 0
			);
		button->setVisible(true);
		button->setImage(texture);
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}

	// exit button
	{
		irr::core::recti button_rect;
		irr::video::ITexture* texture = m_pVideo->getTexture("howto/exitbutton.png");
		if(isIphone4)
		{
			button_rect.UpperLeftCorner = irr::core::position2di( (m_pVideo->getScreenSize().Width - texture->getSize().Width * 2), 0 );
			button_rect.LowerRightCorner = irr::core::position2di( m_pVideo->getScreenSize().Width, (0 + texture->getSize().Height * 2));
		}
		else
		{
			button_rect.UpperLeftCorner = irr::core::position2di( (m_pVideo->getScreenSize().Width - texture->getSize().Width), 0 );
			button_rect.LowerRightCorner = irr::core::position2di( m_pVideo->getScreenSize().Width, (0 + texture->getSize().Height));
		}

		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			button_rect,
			window,
			APP_HOWTO_EXIT_BUTTON,
			0, 0
			);
		button->setVisible(true);
		button->setImage(texture);
		button->setScaleImage(true);
		button->setDrawBorder(false);
		button->setUseAlphaChannel(true);
	}
}

void CApp::RemoveHowToGUI()
{
	m_pGuiEnv->getRootGUIElement()->getElementFromId(APP_HOWTO_WINDOW, true)->remove();
	m_sHowToNumber = 0;
}

void CApp::ChageHowToImage(bool next)
{
	if(next)
	{
		m_sHowToNumber++;
		if(m_sHowToNumber == 11)
			m_sHowToNumber = 0;
	}
	else
	{
		m_sHowToNumber--;
		if(m_sHowToNumber == -1)
			m_sHowToNumber = 10;
	}

	irr::core::stringc fileName("howto/");
	fileName += m_sHowToNumber;
	fileName += ".png";
	irr::video::ITexture* texture = m_pVideo->getTexture(fileName.c_str());
	static_cast<irr::gui::IGUIImage*>(m_pGuiEnv->getRootGUIElement()->getElementFromId(APP_HOWTO_WINDOW_IMAGE, true))->setImage(texture);
}

void CApp::DrawLoadingGUI(bool visible)
{
	m_pLoadingGUI->setVisible(visible);
	m_pLoadingText->setVisible(visible);
}

void CApp::DrawLoadingText(const wchar_t* text)
{
	ActiveGUI(false);
	m_pLoadingText->setVisible(true);
	m_pLoadingText->setText(text);
	m_pVideo->beginScene(true, true, irr::video::SColor(255,0,0,0));
	m_pGuiEnv->drawAll();			
	m_pVideo->endScene();
}

void CApp::ActiveGUI(bool active)
{
	irr::u32 i;
	for(i = 0 ; i < m_pGUIElements.size() ; ++i)
	{
		m_pGUIElements[i]->setVisible(active);
	}
	
	m_bIsActiveGUI = active;
}

void CApp::NativeResize(irr::s32 x, irr::s32 y)
{
	irr::core::dimension2du size(x,y);
	m_pVideo->OnResize(size);
}

void CApp::NativeSetWorkDirectory(irr::core::stringc directory_name)
{
	m_cWorkingDirectory = directory_name;
}

void CApp::NativeOnTouchtEvent(irr::s32 eventType, irr::f32 x, irr::f32 y)
{
	SEvent irrevent;
	irrevent.EventType = irr::EET_MOUSE_INPUT_EVENT;
	irrevent.MouseInput.X = static_cast<irr::s32>(x);
	irrevent.MouseInput.Y = static_cast<irr::s32>(y);

	switch(eventType)
	{
	case 0:
		{
			irrevent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
			irrevent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN; 
		}
		break;

	case 1:
		{
			irrevent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
			irrevent.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP; 
		}
		break;

	case 2:
		{
			irrevent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
			irrevent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN; 
		}
		break;
	}
	m_pDevice->postEventFromUser(irrevent);
}

void CApp::NativeOnMultiTouchtEvent(irr::core::array<STouchEvent>& touchEventArrays)
{
	
}

void CApp::NativeOnOrientEvent(irr::f32 angle_X, irr::f32 angle_Y, irr::f32 angle_Z)
{
	irr::core::vector3df gravity_sensor(angle_X, angle_Y, angle_Z);
	
	switch(m_uMainStatus)
	{
	case MENU:
		{
		

		}
		break;

	case GAME:
		{			
			m_spGameApp->Signal("orientationEvent", &gravity_sensor);
		}
		break;
	}
}

void CApp::ChangeAppMode(APPMODE mode) 
{
	switch (mode) 
	{
		case MENU:
		{
			m_uMainStatus = MENU;
			m_uSubStatus = START;
			ActiveGUI(true);
			m_spGameApp->ActiveGUI(false);			
		}
			break;
			
		case GAME:
		{
			m_uMainStatus = GAME;
			ActiveGUI(false);
			m_spGameApp->ActiveGUI(true);			
		}
			break;
	}
}

void CApp::NativePlaySound(const char* fileName, irr::f32 volume, bool isBgm)
{
	m_pIrrlichtApp->playSound(fileName, volume, isBgm);
}

void CApp::NativePreLoadSound(const char* fileName)
{
	m_pIrrlichtApp->preLoadSound(fileName);
}

void CApp::NativeStopBgm()
{
	m_pIrrlichtApp->stopBgm();
}

void CApp::NativePlayVibration()
{
	m_pIrrlichtApp->playVibration();
}