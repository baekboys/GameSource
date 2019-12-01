#ifndef APPLICATION_H
#define APPLICATION_H

#include "irrlicht.h"

class IApplication :  public irr::IEventReceiver
{
protected :
	
	//bool m_Key[irr::KEY_KEY_CODES_COUNT];
	bool m_bTouch;

	//------------------
	// 일리히트 멤버변수
	//------------------
	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;

public :

	IApplication(irr::IrrlichtDevice* pDevice) : m_pDevice(NULL), m_pVideo(NULL), m_pSmgr(NULL), m_pGuiEnv(NULL)
	{
		m_pDevice = pDevice;
		m_pVideo = m_pDevice->getVideoDriver();
		m_pSmgr = m_pDevice->getSceneManager()->createNewSceneManager();
		m_pGuiEnv = m_pDevice->getGUIEnvironment();

		// 키코드 초기화
//		int i;
//		for( i=0 ;i<irr::KEY_KEY_CODES_COUNT ; i++)
//		{
//			m_Key[i] = false;
//		}
	}

	virtual ~IApplication()
	{
		if(m_pSmgr)
			m_pSmgr->drop();
	}

	// 이벤트 핸들러
	virtual bool OnEvent(const irr::SEvent& event) = 0;

	// 업데이트
	virtual void UpDate(irr::f32 fDelta) = 0;

	// 렌더
	virtual void Render() = 0;

	// 초기화
	virtual bool Init(void* param) = 0;

	// 리셋
	virtual void Reset() = 0;

	// 시그널처리
	virtual void Signal(irr::core::stringc strSignal,void *pParam) = 0;

	// 어플리케이션 시작
	virtual void Start(void* param) = 0;

	// 어플리케이션 시작상태인가
	virtual bool isStart() = 0;

	// GUI 켜고 끄기
	virtual void ActiveGUI(bool active) = 0;

	// GUI가 켜진 상태인가
	virtual bool isActiveGUI() = 0;
};

#endif