#pragma once

#include "irrlicht.h"
#pragma comment(lib,"irrlicht.lib")

enum GUI_ID
{
	GUI_NEW_BUTTON = 100,
	GUI_LOAD_BUTTON,
	GUI_EXIT_BUTTON,

	GUI_MAPSIZE_WINDOW,
	GUI_MAPSIZE_OK_BUTTON,
	GUI_MAPSIZE_CANCEL_BUTTON,
	GUI_MAPSIZE_EDITBOX_HORIZONTAL,
	GUI_MAPSIZE_EDITBOX_VERTICAL,

	GUI_RETURN_MENU_BUTTON,

	GUI_SAVE_BUTTON,
	GUI_SAVE_WINDOW,
	GUI_SAVE_WINDOW_OK_BUTTON,
	GUI_SAVE_WINDOW_CANCEL_BUTTON,
	GUI_SAVE_WINDOW_EDITBOX,

	GUI_LOAD_FILEDIALOG,

	GUI_MAKE_BLOCK_BUTTON,
	GUI_MAKE_ITEM_BUTTON,
	GUI_MAKE_PORTAL_BUTTON,
	GUI_MAKE_ARROW_BUTTON,

	GUI_COPY_BUTTON,
	
	GUI_DELETE_BUTTON,

	GUI_RESTART_ALL_ANIMATOR_BUTTON,

	GUI_TEXTURE_BUTTON,
	GUI_TEXTURE_FILEDIALOG,

	GUI_CHANGE_ALL_BLOCK_PROPERTY_BUTTON,

	GUI_BLOCK_FRICTION_SPINBOX,
	GUI_BLOCK_RESTITUTION_SPINBOX,
	GUI_BLOCK_TEXTURE_COMBOBOX,
	GUI_BLOCK_SIZE_SPINEBOX,

	GUI_MESSEGE_LOG_STATICTEXT,

	GUI_PROPERTY_WINDOW,

	GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON,
	GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON,
	GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON,
	GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON,
	GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON,
	GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON,
	
	GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON,
	GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON,
	
	GUI_PROPERTY_WINDOW_BLOCK_TYPE_COMBOBOX,
	GUI_PROPERTY_WINDOW_BLOCK_FRICTION_SPINBOX,
	GUI_PROPERTY_WINDOW_BLOCK_RESTITUTION_SPINBOX,
	GUI_PROPERTY_WINDOW_BLOCK_SIZE_SPINBOX,

	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_WINDOW,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_COMBOBOX,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ROTATION_COMBOBOX,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_SPRING_COMBOBOX,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE0_SPINBOX,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE1_SPINBOX,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE2_SPINBOX,
	GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ANIMATION_BUTTON,
	
	GUI_PROPERTY_WINDOW_ITEM_TYPE_COMBOBOX,
	GUI_PROPERTY_WINDOW_ITEM_VALUE_SPINBOX,
	GUI_PROPERTY_WINDOW_ITEM_DURATIONTIME_SPINBOX,
	
	GUI_END_COUNT
};

struct Messege
{
	enum MESSEGE_TYPE
	{
		MSG_RESET,
		MSG_CLEAR,

		MSG_CHANGE_MAIN_MODE,
		MSG_CHANGE_EDITOR_MODE,

		MSG_PUSH_LOAD_BUTTON,
		MSG_PUSH_NEW_BUTTON,
		MSG_PUSH_MAPSIZE_OK_BUTTON,

		MSG_PUSH_SAVE_BUTTON,
		MSG_PUSH_SAVE_WINDOW_OK_BUTTON,

		MSG_PUSH_MAKE_BUTTON,	
		MSG_PUSH_COPY_BUTTON,
		MSG_PUSH_DELETE_BUTTON,
		MSG_PUSH_TEXTURE_BUTTON,
		MSG_PUSH_CHANGE_ALL_BLOCK_PROPERTY_BUTTON,
		MSG_PUSH_RESTAET_ALL_ANIMATOR_BUTTON,

		MSG_PUSH_PROPERTY_MICROMOVE_BUTTON,
		MSG_PUSH_PROPERTY_MICROROTATION_BUTTON,
		MSG_PUSH_PROPERTY_ANIMATION_BUTTON,
		
		MSG_CHANGE_BLOCK_BASIC_FRICTION,
		MSG_CHANGE_BLOCK_BASIC_RESTITUTION,
		MSG_CHANGE_BLOCK_BASIC_TEXTURE,
		MSG_CHANGE_BLOCK_BASIC_SIZE,

