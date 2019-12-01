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
	//	각종 자료구조
	//----------------

	// 오브젝트의 포인터를 저장할 리스트
	irr::core::list<IFSMObject*> m_listObjectManager;
	
	// 블럭오브젝트의 물리요소들(motionState와 rigidbody 포인터)의 포인터를 저장할 리스트
	irr::core::list<SRigidBodyAndMotionStatePtr> m_listBlockRigidbody;

	
	//------------------------
	//	게임에 필요한 멤버변수
	//------------------------

	// 공 오브젝트
	IFSMObject* m_spBallObject;

	// 종료방향 오브젝트
	IFSMObject* m_spDirectionObject;
	
	// 게임 타이머
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
	//	신노드 데이터
	//---------------

	// 맵씬이 저장될 노드
	irr::scene::ISceneNode* m_pMainMapNode;

	// 자동카메라노드
	irr::scene::ICameraSceneNode* m_pCamNode;
	
	CBallCameraAnimator* m_pCamAnimator;
	
	// 시작위치 노드
	irr::scene::ISceneNode* m_pStartPositionNode;

	// 종료위치 노드
	irr::scene::ISceneNode* m_pEndPositionNode;

	// 맵 노드
	irr::scene::ISceneNode* m_pMapNode;


	//-------------
	// GUI 멤버변수
	//-------------

	// GUI활성화 되었는지 판단하는 플래그
	bool m_bIsActiveGUI;

	// 페이드 인/아웃
	irr::gui::IGUIInOutFader *m_pInOutFader;

	// 각종 GUI들을 제어하기위한 array. 위의 (1) 속성GUI의 포인터를 제외한 모든 나머지 GUI포인터들. 예를들어 버튼이나 에디트박스 등등..
	irr::core::array<irr::gui::IGUIElement*> m_pGUIElements;

	irr::gui::IGUIElement* m_pMessageGUI;
	void CreateMissionMessage(bool missionClear);
	void CreateAllClearMessage();
	void RemoveMessageGUI();

	//------------------
	// 물리엔진 멤버변수
	//------------------
	irr::scene::CBulletAnimatorManager* m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* m_pWorldAnimator;


	//----------
	// 객체 생성
	//----------
	void AddBlockObj(irr::scene::ISceneNode* pNode);
	void AddItemObj(irr::scene::ISceneNode* pNode);
	void AddPortalObj(irr::scene::ISceneNode* pNode);
	void AddPositionObj(irr::scene::ISceneNode* pNode);


	//---------
	//	초기화
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
	// GUI와 I/O 이벤트 처리
	//--------------------------

	// 화면에 남은시간 표시하는 GUI 설정
	irr::gui::IGUIStaticText* m_pTxtTimer;
	irr::gui::IGUIStaticText* m_pTxtStatus;
	void DrawRemaindTime(irr::f32 time);

public:
	CGameApp(irr::IrrlichtDevice* pDevice);
	virtual ~CGameApp(void);

	// 이벤트 핸들러
	bool OnEvent(const irr::SEvent& event);

	// 업데이트
	void UpDate(irr::f32 fDelta);

	// 렌더
	void Render();

	// 초기화
	bool Init(void* param);

	// StartGame()
	//bool StartGame();
	
	// start by using Map numbe
	bool StartGameByMapNumber(irr::u32 number);
	
	// reset
	void Reset();

	// 시그널
	void Signal(irr::core::stringc strSignal,void *pParam);

	// 애플리케이션 시작
	void Start(void* param);

	// 어플리케이션 시작상태인가
	bool isStart() { if(m_uGameStauts == GAME_START) return true; return false; }

	// GUI 켜고 끄기
	void ActiveGUI(bool active);

	bool isActiveGUI() { return m_bIsActiveGUI; }
	
	void preLoadSound();

	void DrawBallStatus(const wchar_t* text);
};

#endif