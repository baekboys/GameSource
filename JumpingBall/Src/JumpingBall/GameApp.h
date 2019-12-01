#ifndef GAME_APP_H
#define GAME_APP_H

#include "IApplication.h"
#include "irrLib.h"
#include "FSMObject.h"
#include "bulletEngine.h"
#include "AppDataStructure.h"

class CBallCameraAnimator;

enum GAME_STATUS
{
	GAME_INIT,
	GAME_START,
	GAME_GUI_ANIMATION,
	GAME_CLEAR,
	GAME_END
};

enum GAME_GUI_ID
{
	GAME_GUI_FADER = 300,
	GAME_GUI_RESTART,
	GAME_GUI_MAINMENU,
	
	GAME_GUI_FAIL_MESSEGEBOX,
	GAME_GUI_FAIL_MESSEGEBOX_IMAGE,
	GAME_GUI_FAIL_MESSEGEBOX_YESBUTTON,
	GAME_GUI_FAIL_MESSEGEBOX_NOBUTTON,
	
	GAME_GUI_SUCCESS_MESSEGEBOX,
	GAME_GUI_SUCCESS_MESSEGEBOX_IMAGE,
	GAME_GUI_SUCCESS_MESSEGEBOX_YESBUTTON,
	GAME_GUI_SUCCESS_MESSEGEBOX_NOBUTTON,

	GAME_GUI_MAINMENU_MESSEGEBOX,
	GAME_GUI_MAINMENU_MESSEGEBOX_IMAGE,
	GAME_GUI_MAINMENU_MESSEGEBOX_OK,
	
	GAME_GUI_JUMP_BUTTON,
	GAME_GUI_ZOOM_BUTTON
};

struct SRigidBodyAndMotionStatePtr
{
	btDefaultMotionState* pMotionState;
	btRigidBody *pRigidBody;
};

struct SMessege
{
	irr::core::stringc signalMessege;
	irr::f32 signalData;
};

class CGameApp : public IApplication
{
	//--------
	// Status 
	//--------
	irr::u32 m_uGameStauts;
	irr::u32 m_uSubStatus;
	void setStatus(irr::u32 status) { m_uGameStauts = status; m_uSubStatus = 0; }
	irr::u32 getStatus() { return m_uGameStauts; }
	irr::u32 getSubStatus() { return m_uSubStatus; }
	void incSubStatus() { ++m_uSubStatus; }
	void decSubStatus() { --m_uSubStatus; }

	//----------------
	//	���� �ڷᱸ��
	//----------------

	// ������Ʈ�� �����͸� ������ ����Ʈ
	irr::core::list<IFSMObject*> m_listObjectManager;
	
	// ��������Ʈ�� ������ҵ�(motionState�� rigidbody ������)�� �����͸� ������ ����Ʈ
	irr::core::list<SRigidBodyAndMotionStatePtr> m_listBlockRigidbody;

	
	//------------------------
	//	���ӿ� �ʿ��� �������
	//------------------------

	// �� ������Ʈ
	IFSMObject* m_spBallObject;

	// ������� ������Ʈ
	IFSMObject* m_spDirectionObject;
	
	// ���� Ÿ�̸�
	irr::f32 m_fGameTimer;
	

	//-----
	// Map
	//-----
	
	// map number
	irr::u32 m_uCurrentMapNumber;
			
	// xml map data storage
	irr::core::array<SMapData> m_arrayMapData;
	//irr::core::array<irr::scene::ISceneNode*> m_arrayMapSceneNodeData;
	irr::scene::ISceneNode* m_pCurrentStageSceneNode;
	irr::scene::ISceneNode* m_pNextStageSceneNode;
	
	// init xml map data file
	bool InitMapXmlDataFile();
	
	// load all maps (scene node)
	//bool LoadAllMapScenesFromSMapdata();
	
