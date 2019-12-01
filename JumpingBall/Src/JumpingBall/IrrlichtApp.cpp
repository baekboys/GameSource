#include "IrrlichtApp.h"

const irr::f32 m_fAccelMeter_value = 2.f;
const irr::f32 m_fAccelMeter_value_epsilon = 0.001f;

IrrlichtApp::~IrrlichtApp()
{
	if(m_pDevice)
		m_pDevice->drop();

	if(m_pSoundEngine)
	{
		stopBgm();
		m_pSoundEngine->stopAllSounds();
		m_pSoundEngine->drop();
	}
}

void IrrlichtApp::initIrrlicht()
{
	irr::IrrlichtDevice *pDevice = irr::createDevice(
		irr::video::EDT_DIRECT3D9,
		//irr::core::dimension2du(960, 640),	//아이폰4 사이즈
		irr::core::dimension2du(480, 320),		//아이폰3 사이즈
		16,
		false,
		false,
		true,
		this
		);

	m_pDevice = pDevice;
	m_pVideo = pDevice->getVideoDriver();
	m_pSmgr = pDevice->getSceneManager();
	m_pGuiEnv = pDevice->getGUIEnvironment();

	//사운드엔진
	m_pSoundEngine = irrklang::createIrrKlangDevice();
	m_pBGMSound = NULL;

	// change working directory
	m_pDevice->getFileSystem()->changeWorkingDirectoryTo(m_cAbsolutePath.c_str());

	// 키코드 초기화
	int i;
	for( i=0 ;i<irr::KEY_KEY_CODES_COUNT ; i++)
	{
		m_Key[i] = false;
	}

	m_fAccelMeter = 0;

	app.NativeInit(m_pDevice, this);
}


void IrrlichtApp::setWorkingDirectory(const char* path)
{
	m_cAbsolutePath = irr::core::stringc(path);
}


void IrrlichtApp::render()
{	
	app.NativeRender();
}


void IrrlichtApp::update(irr::f32 fDelta)
{
	app.NativeUpDate(fDelta);
}


void IrrlichtApp::resize(irr::s32 x, irr::s32 y)
{
	irr::core::dimension2du size(x,y);
	m_pVideo->OnResize(size);
}


void IrrlichtApp::gameLoop()
{
	while (m_pDevice->run())
	{
		static irr::u32 uLastTick=0;
		irr::u32 uTick = m_pDevice->getTimer()->getTime();      
		irr::f32 fDelta = ((float)(uTick - uLastTick)) / 1000.f;
		uLastTick = uTick;
		
		if(m_Key[irr::KEY_LEFT])
		{
			AccelControl(0, fDelta);
		}
		else if(m_Key[irr::KEY_RIGHT])
		{
			AccelControl(1, fDelta);
		}
		else if(m_Key[irr::KEY_UP])
		{
			AccelControl(3, fDelta);
		}
		else if(m_Key[irr::KEY_DOWN])
		{
			AccelControl(2, fDelta);
		}
		else if(m_Key[irr::KEY_SPACE])
		{
			AccelControl(4, fDelta);
			m_Key[irr::KEY_UP] = false;
		}
		else
		{
			AccelControl(5, fDelta);
		}

		update(fDelta);
		render();
	}
}


void IrrlichtApp::singleTouchEvent(STouchEvent& touchEvent)
{
	irr::SEvent event;
	event.EventType = irr::EET_MOUSE_INPUT_EVENT;
	event.MouseInput.X = static_cast<irr::s32>(touchEvent.pointX);
	event.MouseInput.Y = static_cast<irr::s32>(touchEvent.pointY);	
	event.MouseInput.ButtonStates = irr::EMBSM_LEFT;
	
	switch (touchEvent.type) 
	{
		case ETOUCH_BEGAN:
			{
				event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
			}
			break;
			
		case ETOUCH_MOVE:
			{
				event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
			}
			break;
			
		case ETOUCH_END:
			{
				event.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
			}
			break;
			
		case ETOUCH_DOUBLE_TOUCH:
			{
				event.MouseInput.Event = irr::EMIE_LMOUSE_DOUBLE_CLICK;
			}
			break;
	}
	
	m_pDevice->postEventFromUser(event);
}

