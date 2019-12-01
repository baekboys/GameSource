#ifndef APP_H
#define APP_H

#include "irrLib.h"
#include "Singleton.h"
#include "GameApp.h"
#include "AppDataStructure.h"
#include "TouchEvent.h"

class IrrlichtApp;

enum APP_GUI
{
	APP_LOADING_IMAGE =100,
	APP_BG_IMAGE,
	APP_GAME_BUTTON,
	APP_HOWTO_BUTTON,
	APP_HOWTO_WINDOW,
	APP_HOWTO_WINDOW_IMAGE,
	APP_HOWTO_EXIT_BUTTON,
	APP_HOWTO_NEXT_BUTTON,
	APP_HOWTO_BEFORE_BUTTON
};

enum APPMODE
{
	MENU, 
	GAME 
};

class CApp : public ggf::oop::Singleton<CApp>, public irr::IEventReceiver
{
	IrrlichtApp* m_pIrrlichtApp;

	// status
	enum SUB_STATUS
	{
		START,
		UPDATE
	};
	irr::u32 m_uMainStatus;
	irr::u32 m_uSubStatus;

	//------------------
	// �ϸ���Ʈ �������
	//------------------
	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;
	irr::core::stringc m_cWorkingDirectory;

	// �ϸ���Ʈ �ؽ��� ����Ʈ
	irr::core::array<irr::core::stringc> m_TextureList;

	//--------
	// Ű�ڵ�
	//--------
	bool m_bTouch;

	// �ڵ�ī�޶���
	irr::scene::ICameraSceneNode* m_pCamNode;

	//-------------------
	// ��              ��
	//-------------------
	// ���� ��ü
	CGameApp* m_spGameApp;

	//-----------
	//	GUI �κ�
	//-----------
	irr::gui::IGUIImage* m_pLoadingGUI;
	irr::gui::IGUIStaticText* m_pLoadingText;
	irr::u32 m_sHowToNumber;
	void CreateLoaingGUI();
	void CreateHowToGUI();
	void RemoveHowToGUI();
	void ChageHowToImage(bool next);
	irr::gui::IGUIButton* m_pStartButton;
	irr::gui::IGUIButton* m_pHowToButton;
	irr::f32 m_fButtonMoveTimer, m_fButtonMoveTime;
	bool m_bButtonMoveFlag;
	
	// GUIȰ��ȭ �Ǿ����� �Ǵ��ϴ� �÷���
	bool m_bIsActiveGUI;
 	
	// ���� GUI���� �����ϱ����� array. ���� (1) �Ӽ�GUI�� �����͸� ������ ��� ������ GUI�����͵�. ������� ��ư�̳� ����Ʈ�ڽ� ���..
	irr::core::array<irr::gui::IGUIElement*> m_pGUIElements;

	void ActiveGUI(bool active);
	
	bool InitIrrlicht(irr::IrrlichtDevice *device);
	bool InitAllXmlDataFile();
	bool InitTexture();
	bool InitGameApp();
	bool CreateGUI();

	bool m_bIsFirstStart;
	void preLoadSound();
	void StartThisApp();
	void StartGameApp();
		
public:

	CApp(void);
	~CApp(void);

	bool NativeInit(irr::IrrlichtDevice *device, IrrlichtApp* pIrrlichtApp);
	void NativeRender();
	void NativeUpDate(irr::f32 fDelta);
	void NativeResize(irr::s32 x, irr::s32 y);
	void NativeSetWorkDirectory(irr::core::stringc directory_name);
	
	
	void NativeOnTouchtEvent(irr::s32 eventType, irr::f32 x, irr::f32 y);
	void NativeOnMultiTouchtEvent(irr::core::array<STouchEvent>& touchEventArrays);
	void NativeOnOrientEvent(irr::f32 angle_X, irr::f32 angle_Y, irr::f32 angle_Z);
	virtual bool OnEvent(const irr::SEvent& event);
	
	
	void NativePlaySound(const char* fileName, irr::f32 volume = 1, bool isBgm = false);
	void NativePreLoadSound(const char* fileName);
	void NativeStopBgm();
	void NativePlayVibration();	
	
	void ChangeAppMode(APPMODE mode);
	void DrawLoadingGUI(bool visible);
	void DrawLoadingText(const wchar_t* text);
};

#endif