		MSG_CHANGE_PROPERTY_BLOCK_TYPE,
		MSG_CHANGE_PROPERTY_BLOCK_FRICTION,
		MSG_CHANGE_PROPERTY_BLOCK_RESTITUTION,
		MSG_CHANGE_PROPERTY_BLOCK_SIZE,
		MSG_CHANGE_PROPERTY_BLOCK_PROPERTY,
		MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0,
		MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE1,
		MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE2,
		MSG_CHANGE_PROPERTY_ITEM_TYPE,
		MSG_CHANGE_PROPERTY_ITEM_VALUE,
		MSG_CHANGE_PROPERTY_ITEM_DURATIONTIME,

		MSG_LOAD_SCENE_FROM_FILENAME,

		MSG_SETTEXTURE_TO_BGSCENE_FROM_FILENAME,

		MSG_END_COUNT
	};

	MESSEGE_TYPE type;
	void* pData;
	irr::core::array<irr::f32> fData;
	irr::core::array<irr::core::stringc> cData;
};

struct WorkingHistory
{
	enum WORKING_TYPE
	{
		WORKING_MAKE_BLOCK_NODE,
		WORKING_MAKE_ITEM_NODE,
		WORKING_MAKE_PORTAL_NODE,

		WORKING_DELETE_BLOCK_NODE,
		WORKING_DELETE_ITEM_NODE,
		WORKING_DELETE_PORTAL_NODE,
	};

	WORKING_TYPE type;
	irr::core::vector3df start;
	irr::core::vector3df end;
	irr::scene::ISceneNode* sceneNode;
	irr::f32 fData;
};

class CMapEditor : public irr::IEventReceiver
{
	// ���λ��� ����
	enum MAIN_STATUS
	{
		MENU,
		EDITOR
	};

	// �������� ���� ����
	enum EDITOR_MODE
	{
		MODE_STAND_BY,
		MODE_MAKE_BLOCK,
		MODE_MAKE_ITEM,
		MODE_MAKE_PORTAL,
		MODE_MAKE_ARROW,
		MODE_PICK_NODE,
	};

	//-----------------------------
	// �� ���� ������� �� �Լ�
	//-----------------------------

	// ���� �ʿ������� ���λ���
	irr::u16 m_uMainStatus;

	// �����͸�� ����
	irr::u16 m_uEditorStatus;

	// �� �̸�
	irr::core::stringw m_sMapName;

	// �� ������
	irr::core::dimension2du m_uMapSize;

	// ���� �Ѻ��� �⺻ ������
	const irr::u32 m_uBlockBasicSize;

	// �ʿ����Ϳ��� ���忡 ���� ��Ʈ �ų��
	irr::scene::ISceneNode* m_pMainSceneNode;

	// �ʿ����Ϳ��� ��ŷ�� ���ȭ�鿡 ���� ���ų��
	irr::scene::IMeshSceneNode* m_pBackGroundSceneNode;

	// �Ʒ��� 4���� ���� ��ŷ���� ���ܽ�Ű�� ���Ͽ� ����Ѵ�.
	// (1) ī�޶� ���
	irr::scene::ICameraSceneNode* m_pCameraNode;

	// (2) ������ġ ���
	irr::scene::ISceneNode* m_pStartNode;

	// (3) ������ġ ���
	irr::scene::ISceneNode* m_pEndNode;

	// (4) ���� �������� ������ ���ϰ� �ϴ� �����
	irr::scene::ISceneNode* m_pBackGroundBlockNode[4];

	// ��ŷ�� ���
	irr::scene::ISceneNode* m_pPickingNode;

	// ���콺�� 2D ��ǥ ������
	irr::core::dimension2di GetMouse2DPosition();

	// �����κ��� ��ǥ ������
	bool GetPositionFromBackgourndNode(irr::core::dimension2di position_mouse, irr::core::vector3df &position_3d);

	// 3D��ǥ�κ��� �浹�˻縦 �ǽ��Ͽ� ��� ��ŷ�ϱ�
	irr::scene::ISceneNode* GetPickingNode(irr::core::dimension2di position_mouse);

	// ���ڰ����� ���� �ų�尡 �ܰ��� ������� üũ
	bool CheckBackgoundNode(irr::scene::ISceneNode* pNode);

	// ���ڰ����� ���� �ų�尡 ����, ���Ḧ ��Ÿ���� ȭ��ǥ ������� üũ
	bool CheckPointNode(irr::scene::ISceneNode* pNode);

	// ����� �߰��ϱ�
	irr::scene::ISceneNode* MakeBlockNode(irr::core::vector3df start, irr::core::vector3df end);

	// �����۳�� �߰��ϱ�
	irr::scene::ISceneNode* MakeItemNode(irr::core::vector3df position);

	// ��Ż��� �߰��ϱ�
	irr::scene::ISceneNode* MakePortalNode(irr::core::vector3df position);

	// ȭ��ǥ��� �߰��ϱ�
	irr::scene::ISceneNode* MakeAroowNode(irr::core::vector3df position);

