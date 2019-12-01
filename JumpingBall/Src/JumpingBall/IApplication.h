#ifndef APPLICATION_H
#define APPLICATION_H

#include "irrlicht.h"

class IApplication :  public irr::IEventReceiver
{
protected :
	
	//bool m_Key[irr::KEY_KEY_CODES_COUNT];
	bool m_bTouch;

	//------------------
	// �ϸ���Ʈ �������
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

		// Ű�ڵ� �ʱ�ȭ
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

	// �̺�Ʈ �ڵ鷯
	virtual bool OnEvent(const irr::SEvent& event) = 0;

	// ������Ʈ
	virtual void UpDate(irr::f32 fDelta) = 0;

	// ����
	virtual void Render() = 0;

	// �ʱ�ȭ
	virtual bool Init(void* param) = 0;

	// ����
	virtual void Reset() = 0;

	// �ñ׳�ó��
	virtual void Signal(irr::core::stringc strSignal,void *pParam) = 0;

	// ���ø����̼� ����
	virtual void Start(void* param) = 0;

	// ���ø����̼� ���ۻ����ΰ�
	virtual bool isStart() = 0;

	// GUI �Ѱ� ����
	virtual void ActiveGUI(bool active) = 0;

	// GUI�� ���� �����ΰ�
	virtual bool isActiveGUI() = 0;
};

#endif