void IrrlichtApp::multiTouchEvent(irr::core::array<STouchEvent>& touchEventArrays)
{
	//app.NativeOnMultiTouchtEvent(touchEventArrays);
}

void IrrlichtApp::accelerometerEvent(irr::f32 x, irr::f32 y, irr::f32 z)
{
	app.NativeOnOrientEvent(x, y, z);
}

bool IrrlichtApp::OnEvent(const irr::SEvent &event)
{
	switch(event.EventType)
	{
	case irr::EET_KEY_INPUT_EVENT:
		{
			if(event.KeyInput.PressedDown)
			{
				m_Key[event.KeyInput.Key] = true;
			}
			else
			{
				m_Key[event.KeyInput.Key] = false;
			}
		}
		break;

	case irr::EET_USER_EVENT:
		{

		}
		break;
	}

	app.OnEvent(event);
	
	return false;
}

void IrrlichtApp::playSound(const char* fileName, irr::f32 volume, bool isBgm)
{
	irr::core::stringc name(fileName);
	irr::core::stringc newFileName(name.subString(4, name.size()));

	if(isBgm)
	{
		if(m_pBGMSound)
			stopBgm();

		m_pBGMSound =	m_pSoundEngine->play2D(newFileName.c_str(), true, true);
		m_pBGMSound->setVolume(volume);
		m_pBGMSound->setIsPaused(false);
	}
	else
	{
		m_pSoundEngine->play2D(newFileName.c_str());
	}
}

void IrrlichtApp::preLoadSound(const char* fileName)
{
	
}

void IrrlichtApp::setBgmVolume(float volume)
{
	if(m_pBGMSound)
		m_pBGMSound->setVolume(volume);
}

void IrrlichtApp::stopBgm()
{
	if(m_pBGMSound)
	{
		m_pBGMSound->stop();
		m_pBGMSound->drop();
		m_pBGMSound = NULL;
	}
}

void IrrlichtApp::AccelControl(irr::u32 direction, irr::f32 fDelta)
{
	switch(direction)
	{
	case 0: // 왼쪽
		{
			m_fAccelMeter += m_fAccelMeter_value * fDelta;

			if(m_fAccelMeter > 1)
				m_fAccelMeter = 1;

			accelerometerEvent(0, m_fAccelMeter, 0);
		}
		break;

	case 1: // 오른쪽
		{
			m_fAccelMeter -= m_fAccelMeter_value * fDelta;

			if(m_fAccelMeter < -1)
				m_fAccelMeter = -1;

			accelerometerEvent(0, m_fAccelMeter, 0);
		}
		break;

	case 2: // 아래쪽
		{
			m_fAccelMeter += m_fAccelMeter_value * fDelta;

			if(m_fAccelMeter > 1)
				m_fAccelMeter = 1;

			accelerometerEvent(0, 0, m_fAccelMeter);
		}
		break;

	case 3: // 위쪽
		{
			m_fAccelMeter -= m_fAccelMeter_value * fDelta;

			if(m_fAccelMeter < -1)
				m_fAccelMeter = -1;

			accelerometerEvent(0, 0, m_fAccelMeter);
		}
		break;

	case 4: // 점프
		{
			accelerometerEvent(0, m_fAccelMeter, 1.5f);
		}
		break;
	
	case 5: // 아무키도 누르지 않음
		{
			//if(m_fAccelMeter != 0)
			{
				if(m_fAccelMeter > 0)
				{
					m_fAccelMeter -= m_fAccelMeter_value * fDelta;
				}
				else if(m_fAccelMeter < 0)
				{
					m_fAccelMeter += m_fAccelMeter_value * fDelta;
				}
			}

			if(m_fAccelMeter > -m_fAccelMeter_value_epsilon && m_fAccelMeter < m_fAccelMeter_value_epsilon)
			{
				m_fAccelMeter = 0;
			}

			accelerometerEvent(0, 0, 0);
		}
		break;
	}
}