	// �����ϱ�
	void CopySceneNode(irr::scene::ISceneNode* pNode);

	//---------------------
	// �ϸ���Ʈ �������
	//---------------------
	irr::IrrlichtDevice *m_pDevice;
	irr::video::IVideoDriver *m_pVideo;
	irr::scene::ISceneManager *m_pSmgr;
	irr::gui::IGUIEnvironment *m_pGuiEnv;
	bool m_Key[irr::KEY_KEY_CODES_COUNT];
	bool m_Key_MouseWheel_Up;
	bool m_Key_MouseWheel_Down;

	//---------------------------------------------------
	// ���ɹڽ� GUI ���� ���� - �ּ�, �ִ�, ���� ��
	//---------------------------------------------------
	const irr::f32 m_fFrictionMin;
	const irr::f32 m_fFrictionMax;
	const irr::f32 m_fFrictionStep;
	const irr::f32 m_fRestitutionMin;
	const irr::f32 m_fRestitutionMax;
	const irr::f32 m_fRestitutionStep;
	const irr::f32 m_fSizeMin;
	const irr::f32 m_fSizeMax;
	const irr::f32 m_fSizeStep;
	const irr::f32 m_fItemMin;
	const irr::f32 m_fItemMax;
	const irr::f32 m_fItemStep;
	const irr::f32 m_fValueMin;
	const irr::f32 m_fValueMax;
	const irr::f32 m_fValueStep;

	irr::f32 m_fBasicFrictionValue;
	irr::f32 m_fBasicRestitutionValue;
	irr::u32 m_uBasicBlockTexture;
	irr::f32 m_fBasicBlockSize;

	//-----------------------------------------------------
	// GUI ���� (GUI Array�� ����  GUI�����츸���)
	//-----------------------------------------------------
	irr::core::array<irr::gui::IGUIElement *> m_pGUI_menu;
	irr::core::array<irr::gui::IGUIElement *> m_pGUI_Editor;
	void CreateMapSizeWindow();
	void CreateSaveWindow();
	void CreateLoadFileDialog();
	void CreateBGSceneTextureFileDialog();
	void CreatePropertyWindow(irr::scene::ISceneNode* pNode);
	void CreateBlockPropertyWindow(irr::scene::ISceneNode* pNode, irr::u32 block_property_type);

	//---------------
	// �ʱ�ȭ ����
	//---------------

	bool InitIrrlicht();
	bool InitGUI();
	bool InitIrrPluginFactory();

	bool CreateCamera();
	bool CreateSceneGraph();
	bool CreateHillPlane();


	//--------------------------------------------
	// ���� �� Ŭ���� (������ ��庯�� ����)
	//--------------------------------------------
	bool m_bIsClear;
	void Clear();
	void Reset();
	void ChangeMode(MAIN_STATUS mode);
	void ChangeEditorMode(EDITOR_MODE mode);
	bool SaveScene(irr::core::stringc file_name);
	bool LoadScene(irr::core::stringc file_name);


	//-------------------------
	// �޽����� �̺�Ʈ ó��
	//-------------------------

	// �޽��� ó���� �޽��� �����丮�� ���� Array
	irr::core::array<Messege> m_messegeQueue;
	irr::core::array<WorkingHistory> m_workingHistory;
	void AddMessege(Messege &messege);
	void AddWorkingHitory(WorkingHistory &history);
	bool ProccessingMessegeQueue(Messege &messege);

	// IO (Ű����� ���콺) �̺�Ʈ ó��
	const irr::f32 m_fCameraMoveValue; // ī�޶� ��ũ�� ��
	const irr::f32 m_fCameraZoomValue; // ī�޶� �� ��
	const irr::f32 m_fSceneNodeMicroMoveValue; // �ų�� �̼��̵� ��
	const irr::f32 m_fSceneNodeMicroRotationValue; // �ų�� �̼�ȸ�� ��
	bool m_bGUIHoveredEvent; // ���콺 Ŀ���� GUI���� �ִٸ� ���콺�� ������ ��ü�� �̺�Ʈ�� �߻���Ű�°��� �����ϱ� ���� �÷���

	bool ProccessingIOEvent();


	// �޽��� �α� ������
	void MessegeLogToStaticText(wchar_t* msg);
	void MessegeLogToStaticText(wchar_t* msg, irr::core::vector3df vec);
	irr::gui::IGUIStaticText* m_pStaticText;

	//------------------------------------------------
	// ��      �� Loop() { UpDate(); ->Render (); }
	//------------------------------------------------
	void UpDate();
	void Render();

public:
	CMapEditor(void);
	~CMapEditor(void);

	bool Init();
	inline bool Run() { return m_pDevice->run(); }
	void Loop();
	bool OnEvent(const irr::SEvent& event);
};