	SMapData m_sMapData;

	
	//---------------
	// stage control
	//---------------
	bool hasNextStage();
	void startNextStage();
	//void finishGameAndGotoMainMenu();
	void GotoMainMenu();
	void preLoadNextScene();
	void preLoadFirstScene();

	//bool isStageClear;
	
	//---------------
	//	�ų�� ������
	//---------------

	// �ʾ��� ����� ���
	irr::scene::ISceneNode* m_pMainMapNode;

	// �ڵ�ī�޶���
	irr::scene::ICameraSceneNode* m_pCamNode;
	
	CBallCameraAnimator* m_pCamAnimator;
	
	// ������ġ ���
	irr::scene::ISceneNode* m_pStartPositionNode;

	// ������ġ ���
	irr::scene::ISceneNode* m_pEndPositionNode;

	// �� ���
	irr::scene::ISceneNode* m_pMapNode;


	//-------------
	// GUI �������
	//-------------

	// GUIȰ��ȭ �Ǿ����� �Ǵ��ϴ� �÷���
	bool m_bIsActiveGUI;

	// ���̵� ��/�ƿ�
	irr::gui::IGUIInOutFader *m_pInOutFader;

	// ���� GUI���� �����ϱ����� array. ���� (1) �Ӽ�GUI�� �����͸� ������ ��� ������ GUI�����͵�. ������� ��ư�̳� ����Ʈ�ڽ� ���..
	irr::core::array<irr::gui::IGUIElement*> m_pGUIElements;

	irr::gui::IGUIElement* m_pMessageGUI;
	void CreateMissionMessage(bool missionClear);
	void CreateAllClearMessage();
	void RemoveMessageGUI();

	//------------------
	// �������� �������
	//------------------
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;


	//----------
	// ��ü ����
	//----------
	void AddBlockObj(irr::scene::ISceneNode* pNode);
	void AddItemObj(irr::scene::ISceneNode* pNode);
	void AddPortalObj(irr::scene::ISceneNode* pNode);
	void AddPositionObj(irr::scene::ISceneNode* pNode);


	//---------
	//	�ʱ�ȭ
	//---------

	bool m_bIsClear;

	void InitIrrFactory();
	void InitBulletFactory();
	void InitWorld();
	void InitScene();
	void SetTargetBallToObjects();
	void InitCamera();
	void InitBall();
	void CreateDirectionObject();
	void CreateGUI();
	void CreateObjFromScene(irr::scene::ISceneNode *pMapNode);
	void Clear();
	void SetGravity(irr::f32 scalar);

	
	//--------------------------
	// GUI�� I/O �̺�Ʈ ó��
	//--------------------------

	// ȭ�鿡 �����ð� ǥ���ϴ� GUI ����
	irr::gui::IGUIStaticText* m_pTxtTimer;
	irr::gui::IGUIStaticText* m_pTxtStatus;
	void DrawRemaindTime(irr::f32 time);

public:
	CGameApp(irr::IrrlichtDevice* pDevice);
	virtual ~CGameApp(void);

	// �̺�Ʈ �ڵ鷯
	bool OnEvent(const irr::SEvent& event);

	// ������Ʈ
	void UpDate(irr::f32 fDelta);

	// ����
	void Render();

	// �ʱ�ȭ
	bool Init(void* param);

	// StartGame()
	//bool StartGame();
	
	// start by using Map numbe
	bool StartGameByMapNumber(irr::u32 number);
	
	// reset
	void Reset();

	// �ñ׳�
	void Signal(irr::core::stringc strSignal,void *pParam);

	// ���ø����̼� ����
	void Start(void* param);

	// ���ø����̼� ���ۻ����ΰ�
	bool isStart() { if(m_uGameStauts == GAME_START) return true; return false; }

	// GUI �Ѱ� ����
	void ActiveGUI(bool active);

	bool isActiveGUI() { return m_bIsActiveGUI; }
	
	void preLoadSound();

	void DrawBallStatus(const wchar_t* text);
};

#endif