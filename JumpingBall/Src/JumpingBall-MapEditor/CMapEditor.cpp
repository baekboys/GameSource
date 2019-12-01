#include "CMapEditor.h"
#include "irrFactory.h"

#include <string>
#include <algorithm>

#include <iostream>

CMapEditor::CMapEditor(void)
: m_uBlockBasicSize(10), m_pPickingNode(NULL),
// 맵 이름
m_sMapName("stage"),
// 보정에 필요한 각종 값들 초기화 - 카메라 이동, 줌, 신노드의 이동
m_fCameraMoveValue(1), m_fCameraZoomValue(15), m_fSceneNodeMicroMoveValue(0.1f), m_fSceneNodeMicroRotationValue(5),
// GUI 이벤트에  관한 처리
m_bGUIHoveredEvent(false),
// 물리 관련 - 마찰력
m_fFrictionMin(0), m_fFrictionMax(10), m_fFrictionStep(0.1f),
// 물리 관련 - 탄성력
m_fRestitutionMin(0), m_fRestitutionMax(10), m_fRestitutionStep(0.1f),
// 블럭 사이즈 스핀박스 제어값
m_fSizeMin(1), m_fSizeMax(100), m_fSizeStep(1),
// 아이템 값 스핀박스 제어값
m_fItemMin(-10000), m_fItemMax(10000), m_fItemStep(1),
// 블럭 속성값
m_fValueMin(-10000), m_fValueMax(10000), m_fValueStep(10),
// 블럭의 기본 마찰력, 탄성력, 텍스쳐, 사이즈
m_fBasicFrictionValue(0.5f), m_fBasicRestitutionValue(0.5f), m_uBasicBlockTexture(0), m_fBasicBlockSize(20)
{
	int i;
	for( i=0 ;i<irr::KEY_KEY_CODES_COUNT ; i++)
	{
		m_Key[i] = false;
	}

	m_Key_MouseWheel_Up = false;
	m_Key_MouseWheel_Down = false;
}

CMapEditor::~CMapEditor(void)
{
	m_pDevice->drop();
}

bool CMapEditor::Init()
{
	if(!InitIrrlicht())
		return false;

	if(!InitIrrPluginFactory())
		return false;

	if(!InitGUI())
		return false;

	ChangeMode(MENU);

	return true;
}

bool CMapEditor::InitIrrlicht()
{
	m_pDevice = irr::createDevice(
		irr::video::EDT_DIRECT3D9,
		irr::core::dimension2du(800,700),
		16,
		false,
		false,
		false,
		this
		);

	m_pVideo = m_pDevice->getVideoDriver();
	m_pSmgr = m_pDevice->getSceneManager();
	m_pGuiEnv = m_pDevice->getGUIEnvironment();

	m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../../res");

	return true;
}

bool CMapEditor::InitIrrPluginFactory()
{
	//사용자 추가 확장 펙토리등록
	irr::scene::jz3d::CJZ3DSceneNodeFactory *factory = new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(factory);
	factory->drop();

	if(!factory)
		return false;
	return true;
}

bool CMapEditor::InitGUI()
{
	// 스킨 컬러값 설정
	{
		for (u32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
		{
			irr::video::SColor col = m_pGuiEnv->getSkin()->getColor((irr::gui::EGUI_DEFAULT_COLOR)i);
			col.setAlpha(255);
			m_pGuiEnv->getSkin()->setColor((irr::gui::EGUI_DEFAULT_COLOR)i, col);
		}
	}

	// 새로만들기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(100,200,250,400),
			0,
			GUI_NEW_BUTTON,
			L"Creating a new map",
			0);

		m_pGUI_menu.push_back(button);
	}

	// 맵 로딩 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(300,200,450,400),
			0,
			GUI_LOAD_BUTTON,
			L"Loading a map",
			0);

		m_pGUI_menu.push_back(button);
	}

	// 종료 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(500,200,650,400),
			0,
			GUI_EXIT_BUTTON,
			L"EXIT",
			0);

		m_pGUI_menu.push_back(button);
	}


	// 메뉴로 돌아가기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(0,0,50,50),
			0,
			GUI_RETURN_MENU_BUTTON,
			L"RETURN TO\nMAINMENU",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 세이브 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(50,0,100,50),
			0,
			GUI_SAVE_BUTTON,
			L"SAVE",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 블럭 만들기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(150,0,200,50),
			0,
			GUI_MAKE_BLOCK_BUTTON,
			L"BLOCK",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 아이템 만들기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(200,0,250,50),
			0,
			GUI_MAKE_ITEM_BUTTON,
			L"ITEM",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 포탈 만들기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(250,0,300,50),
			0,
			GUI_MAKE_PORTAL_BUTTON,
			L"PORTAL",
			0);

		m_pGUI_Editor.push_back(button);
	}

	// 화살표 만들기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(300,0,350,50),
			0,
			GUI_MAKE_ARROW_BUTTON,
			L"ARROW",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 복사 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(350,0,400,50),
			0,
			GUI_COPY_BUTTON,
			L"COPY",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 삭제 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(400,0,450,50),
			0,
			GUI_DELETE_BUTTON,
			L"DELETE",
			0);

		m_pGUI_Editor.push_back(button);
	}

	// 블럭의 모든 애니메이션 리스타트 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(450,0,500,50),
			0,
			GUI_RESTART_ALL_ANIMATOR_BUTTON,
			L"RESTART\nANIMATOR",
			0);

		m_pGUI_Editor.push_back(button);
	}



	// 텍스쳐 선택 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(500,0,550,50),
			0,
			GUI_TEXTURE_BUTTON,
			L"BackGround\nTEXTURE",
			0);

		m_pGUI_Editor.push_back(button);
	}

	// 블럭 전체 속성값 바꾸기 버튼
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(550,0,600,50),
			0,
			GUI_CHANGE_ALL_BLOCK_PROPERTY_BUTTON,
			L"Change\nAll Block\nProperty",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// 블럭 마찰력 스태틱 텍스트
	{
		irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
			L"Basic Friction",
			irr::core::rect<irr::s32>(irr::core::position2di(600,0), irr::core::dimension2du(100,20)),
			true, true, 0,
			-1,
			false
			);
		staticText->setDrawBackground(false);
		staticText->setDrawBorder(false);
		staticText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		m_pGUI_Editor.push_back(staticText);
	}

	// 블럭 마찰력 스핀박스 만들기 ---
	{
		irr::gui::IGUISpinBox *box_friction = m_pGuiEnv->addSpinBox(
			L"",
			irr::core::rect<irr::s32>(irr::core::position2di(600,20), irr::core::dimension2du(100,20)),
			true,
			0,
			GUI_BLOCK_FRICTION_SPINBOX
			);
		box_friction->setRange(m_fFrictionMin, m_fFrictionMax);
		box_friction->setStepSize(m_fFrictionStep);
		box_friction->setValue(m_fBasicFrictionValue);
		m_pGUI_Editor.push_back(box_friction);
	}

	// 블럭 탄성력 스태틱 텍스트
	{
		irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
			L"Basic Restitution",
			irr::core::rect<irr::s32>(irr::core::position2di(600,40), irr::core::dimension2du(100,20)),
			true, true, 0,
			-1,
			false
			);
		staticText->setDrawBackground(false);
		staticText->setDrawBorder(false);
		staticText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		m_pGUI_Editor.push_back(staticText);
	}

	// 블럭 탄성력 스핀박스 만들기 ---
	{
		irr::gui::IGUISpinBox *box_friction = m_pGuiEnv->addSpinBox(
			L"",
			irr::core::rect<irr::s32>(irr::core::position2di(600,60), irr::core::dimension2du(100,20)),
			true,
			0,
			GUI_BLOCK_RESTITUTION_SPINBOX
			);
		box_friction->setRange(m_fFrictionMin, m_fFrictionMax);
		box_friction->setStepSize(m_fFrictionStep);
		box_friction->setValue(m_fBasicRestitutionValue);
		m_pGUI_Editor.push_back(box_friction);
	}

	// 블럭 기본 텍스쳐 스태틱 텍스트 만들기
	{
		irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
			L"Basic Texture",
			irr::core::rect<irr::s32>(irr::core::position2di(600,80), irr::core::dimension2du(100,20)),
			true, true, 0,
			-1,
			false
			);
		staticText->setDrawBackground(false);
		staticText->setDrawBorder(false);
		staticText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		m_pGUI_Editor.push_back(staticText);
	}
	
	// 블럭 기본 텍스쳐 콤보박스 만들기
	{
		irr::gui::IGUIComboBox *box_type = m_pGuiEnv->addComboBox(
			irr::core::rect<irr::s32>(irr::core::position2di(600,100), irr::core::dimension2du(100,20)),
			0,
			GUI_BLOCK_TEXTURE_COMBOBOX
			);

		// 블럭 기본 텍스쳐 콤보박스 아이템 추가
		irr::u16 i;
		for(i = 0 ; i < irr::scene::jz3d::BLOCK_TOTAL_COUNT ; ++i)
		{
			box_type->addItem(irr::scene::jz3d::CBlockNode::typeNames[i], i);
		}

		box_type->setSelected(m_uBasicBlockTexture);
		m_pGUI_Editor.push_back(box_type);
	}

	// 블럭 사이즈 스태틱 텍스트
	{
		irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
			L"Basic Size",
			irr::core::rect<irr::s32>(irr::core::position2di(600,120), irr::core::dimension2du(100,20)),
			true, true, 0,
			-1,
			false
			);
		staticText->setDrawBackground(false);
		staticText->setDrawBorder(false);
		staticText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		m_pGUI_Editor.push_back(staticText);
	}

	// 블럭 사이즈 스핀박스 만들기 ---
	{
		irr::gui::IGUISpinBox *box_friction = m_pGuiEnv->addSpinBox(
			L"",
			irr::core::rect<irr::s32>(irr::core::position2di(600,140), irr::core::dimension2du(100,20)),
			true,
			0,
			GUI_BLOCK_SIZE_SPINEBOX
			);
		box_friction->setRange(m_fSizeMin, m_fSizeMax);
		box_friction->setStepSize(m_fSizeStep);
		box_friction->setValue(m_fBasicBlockSize);
		m_pGUI_Editor.push_back(box_friction);
	}
	

	// 로그메시지 박스
	{
		irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
			L"",
			irr::core::rect<irr::s32>(irr::core::position2di(0,670), irr::core::dimension2du(800,50)),
			true, true, 0,
			GUI_MESSEGE_LOG_STATICTEXT,
			false
			);

		staticText->setDrawBackground(false);
		staticText->setDrawBorder(false);
		staticText->setOverrideColor(irr::video::SColor(255,255,0,0));
		staticText->setOverrideFont(m_pGuiEnv->getFont("font/sharkbite.xml"));

		m_pStaticText = staticText;
		m_pGUI_Editor.push_back(staticText);
	}


	return true;
}

void CMapEditor::CreateMapSizeWindow()
{
	// 맵사이즈 윈도우 만들기
	irr::gui::IGUIWindow *window = m_pGuiEnv->addWindow(
		irr::core::rect<irr::s32>(300,250,500,350),
		true,
		L"Map size",
		0,
		GUI_MAPSIZE_WINDOW
		);
	// 윈도우 닫기버튼 비활성화 및 감추기, 드래그 금지
	window->getCloseButton()->setEnabled(false);
	window->getCloseButton()->setVisible(false);
	window->setDraggable(false);

	// 맵사이즈 - 가로사이즈 입력 에디트 박스 만들기
	irr::gui::IGUIEditBox *box1 = m_pGuiEnv->addEditBox(
		L"400",
		irr::core::rect<irr::s32>(10,25,90,55),
		true,
		window,
		GUI_MAPSIZE_EDITBOX_HORIZONTAL
		);

	// 맵사이즈 - 세로사이즈 입력 에디트 박스 만들기
	irr::gui::IGUIEditBox *box2 = m_pGuiEnv->addEditBox(
		L"300",
		irr::core::rect<irr::s32>(110,25,190,55),
		true,
		window,
		GUI_MAPSIZE_EDITBOX_VERTICAL
		);

	// 맵사이즈 - OK버튼 만들기
	irr::gui::IGUIButton* button1 = m_pGuiEnv->addButton(
		irr::core::recti(10,60,90,90),
		window,
		GUI_MAPSIZE_OK_BUTTON,
		L"O      K",
		0);

	// 맵사이즈 - CANCEL버튼 만들기
	irr::gui::IGUIButton* button2 = m_pGuiEnv->addButton(
		irr::core::recti(110,60,190,90),
		window,
		GUI_MAPSIZE_CANCEL_BUTTON,
		L"CANCEL",
		0);
}

void CMapEditor::CreateSaveWindow()
{
	// 세이브 윈도우 만들기
	irr::gui::IGUIWindow *window = m_pGuiEnv->addWindow(
		irr::core::rect<irr::s32>(300,250,500,350),
		true,
		L"SAVE",
		0,
		GUI_SAVE_WINDOW
		);
	// 윈도우 닫기버튼 비활성화 및 감추기, 드래그 금지
	window->getCloseButton()->setEnabled(false);
	window->getCloseButton()->setVisible(false);
	window->setDraggable(false);

	// 세이브 파일이름 에디트박스 만들기
	irr::gui::IGUIEditBox *box1 = m_pGuiEnv->addEditBox(
		m_sMapName.c_str(),
		irr::core::rect<irr::s32>(10,25,190,55),
		true,
		window,
		GUI_SAVE_WINDOW_EDITBOX
		);

	// 세이브 OK버튼 만들기
	irr::gui::IGUIButton* button1 = m_pGuiEnv->addButton(
		irr::core::recti(10,60,90,90),
		window,
		GUI_SAVE_WINDOW_OK_BUTTON,
		L"O      K",
		0);

	// 세이브 CANCEL버튼 만들기
	irr::gui::IGUIButton* button2 = m_pGuiEnv->addButton(
		irr::core::recti(110,60,190,90),
		window,
		GUI_SAVE_WINDOW_CANCEL_BUTTON,
		L"CANCEL",
		0);
}

void CMapEditor::CreateLoadFileDialog()
{
	// 파일오픈다이얼로그 만들기
	irr::gui::IGUIFileOpenDialog* dialog = m_pGuiEnv->addFileOpenDialog(
		L"Select a  map-name.irr", 
		true, 
		0, 
		GUI_LOAD_FILEDIALOG
		);
}

void CMapEditor::CreateBGSceneTextureFileDialog()
{
	// 백경신노드의 텍스쳐를 선택할 다이얼로그 만들기
	irr::gui::IGUIFileOpenDialog* dialog = m_pGuiEnv->addFileOpenDialog(
		L"Select a texture file", 
		true, 
		0, 
		GUI_TEXTURE_FILEDIALOG
		);
}

void CMapEditor::CreatePropertyWindow(irr::scene::ISceneNode* pNode)
{
	//------------------------------------------------------------
	// GUI 속성윈도우를 위한 필요한 변수 선언 코드 - 시작
	//------------------------------------------------------------
	irr::gui::IGUIWindow *window = NULL;
	irr::gui::IGUIStaticText* staticText[2];
	irr::gui::IGUIButton* button[8];
	irr::core::stringw windowName;
	irr::core::rect<irr::s32> windowSize;
	bool moveControlFlag;
	bool blockPropertyFlag;
	bool itemPropertyFlag;
	bool rotationControlFlag;
	irr::scene::jz3d::CBlockNode* blockNode;
	//------------------------------------------------------------
	// GUI 속성윈도우를 위한 필요한 변수 선언 코드 - 종료
	//------------------------------------------------------------



	//--------------------------------------------------------------------
	// 노드의 타입에 따라서 적절한 변수값을 할당하는 코드 - 시작
	//--------------------------------------------------------------------
	switch(pNode->getType())
	{
		// 블럭노드
	case irr::scene::jz3d::CBlockNode::TypeID:
		{
			// 윈도우이름
			windowName = L"Block property";
			// 윈도우 사이즈
			windowSize = irr::core::recti(700,0,800,700);
			// 회전컨트롤 버튼 생성
			blockPropertyFlag = true;
			// 외곽 블럭이면 이동,회전 컨트롤 불가능.
			if( CheckBackgoundNode(pNode) )
			{
				moveControlFlag = rotationControlFlag = false;
			}
			else
			{
				moveControlFlag = rotationControlFlag = true;
			}
			// 형변환 시켜 포인터 얻어오기
			blockNode = static_cast<irr::scene::jz3d::CBlockNode*>(pNode);
			// 아이템 속성 플래그
			itemPropertyFlag = false;
		}
		break;

		// 바운딩박스 노드 - 자식으로 다시 구분
	case irr::scene::jz3d::CEmptyBBoxNode::TypeID:
		{
			switch( static_cast<irr::scene::jz3d::CEmptyBBoxNode*>(pNode)->getFirstChild()->getType() )
			{
				// 아이템 노드
			case irr::scene::jz3d::CItemNode::TypeID:
				{
					// 윈도우이름
					windowName = L"Item property";
					// 윈도우 사이즈
					windowSize = irr::core::recti(700,0,800,500);
					// 이동컨트롤 가능
					moveControlFlag = true;
					// 회전컨트롤 버튼 불가능
					rotationControlFlag = blockPropertyFlag = false;
					// 아이템 속성 플래그
					itemPropertyFlag = true;
				}
				break;

				// 위치노드 - 시작, 종료 화살표 노드
			case irr::scene::jz3d::CPositionNode::TypeID:
				{
					// 윈도우이름
					windowName = L"Position property";
					// 윈도우 사이즈
					windowSize = irr::core::recti(700,0,800,220);
					// 이동컨트롤 가능
					moveControlFlag = true;
					// 회전컨트롤 버튼 불가능
					rotationControlFlag = true;
					// 블럭 속성 플래그
					blockPropertyFlag = false;
					// 아이템 속성 플래그
					itemPropertyFlag = false;
				}
				break;
			}
		}
		break;

	case irr::scene::jz3d::CPortalNode::TypeID:
		{
			// 윈도우이름
			windowName = L"Portal property";
			// 윈도우 사이즈
			windowSize = irr::core::recti(700,0,800,150);
			// 이동컨트롤 가능
			moveControlFlag = true;
			// 회전컨트롤 버튼 불가능
			rotationControlFlag = blockPropertyFlag = false;
			// 아이템 속성 플래그
			itemPropertyFlag = false;
		}
		break;
	}
	//--------------------------------------------------------------------
	// 노드의 타입에 따라서 적절한 변수값을 할당하는 코드 - 종료
	//--------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	// 위에서 할당한 적절한 변수를 참조하여 공통GUI(윈도우, 이동버튼)를 생성하는 코드 - 시작
	//----------------------------------------------------------------------------------------------------
	{
		// --- 속성창 만들기 ---
		window = m_pGuiEnv->addWindow(
			windowSize,
			false,
			windowName.c_str(),
			0,
			GUI_PROPERTY_WINDOW
			);
		// 윈도우 닫기버튼 비활성화 및 감추기, 드래그 금지
		window->getCloseButton()->setEnabled(false);
		window->getCloseButton()->setVisible(false);
		window->setDraggable(false);

		// --- 스태틱텍스트(미세 움직임) 만들기 ---
		staticText[0] = m_pGuiEnv->addStaticText(
			L"Micro-moving Control",
			irr::core::rect<irr::s32>(irr::core::position2di(5,20), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 미세조정(위로 이동) 버튼 만들기 ---
		button[0] = m_pGuiEnv->addButton(
			irr::core::recti(35,40,65,70),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON,
			L"UP",
			0);
		button[0]->setEnabled(moveControlFlag);

		// --- 미세조정(아래로 이동) 버튼 만들기 ---
		button[1] = m_pGuiEnv->addButton(
			irr::core::recti(35,70,65,100),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON,
			L"DN",
			0);
		button[1]->setEnabled(moveControlFlag);

		// --- 미세조정(왼쪽으로 이동) 버튼 만들기 ---
		button[2] = m_pGuiEnv->addButton(
			irr::core::recti(5,70,35,100),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON,
			L"LT",
			0);
		button[2]->setEnabled(moveControlFlag);

		// --- 미세조정(오른쪽으로 이동) 버튼 만들기 ---
		button[3] = m_pGuiEnv->addButton(
			irr::core::recti(65,70,95,100),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON,
			L"RT",
			0);
		button[3]->setEnabled(moveControlFlag);

		// --- 미세조정(앞으로 이동) 버튼 만들기 ---
		button[4] = m_pGuiEnv->addButton(
			irr::core::recti(5,100,35,130),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON,
			L"FRONT",
			0);
		button[4]->setEnabled(moveControlFlag);

		// --- 미세조정(뒤로 이동) 버튼 만들기 ---
		button[5] = m_pGuiEnv->addButton(
			irr::core::recti(65,100,95,130),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON,
			L"BACK",
			0);
		button[5]->setEnabled(moveControlFlag);


		// --- 회전이 필요할 경우 회전버튼 만들기 ---
		if(rotationControlFlag)
		{
			// --- 스태틱텍스트(미세 회전) 만들기 ---
			staticText[1] = m_pGuiEnv->addStaticText(
				L"Rotating Control",
				irr::core::rect<irr::s32>(irr::core::position2di(5,150), irr::core::dimension2du(90,20)),
				false, true,
				window,
				0,
				false
				);

			// --- 미세조정(좌로 회전) 버튼 만들기 ---
			button[6] = m_pGuiEnv->addButton(
				irr::core::recti(5,170,50,200),
				window,
				GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON,
				L"LT",
				0);
			button[6]->setEnabled(rotationControlFlag);

			// --- 미세조정(우로 회전) 버튼 만들기 ---
			button[7] = m_pGuiEnv->addButton(
				irr::core::recti(50,170,95,200),
				window,
				GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON,
				L"RT",
				0);
			button[7]->setEnabled(rotationControlFlag);
		}
	}
	//----------------------------------------------------------------------------------------------------
	// 위에서 할당한 적절한 변수를 참조하여 공통GUI(윈도우, 이동버튼)를 생성하는 코드 - 종료
	//----------------------------------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// 신노드 타입에 따라 각각 필요한 속성GUI를 생성하는 코드 - 시작
	//-------------------------------------------------------------------------
	// 블럭노드 속성 GUI 생성
	if(blockPropertyFlag)
	{
		// --- 블럭 타입 스태틱 텍스트 만들기 ---
		irr::gui::IGUIStaticText* staticText_type = m_pGuiEnv->addStaticText(
			L"Block Texture",
			irr::core::rect<irr::s32>(irr::core::position2di(5,220), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 블럭 타입 콤보박스 만들기 ---
		irr::gui::IGUIComboBox *box_type = m_pGuiEnv->addComboBox(
			irr::core::rect<irr::s32>(5,240,90,260),
			window,
			GUI_PROPERTY_WINDOW_BLOCK_TYPE_COMBOBOX
			);

		// --- 블럭 타입 콤보박스 아이템 추가 ---
		{
			irr::u16 i;
			for(i = 0 ; i < irr::scene::jz3d::BLOCK_TOTAL_COUNT ; ++i)
			{
				box_type->addItem(irr::scene::jz3d::CBlockNode::typeNames[i], i);
			}

			box_type->setSelected(blockNode->getBlockType());
		}

		// --- 블럭 마찰력 스태틱 텍스트 만들기 ---
		irr::gui::IGUIStaticText* staticText_friction = m_pGuiEnv->addStaticText(
			L"Block Friction",
			irr::core::rect<irr::s32>(irr::core::position2di(5,280), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 블럭 마찰력 스핀박스 만들기 ---
		{
			irr::gui::IGUISpinBox *box_friction = m_pGuiEnv->addSpinBox(
				L"",
				irr::core::rect<irr::s32>(5,300,90,320),
				true,
				window,
				GUI_PROPERTY_WINDOW_BLOCK_FRICTION_SPINBOX
				);
			box_friction->setRange(m_fFrictionMin, m_fFrictionMax);
			box_friction->setStepSize(m_fFrictionStep);
			box_friction->setValue(blockNode->m_fFriction);
		}

		// --- 블럭 탄성력 스태틱 텍스트 만들기 ---
		irr::gui::IGUIStaticText* staticText_restitution = m_pGuiEnv->addStaticText(
			L"Block Restitution",
			irr::core::rect<irr::s32>(irr::core::position2di(5,340), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 블럭 탄성력 스핀박스 만들기 ---
		{
			irr::gui::IGUISpinBox *box_restitution = m_pGuiEnv->addSpinBox(
				L"",
				irr::core::rect<irr::s32>(5,360,90,380),
				true,
				window,
				GUI_PROPERTY_WINDOW_BLOCK_RESTITUTION_SPINBOX
				);
			box_restitution->setRange(m_fRestitutionMin, m_fRestitutionMax);
			box_restitution->setStepSize(m_fRestitutionStep);
			box_restitution->setValue(blockNode->m_fRestitution);
		}

		// --- 블럭 사이즈 스태틱 텍스트 만들기 ---
		{
			irr::gui::IGUIStaticText* staticText_size = m_pGuiEnv->addStaticText(
				L"Block Size",
				irr::core::rect<irr::s32>(irr::core::position2di(5,400), irr::core::dimension2du(90,20)),
				false, true,
				window,
				0,
				false
				);
			staticText_size->setVisible(moveControlFlag);
		}
		
		// --- 블럭 사이즈 스핀박스 만들기 ---
		{
			irr::gui::IGUISpinBox *box_size = m_pGuiEnv->addSpinBox(
				L"",
				irr::core::rect<irr::s32>(5,420,90,440),
				true,
				window,
				GUI_PROPERTY_WINDOW_BLOCK_SIZE_SPINBOX
				);
			box_size->setRange(m_fSizeMin, m_fSizeMax);
			box_size->setStepSize(m_fSizeStep);
			box_size->setValue(blockNode->getBlockSize());
			box_size->setVisible(moveControlFlag);
		}

		// --- 블럭 속성 스태틱 텍스트 만들기 ---
		{
			irr::gui::IGUIStaticText* staticText_size = m_pGuiEnv->addStaticText(
				L"Block property",
				irr::core::rect<irr::s32>(irr::core::position2di(5,460), irr::core::dimension2du(90,20)),
				false, true,
				window,
				0,
				false
				);
			staticText_size->setVisible(moveControlFlag);
		}

		// --- 블럭 속성 콤보박스 만들기 ---
		{
			irr::gui::IGUIComboBox *box_property = m_pGuiEnv->addComboBox(
				irr::core::rect<irr::s32>(5,480,90,500),
				window,
				GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_COMBOBOX
				);

			box_property->setVisible(moveControlFlag);
			// --- 블럭 타입 콤보박스 아이템 추가 ---
			{
				irr::u16 i;
				for(i = 0 ; i < irr::scene::jz3d::BLOCK_PROPERTY_TOTAL_COUNT ; ++i)
				{
					box_property->addItem(irr::scene::jz3d::CBlockNode::propertyNames[i], i);
				}

				box_property->setSelected(blockNode->getBlockProperty());
			}
			
			// --- 블럭 타입 속성윈도우 만들기 ---
			{
				CreateBlockPropertyWindow(pNode, blockNode->getBlockProperty());
			}
		}
	}

	// 아이템노드 속성 GUI 생성
	if(itemPropertyFlag)
	{
		// 아이템노드로 형변환
		irr::scene::jz3d::CItemNode* itemNode = static_cast<irr::scene::jz3d::CItemNode*>( static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( pNode )->getFirstChild() );

		// --- 블럭 타입 스태틱 텍스트 만들기 ---
		irr::gui::IGUIStaticText* staticText_type = m_pGuiEnv->addStaticText(
			L"Item Type",
			irr::core::rect<irr::s32>(irr::core::position2di(5,220), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 아이템 타입 콤보박스 만들기 ---
		irr::gui::IGUIComboBox *box_type = m_pGuiEnv->addComboBox(
			irr::core::rect<irr::s32>(5,240,90,260),
			window,
			GUI_PROPERTY_WINDOW_ITEM_TYPE_COMBOBOX
			);

		// --- 아이템 타입 콤보박스 아이템 추가 ---
		{
			irr::u16 i;
			for(i = 0 ; i < irr::scene::jz3d::ITEM_TOTAL_COUNT ; ++i)
			{
				box_type->addItem(irr::scene::jz3d::CItemNode::typeNames[i], i);
			}
			box_type->setSelected( itemNode->getItemType() );
		}

		// --- 아이템 값 스태틱 텍스트 만들기 ---
		irr::gui::IGUIStaticText* staticText_friction = m_pGuiEnv->addStaticText(
			L"Item Value",
			irr::core::rect<irr::s32>(irr::core::position2di(5,280), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 아이템 값 스핀박스 만들기 ---
		{
			irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
				L"",
				irr::core::rect<irr::s32>(5,300,90,320),
				true,
				window,
				GUI_PROPERTY_WINDOW_ITEM_VALUE_SPINBOX
				);
			box_value->setRange(m_fItemMin, m_fItemMax);
			box_value->setStepSize(m_fItemStep);
			box_value->setValue(itemNode->m_fValue1);
		}

		// --- 아이템 지속시간 스태틱 텍스트 만들기 ---
		irr::gui::IGUIStaticText* staticText_restitution = m_pGuiEnv->addStaticText(
			L"Item DurationTime",
			irr::core::rect<irr::s32>(irr::core::position2di(5,340), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- 아이템 지속시간 스핀박스 만들기 ---
		{
			irr::gui::IGUISpinBox *box_durationtime = m_pGuiEnv->addSpinBox(
				L"",
				irr::core::rect<irr::s32>(5,360,90,380),
				true,
				window,
				GUI_PROPERTY_WINDOW_ITEM_DURATIONTIME_SPINBOX
				);
			box_durationtime->setRange(m_fItemMin, m_fItemMax);
			box_durationtime->setStepSize(m_fItemStep);
			box_durationtime->setValue(itemNode->m_fDurationTime);
		}
	}
	//-------------------------------------------------------------------------
	// 신노드 타입에 따라 각각 필요한 속성GUI를 생성하는 코드 - 종료
	//-------------------------------------------------------------------------
}

void CMapEditor::CreateBlockPropertyWindow(irr::scene::ISceneNode* pNode, irr::u32 block_property_type)
{
	// 인자값으로 들어오는 pNode는 블럭에 관한 것이므로 블럭신노드로 형변환 한다.
	irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>(pNode);

	// 부모가 될 속성윈도우를 가져온다
	irr::gui::IGUIElement* propertywindow = m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_PROPERTY_WINDOW);
	irr::gui::IGUIElement* blockPropertyWindow = m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_WINDOW, true);
	if(blockPropertyWindow)
		blockPropertyWindow->remove();

	if(propertywindow)
	{
		// 속성윈도우에 블럭속성윈도우를 추가한다.
		irr::gui::IGUIWindow* window = m_pGuiEnv->addWindow(
			irr::core::rect<irr::s32>(irr::core::position2di(0,500), irr::core::dimension2du(100,200)),
			false,
			0,
			propertywindow,
			GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_WINDOW
			);

		window->getCloseButton()->setVisible(false);
		window->setDraggable(false);
		window->setDrawBackground(false);
		window->setDrawTitlebar(false);

		switch(block_property_type)
		{

		case irr::scene::jz3d::BLOCK_PROPERTY_NORMAL:
			{
				pBlockNode->setPropertyValue(0, 0);
				pBlockNode->setPropertyValue(1, 0);
				pBlockNode->setPropertyValue(2, 0);
			}
			break;
		
		case irr::scene::jz3d::BLOCK_PROPERTY_DESTROY:
			{
				// --- 파괴블럭  충돌횟수 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"Collision count for destroy",
						irr::core::rect<irr::s32>(irr::core::position2di(5,20), irr::core::dimension2du(90,40)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 파괴블럭 충돌횟수 스핀박스 만들기
				{
					irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
						L"",
						irr::core::rect<irr::s32>(5,40,90,60),
						true,
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE0_SPINBOX
						);
					box_value->setRange(m_fValueMin, m_fValueMax);
					box_value->setStepSize(1);
					box_value->setValue(pBlockNode->getPropertyValue(0));
				}
			}
			break;

		case irr::scene::jz3d::BLOCK_PROPERTY_MOVE:
			{
				// --- 이동블럭  X좌표 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"X point to move",
						irr::core::rect<irr::s32>(irr::core::position2di(5,20), irr::core::dimension2du(90,20)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 이동블럭 X좌표 스핀박스 만들기
				{
					irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
						L"",
						irr::core::rect<irr::s32>(5,30,90,50),
						true,
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE0_SPINBOX
						);
					box_value->setRange(m_fValueMin, m_fValueMax);
					box_value->setStepSize(m_fValueStep);
					box_value->setValue(pBlockNode->getPropertyValue(0));
				}

				// --- 이동블럭  Y좌표 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"Y point to move",
						irr::core::rect<irr::s32>(irr::core::position2di(5,60), irr::core::dimension2du(90,20)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 이동블럭 Y좌표 스핀박스 만들기
				{
					irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
						L"",
						irr::core::rect<irr::s32>(5,70,90,90),
						true,
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE1_SPINBOX
						);
					box_value->setRange(m_fValueMin, m_fValueMax);
					box_value->setStepSize(m_fValueStep);
					box_value->setValue(pBlockNode->getPropertyValue(1));
				}

				// --- 이동블럭  이동속도 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"moving speed",
						irr::core::rect<irr::s32>(irr::core::position2di(5,100), irr::core::dimension2du(90,20)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 이동블럭 이동속도 스핀박스 만들기
				{
					irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
						L"",
						irr::core::rect<irr::s32>(5,110,90,130),
						true,
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE2_SPINBOX
						);
					box_value->setRange(m_fValueMin, m_fValueMax);
					box_value->setStepSize(0.1f);
					//pBlockNode->setPropertyValue(2, 0.5f);
					box_value->setValue(pBlockNode->getPropertyValue(2));
				}

				// 애니메이션 시작 버튼 만들기
				{
					m_pGuiEnv->addButton(
						irr::core::recti(5,170,95,190),
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ANIMATION_BUTTON,
						L"START ANIMATION",
						0
						);
				}
			}
			break;

		case irr::scene::jz3d::BLOCK_PROPERTY_ROTATION:
			{
				// --- 회전블럭  회전방향 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"Rotating Direction",
						irr::core::rect<irr::s32>(irr::core::position2di(5,20), irr::core::dimension2du(90,40)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 회전블럭  회전방향 콤보박스 만들기 ---
				{
					irr::gui::IGUIComboBox *box = m_pGuiEnv->addComboBox(
						irr::core::rect<irr::s32>(5,40,90,60),
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ROTATION_COMBOBOX
						);

					// --- 회전블럭 회전방향 콤보박스 아이템 추가 ---

					irr::u16 i;
					for(i = 0 ; i < irr::scene::jz3d::BLOCK_PROPERTY_ROTATING_DIRECTION_TOTAL_COUNT ; ++i)
					{
						box->addItem(irr::scene::jz3d::CBlockNode::rotationBlockDirectionNames[i], i);
					}
					box->setSelected(static_cast<irr::u32>(pBlockNode->getPropertyValue(0)));
				}

				// --- 회전블럭  회전속도 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"Rotating speed",
						irr::core::rect<irr::s32>(irr::core::position2di(5,80), irr::core::dimension2du(90,100)),
						false, true,
						window,
						0,
						false
						);
				}
				
				// --- 회전블럭 회전속도 스핀박스 만들기
				{
					irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
						L"",
						irr::core::rect<irr::s32>(5,100,90,120),
						true,
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE1_SPINBOX
						);
					box_value->setRange(m_fValueMin, m_fValueMax);
					box_value->setStepSize(0.1f);
					//pBlockNode->setPropertyValue(1, 0.5f);
					box_value->setValue(pBlockNode->getPropertyValue(1));
				}

				// 애니메이션 시작 버튼 만들기
				{
					m_pGuiEnv->addButton(
						irr::core::recti(5,170,95,190),
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ANIMATION_BUTTON,
						L"START ANIMATION",
						0
						);
				}
			}
			break;

		case irr::scene::jz3d::BLOCK_PROPERTY_SPRING:
			{
				// --- 스프링블럭  스프링탄력방향 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"Spring Direction",
						irr::core::rect<irr::s32>(irr::core::position2di(5,20), irr::core::dimension2du(90,40)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 스프링블럭  스프링탄력방향 콤보박스 만들기 ---
				{
					irr::gui::IGUIComboBox *box = m_pGuiEnv->addComboBox(
						irr::core::rect<irr::s32>(5,40,90,60),
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_SPRING_COMBOBOX
						);

					// --- 스프링블럭 스프링탄력방향 콤보박스 아이템 추가 ---
					irr::u16 i;
					for(i = 0 ; i < irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_TOTAL_COUNT ; ++i)
					{
						box->addItem(irr::scene::jz3d::CBlockNode::springBlockDirectionNames[i], i);
					}
					box->setSelected(static_cast<irr::u32>(pBlockNode->getPropertyValue(0)));
				}

				// --- 스프링블럭  스프링탄력 스태틱 텍스트 만들기 ---
				{
					irr::gui::IGUIStaticText* staticText = m_pGuiEnv->addStaticText(
						L"Spring force",
						irr::core::rect<irr::s32>(irr::core::position2di(5,80), irr::core::dimension2du(90,100)),
						false, true,
						window,
						0,
						false
						);
				}

				// --- 스프링블럭 스프링탄력 스핀박스 만들기
				{
					irr::gui::IGUISpinBox *box_value = m_pGuiEnv->addSpinBox(
						L"",
						irr::core::rect<irr::s32>(5,100,90,120),
						true,
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE1_SPINBOX
						);
					box_value->setRange(m_fValueMin, m_fValueMax);
					box_value->setStepSize(10);
					box_value->setValue(pBlockNode->getPropertyValue(1));
				}
			}
			break;
		}
	}
}

void CMapEditor::Clear()
{
	// 신메니저 클리어
	{
		m_pSmgr->clear();
	}

	// 클리어 상태 플래그
	{
		m_bIsClear = true;
	}
}

void CMapEditor::Reset()
{
	// (1) 클리어 실시
	if(!m_bIsClear)
		Clear();

	// (2) 신그래프 만들기
	CreateSceneGraph();

	// (3) 배경 만들기(힐플래인) 
	CreateHillPlane();

	// (4) 카메라 만들기
	CreateCamera();

	// 클리어 상태 플래그
	{
		m_bIsClear = false;
	}
}

void CMapEditor::ChangeMode(MAIN_STATUS mode)
{
	bool menu, editor;

	switch(mode)
	{
	case MENU:
		{
			menu = true;
			editor = false;

			m_uMainStatus = MENU;
		}
		break;

	case EDITOR:
		{
			menu = false;
			editor = true;

			m_uMainStatus = EDITOR;
		}
		break;
	}


	irr::u32 i, j;

	for(i = 0 ; i < m_pGUI_menu.size() ; i++)
	{
		m_pGUI_menu[i]->setEnabled(menu);
		m_pGUI_menu[i]->setVisible(menu);
	}

	for(j = 0 ; j < m_pGUI_Editor.size() ; j++)
	{
		m_pGUI_Editor[j]->setEnabled(editor);
		m_pGUI_Editor[j]->setVisible(editor);
	}

	ChangeEditorMode(MODE_STAND_BY);
}

void CMapEditor::ChangeEditorMode(EDITOR_MODE mode)
{
	// 모드 상태 변환
	m_uEditorStatus = mode;

	// 속성윈도우가 만들어져있다면 속성윈도우를 삭제한다.
	{
		// 속성윈도우 포인터 가져오기
		irr::gui::IGUIElement* propertywindow = m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_PROPERTY_WINDOW);

		// NULL포인터가 아니면, 즉 속성윈도우가 있다면 삭제한다.
		if(propertywindow)
			propertywindow->remove();
	}

	switch(mode)
	{
	case MODE_STAND_BY:
		{
			// 피킹된 신노드 없음
			m_pPickingNode = NULL;

			// 모두 이용가능한 상태로 전환
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(true);

			// 삭제버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// 복사버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// 로그 메시지 출력
			MessegeLogToStaticText(L"");
		}
		break;

	case MODE_MAKE_BLOCK:
		{
			// 블럭만들기 버튼 비활성 상태
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(false);

			// 나머지 활성상태
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(true);

			// 삭제버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// 복사버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// 로그 메시지 출력
			MessegeLogToStaticText(L"Creating a Block Node : Drag a mouse if you want to create..!!");
		}
		break;

	case MODE_MAKE_ITEM:
		{
			// 아이템만들기 버튼 비활성 상태
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(false);

			// 나머지 활성상태
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(true);

			// 삭제버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// 복사버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// 로그 메시지 출력
			MessegeLogToStaticText(L"Creating a Item Node : Click the mouse if you want to create..!!");
		}
		break;

	case MODE_MAKE_PORTAL:
		{
			// 포탈만들기 버튼 비활성 상태
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(false);

			// 나머지 활성상태
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(true);

			// 삭제버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// 복사버튼 비활성화
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// 로그 메시지 출력
			MessegeLogToStaticText(L"Creating Portal Node set : Click the mouse if you want to create..!!");
		}
		break;

	case MODE_PICK_NODE:
		{
			// 피킹 모드 상태
			m_uEditorStatus = MODE_PICK_NODE;

			// 속성윈도우를 만듬.
			CreatePropertyWindow(m_pPickingNode);

			// 피킹된 노드가 배경노드(시작, 종료, 외곽 블럭노드)이면 삭제버튼을 비활성화
			// 배경노드가 아니면 삭제버튼을 활성화 시킴
			if(	CheckBackgoundNode(m_pPickingNode) || CheckPointNode(m_pPickingNode) )
			{
				// 삭제버튼 비활성화
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);
				// 복사버튼 비활성화
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);
			}
			else
			{
				// 삭제버튼 비활성화
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(true);
				// 복사버튼 비활성화
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(true);
			}
		}
		break;

	}
}

bool CMapEditor::CheckBackgoundNode(irr::scene::ISceneNode* pNode)
{
	// 외곽 블럭노드인가 체크
	irr::u32 i;
	for(i = 0 ; i < 4 ; i++)
	{
		if(m_pPickingNode == m_pBackGroundBlockNode[i])
			return true;
	}
	return false;
}

bool CMapEditor::CheckPointNode(irr::scene::ISceneNode* pNode)
{
	// 시작-종료 지점을 나타내는 화살표 노드인가 체크
	if(pNode == m_pStartNode || pNode == m_pEndNode)
		return true;
	return false;
}

bool CMapEditor::SaveScene(irr::core::stringc file_name)
{
	// 카메라 정보
	irr::core::vector3df cam_pos;
	irr::core::vector3df cam_target;
	irr::core::vector3df cam_upVector;

	// 카메라 지우기
	// 맵에 관한 정보를 저장하므로 카메라씬 까지 저장할 필요가 없다!!!
	{
		m_pSmgr->setActiveCamera(NULL);

		cam_pos = m_pCameraNode->getAbsolutePosition();
		cam_target = m_pCameraNode->getTarget();
		cam_upVector = m_pCameraNode->getUpVector();

		m_pCameraNode->remove();
		m_pCameraNode = NULL;
	}

	// 배경이 와어이프레임으로 되어 있다면 와이어프레임 해제
	bool wireframe;
	if(m_pBackGroundSceneNode->getMaterial(0).Wireframe)
	{
		m_pBackGroundSceneNode->getMaterial(0).Wireframe = false;
		wireframe = true;
	}
	else
		wireframe = false;

	if(m_pBackGroundSceneNode->getMaterial(0).getTexture(0) == NULL)
	{
		m_pBackGroundSceneNode->setVisible(false);
	}

	// 시작위치 노드 visible 끄기
	m_pStartNode->setVisible(false);
	
	// 루트신노드의 이름 생성
	m_pMainSceneNode->setName(file_name.c_str());

	// 파일 이름 보관
	m_sMapName = file_name;

	// irr 확장자 추가
	file_name.append(".irr");
	//씬 저장하기
	bool successFlag = m_pSmgr->saveScene(file_name.c_str());

	//와이어프레임 원상태로 돌리기
	if(wireframe)
		m_pBackGroundSceneNode->getMaterial(0).Wireframe = true;

	// 시작위치노드 원상태로 돌리기
	m_pStartNode->setVisible(true);

	// 배경노드 원상태로 돌리기
	m_pBackGroundSceneNode->setVisible(true);

	// 카메라씬을 다시 생성하기
	{
		m_pCameraNode = m_pSmgr->addCameraSceneNode();
		m_pCameraNode->setPosition(cam_pos);
		m_pCameraNode->setTarget(cam_target);
		m_pCameraNode->setUpVector(cam_upVector);
	}

	return successFlag;
}

bool CMapEditor::LoadScene(irr::core::stringc file_name)
{
	// 씬을 로딩하기 전에 먼저 클리어 실시
	if(!m_bIsClear)	
		Clear();

	// 씬매니저로 부터 신 로딩 하기
	if(!m_pSmgr->loadScene(file_name.c_str()))
		return false;

	// 메인씬노드 포인터 저장하기
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_pSmgr->getRootSceneNode()->getChildren().begin();
	
	m_pMainSceneNode = static_cast<irr::scene::ISceneNode*>( (*it) );

	// 피킹과 트라이앵글 셀렉터를 위한 포인터 저장 및 트라이앵글 셀렉터 등록
	{
		// "HillPlane"으로 되어 있는 신노드 검색
		irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName("HillPlane", m_pMainSceneNode);
		// 포인터 저장
		m_pBackGroundSceneNode = static_cast<irr::scene::IMeshSceneNode *>(pNode);

		// 트라이앵글 셀렉터 등록
		irr::scene::IMeshSceneNode *pMeshNode = m_pBackGroundSceneNode;
		irr::scene::ITriangleSelector *pSelector = m_pSmgr->createTriangleSelector(pMeshNode->getMesh(), pMeshNode);
		pMeshNode->setTriangleSelector(pSelector);
		pSelector->drop();
	}

	// 피킹과 에디팅에 필요한 멤버변수들 다시 설정
	{
		irr::scene::ISceneNode* pNode1,* pNode2;
		pNode1 = m_pSmgr->getSceneNodeFromName("BackGround", m_pMainSceneNode);
		pNode2 = m_pSmgr->getSceneNodeFromName("Point", m_pMainSceneNode);

		// 외곽 블럭 노드
		m_pBackGroundBlockNode[0] = m_pSmgr->getSceneNodeFromName("Up", pNode1);
		m_pBackGroundBlockNode[1] = m_pSmgr->getSceneNodeFromName("Down", pNode1);
		m_pBackGroundBlockNode[2] = m_pSmgr->getSceneNodeFromName("Left", pNode1);
		m_pBackGroundBlockNode[3] = m_pSmgr->getSceneNodeFromName("Right", pNode1);

		// 시작지점 노드
		m_pStartNode = m_pSmgr->getSceneNodeFromName("Start", pNode2);
		m_pStartNode->setVisible(true);
		// 종료지점 노드
		m_pEndNode = m_pSmgr->getSceneNodeFromName("End", pNode2);
	}

	return true;
}

void CMapEditor::AddMessege(Messege &messege)
{
	m_messegeQueue.push_back(messege);
}

void CMapEditor::AddWorkingHitory(WorkingHistory &history)
{
	//m_workingHistory.push_back(history);
}

irr::scene::ISceneNode* CMapEditor::MakeBlockNode(irr::core::vector3df start, irr::core::vector3df end)
{
	irr::scene::ISceneNode* node = m_pSmgr->getSceneNodeFromName("Block", m_pMainSceneNode);
	irr::scene::jz3d::CBlockNode* pNode = irr::scene::jz3d::CBlockNode::CreateBlockNode(node, m_pSmgr, -1, start, end, 0);

	// 기본 마찰력 적용
	pNode->m_fFriction = m_fBasicFrictionValue;
	// 기본 탄성력 적용
	pNode->m_fRestitution = m_fBasicRestitutionValue;
	// 기본 텍스쳐 적용
	pNode->setType(m_uBasicBlockTexture);
	// 기본 사이즈 적용
	pNode->setSize(m_fBasicBlockSize);

	return pNode;
}

irr::scene::ISceneNode* CMapEditor::MakeItemNode(irr::core::vector3df position)
{
	irr::scene::ISceneNode* node = m_pSmgr->getSceneNodeFromName("Item", m_pMainSceneNode);
	irr::scene::jz3d::CEmptyBBoxNode *boxNode = irr::scene::jz3d::CEmptyBBoxNode::CreateEmptyBBoxNode(
		node, m_pSmgr, -1,
		position, 
		irr::core::vector3df(11)
		);

	irr::scene::jz3d::CItemNode* pNode = irr::scene::jz3d::CItemNode::CreateItemNode(boxNode, m_pSmgr, -1, irr::core::vector3df(0,0,0));

	return boxNode;
}

irr::scene::ISceneNode* CMapEditor::MakeAroowNode(irr::core::vector3df position)
{
	irr::scene::ISceneNode* node = m_pSmgr->getSceneNodeFromName("Arrow", m_pMainSceneNode);

	// 화살표노드를 피킹할 바운딩박스 노드 생성
	irr::scene::jz3d::CEmptyBBoxNode *pBoxNode = irr::scene::jz3d::CEmptyBBoxNode::CreateEmptyBBoxNode(
		node, m_pSmgr, -1,
		position,
		irr::core::vector3df(20)
		);

	irr::scene::jz3d::CPositionNode* ArrowNode = irr::scene::jz3d::CPositionNode::CreatePositionNode(
		pBoxNode, m_pSmgr, -1,
		 irr::core::vector3df(0,0,0),
		true
		);

	return pBoxNode;
}

irr::scene::ISceneNode* CMapEditor::MakePortalNode(irr::core::vector3df position)
{
	irr::scene::ISceneNode* node = m_pSmgr->getSceneNodeFromName("Portal", m_pMainSceneNode);

	// 포탈은 2개의 셋트로 이루어져 있으므로 이 세트를 관리할 빈노드를 먼저 만들고 2개의 입구, 출구포탈을 자식으로 붙인다.
	irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode();

	// 자식으로 붙여질 포탈노드의 이름과 위치 설정
	irr::core::stringc name("PortalSet/");
	name += node->getChildren().getSize() + 1;
	pNode->setName(name.c_str());
	irr::core::vector3df newPos = position;
	newPos.Y = 0;
	pNode->setPosition(newPos);

	//부모 설정
	node->addChild(pNode);

	// 입구포탈 생성
	{
		// 가로세로 사이즈(10, 10), 부모 pNode의 자식으로, 부모로부터 x축으로 -20만큼 떨어진곳에 생성
		irr::scene::jz3d::CPortalNode* pPortalNode = irr::scene::jz3d::CPortalNode::CreatePortalNode(
			pNode, m_pSmgr, -1,
			irr::core::dimension2df(30, 30),
			irr::core::vector3df(-20, 0, 0),
			irr::scene::jz3d::PORTAL_IN
			);
	}

	// 출구포탈 생성
	{
		// 가로세로 사이즈(10, 10), 부모 pNode의 자식으로, 부모로부터 x축으로 -20만큼 떨어진곳에 생성
		irr::scene::jz3d::CPortalNode* pPortalNode = irr::scene::jz3d::CPortalNode::CreatePortalNode(
			pNode, m_pSmgr, -1,
			irr::core::dimension2df(30, 30),
			irr::core::vector3df(20, 0, 0),
			irr::scene::jz3d::PORTAL_OUT
			);
	}

	return pNode;
}

bool CMapEditor::CreateSceneGraph()
{
	irr::scene::ISceneNode *m_pRootNode = m_pSmgr->getRootSceneNode();

	// 메인 노드
	m_pMainSceneNode = m_pSmgr->addEmptySceneNode(m_pRootNode);

	// 스테이지 노드
	{
		irr::scene::ISceneNode* pStageNode = m_pSmgr->addEmptySceneNode(m_pMainSceneNode);
		pStageNode->setName("Stage");

		// 아이템
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Item");
		}

		// 블럭
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Block");
		}

		// 포탈
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Portal");
		}

		// 화살표
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Arrow");
		}
	}

	// Point 노드
	{
		irr::scene::ISceneNode* pPointNode = m_pSmgr->addEmptySceneNode(m_pMainSceneNode);
		pPointNode->setName("Point");
	}

	// 배경
	{
		irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(m_pMainSceneNode);
		pNode->setName("BackGround");
	}

	return true;
}

bool CMapEditor::CreateHillPlane()
{
	// 백그라운드 신노드 가져오기
	irr::scene::ISceneNode* node = m_pSmgr->getSceneNodeFromName("BackGround", m_pMainSceneNode);

	// 백그라운드 신노드를 부모로 하는 힐플래인 노드 만들기
	// 인자값 : 부모노드, 신매니저, ID, 힐플래인노드의 위치, 기본사이즈, 맵사이즈
	irr::scene::jz3d::CHillPlaneNode* pNode = irr::scene::jz3d::CHillPlaneNode::CreateHillPlaneNode(
		node, m_pSmgr, -1, 
		irr::core::vector3df( 0, m_uBlockBasicSize * -1.f , 0 ),
		irr::core::dimension2df( static_cast<irr::f32>(m_uMapSize.Width), static_cast<irr::f32>(m_uMapSize.Height) )
		);

	// 재질설정. 라이트닝과 와이어프레임
	pNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	pNode->setMaterialFlag(irr::video::EMF_WIREFRAME, true);

	// 이름 설정
	pNode->setName("HillPlane");

	// 피킹과 트라이앵글 셀렉터를 위한 포인터 저장 및 트라이앵글 셀렉터 등록
	{
		// 포인터 저장
		m_pBackGroundSceneNode = pNode;

		// 트라이앵글 셀렉터 등록
		irr::scene::IMeshSceneNode *pMeshNode = static_cast<irr::scene::IMeshSceneNode *>(pNode);
		irr::scene::ITriangleSelector *pSelector = m_pSmgr->createTriangleSelector(pMeshNode->getMesh(), pMeshNode);
		pMeshNode->setTriangleSelector(pSelector);
		pSelector->drop();
	}


	// 배경화면에 블럭만들기
	{
		// 배경노드
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("BackGround", m_pMainSceneNode);

		// 윗부분 블럭
		irr::scene::jz3d::CBlockNode* pNode0 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/2.f),
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/2.f),
			0
			);

		pNode0->setName("Up");
		m_pBackGroundBlockNode[0] = pNode0;

		// 아랫부분 블럭
		irr::scene::jz3d::CBlockNode* pNode1 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/-2.f),
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/-2.f),
			0
			);
		pNode1->setName("Down");
		m_pBackGroundBlockNode[1] = pNode1;

		// 왼쪽부분 블럭
		irr::scene::jz3d::CBlockNode* pNode2 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/2.f),
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/-2.f),
			0
			);
		pNode2->setName("Left");
		m_pBackGroundBlockNode[2] = pNode2;

		// 오른쪽부분 블럭
		irr::scene::jz3d::CBlockNode* pNode3 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/2.f),
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/-2.f),
			0
			);
		pNode3->setName("Right");
		m_pBackGroundBlockNode[3] = pNode3;
	}


	// 시작위치와 종료위치 설정하기
	{
		// 포인트노드
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("Point", m_pMainSceneNode);

		// 시작위치노드
		{
			// 시작위치 계산
			irr::core::vector3df position( m_uMapSize.Width/-2.f + (irr::f32) m_uBlockBasicSize * 3, 0, m_uMapSize.Height/2.f - (irr::f32) m_uBlockBasicSize * 3 );

			// 위치노드를 피킹할 바운딩박스 노드 생성, 위에서 계산한 위치로 설정
			irr::scene::jz3d::CEmptyBBoxNode *pStartNode = irr::scene::jz3d::CEmptyBBoxNode::CreateEmptyBBoxNode(
				pNode, m_pSmgr, -1,
				position,
				irr::core::vector3df(20)
				);

			// 위치노드를 생성하고 바운딩박스노드의 자식으로 붙이다.
			irr::scene::jz3d::CPositionNode* pPositionNode = irr::scene::jz3d::CPositionNode::CreatePositionNode(
				pStartNode,
				m_pSmgr,
				-1,
				irr::core::vector3df(0),
				true
				);

			pStartNode->setName("Start");
			m_pStartNode = pStartNode;
		}

		// 종료위치노드
		{
			// 시작위치 계산
			irr::core::vector3df position( m_uMapSize.Width/2.f - (irr::f32) m_uBlockBasicSize * 3, 0, m_uMapSize.Height/-2.f + (irr::f32) m_uBlockBasicSize * 3 );

			//// 위치노드를 피킹할 바운딩박스 노드 생성, 위에서 계산한 위치로 설정
			//irr::scene::jz3d::CEmptyBBoxNode *pEndNode = irr::scene::jz3d::CEmptyBBoxNode::CreateEmptyBBoxNode(
			//	pNode, m_pSmgr, -1,
			//	position,
			//	irr::core::vector3df(15)
			//	);

			// 위치노드를 생성하고 바운딩박스노드의 자식으로 붙이다.
			irr::scene::jz3d::CPortalNode* pEndNode = irr::scene::jz3d::CPortalNode::CreatePortalNode(
				pNode, m_pSmgr, -1,
				irr::core::dimension2df(50, 50),
				position,
				irr::scene::jz3d::PORTAL_END
				);

			pEndNode->setName("End");
			m_pEndNode = pEndNode;
		}
	}

	
	return true;
}

bool CMapEditor::CreateCamera()
{
	m_pCameraNode = m_pSmgr->addCameraSceneNode();

	m_pCameraNode->setPosition(irr::core::vector3df(0, 400,0));
	m_pCameraNode->setTarget(irr::core::vector3df(0,0,0));
	m_pCameraNode->setUpVector(irr::core::vector3df(0,0,1));

	m_pCameraNode->setName("Camera");
	m_pSmgr->setActiveCamera(m_pCameraNode);

	m_pSmgr->getRootSceneNode()->addChild(m_pCameraNode);

	return true;
}

void CMapEditor::MessegeLogToStaticText(wchar_t* msg)
{
	m_pStaticText->setText(msg);
}

void CMapEditor::MessegeLogToStaticText(wchar_t* msg, irr::core::vector3df vec)
{
	irr::core::stringw messege(msg);
	messege += " : ";
	messege += vec.X;
	messege += " : ";
	messege += vec.Y;
	messege += " : ";
	messege += vec.Z;

	m_pStaticText->setText(messege.c_str());
}

irr::core::dimension2di CMapEditor::GetMouse2DPosition()
{
	return m_pDevice->getCursorControl()->getPosition();
}

bool CMapEditor::GetPositionFromBackgourndNode(irr::core::dimension2di position_mouse, irr::core::vector3df &position_3d)
{
	// 마우스좌표와 카메라로부터 피킹광선 얻어오기
	irr::core::line3df Ray = m_pSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates(position_mouse, m_pCameraNode);

	// 충돌좌표를 계산할때 필요한 변수들.
	irr::core::triangle3df tri;
	const irr::scene::ISceneNode* pNode;

	// 피킹광선으로 충돌좌표 얻어오기. 즉 힐플레인메쉬와 충돌광선의 충돌 좌표값 얻어오기
	// 충돌하여 좌표값이 있다면 true를 반환.
	bool pick = m_pSmgr->getSceneCollisionManager()->getCollisionPoint(
		Ray, 
		m_pBackGroundSceneNode->getTriangleSelector(),
		position_3d,
		tri,
		pNode
		);

	return pick;
}

irr::scene::ISceneNode* CMapEditor::GetPickingNode(irr::core::dimension2di position_mouse)
{
	irr::core::line3df Ray;
	irr::core::triangle3df tri;

	// 마우스좌표로 부터 피킹레이 만들기
	Ray = m_pSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates(position_mouse);

	// 피킹레이로 피킹된 신노드 얻기
	irr::scene::ISceneNode* pNode = m_pSmgr->getSceneCollisionManager()->getSceneNodeFromRayBB(Ray);

	// 피킹된 신노드가 배경, 카메라, 배경블럭이면 NULL포인터 반환. 즉 피킹되지 않았다는 의미.
	if(m_pBackGroundSceneNode == pNode || m_pCameraNode == pNode)
		return NULL;

	/*if(m_pBackGroundSceneNode == pNode || m_pCameraNode == pNode || 
		m_pBackGroundBlockNode[0] == pNode || m_pBackGroundBlockNode[1] == pNode || 
		m_pBackGroundBlockNode[2] == pNode || m_pBackGroundBlockNode[3] == pNode)
		return NULL;*/

	return pNode;
}

bool CMapEditor::ProccessingMessegeQueue(Messege &messege)
{
	switch(messege.type)
	{
	case Messege::MSG_RESET:
		{

		}
		break;

	case Messege::MSG_CLEAR:
		{

		}
		break;

	case Messege::MSG_CHANGE_MAIN_MODE:
		{
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);
			
			switch(value)
			{
			// 메인메뉴로 전환
			case MENU:
				{
					Clear();
					ChangeMode(MENU);
				}
				break;

			// 에디터로 전환
			case EDITOR:
				{
					ChangeMode(EDITOR);
				}
				break;
			}
		}
		break;

	case Messege::MSG_CHANGE_EDITOR_MODE:
		{
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			switch(value)
			{
			case MODE_STAND_BY:
				{
					ChangeEditorMode(MODE_STAND_BY);
				}
				break;

			case MODE_MAKE_BLOCK:
				{
					ChangeEditorMode(MODE_MAKE_BLOCK);
				}
				break;

			case MODE_MAKE_ITEM:
				{
					ChangeEditorMode(MODE_MAKE_ITEM);
				}
				break;

			case MODE_MAKE_PORTAL:
				{
					ChangeEditorMode(MODE_MAKE_PORTAL);
				}
				break;
			}
		}
		break;

	case Messege::MSG_PUSH_NEW_BUTTON:
		{
			// 맵사이즈 윈도우 만들기
			CreateMapSizeWindow();
		}
		break;

	case Messege::MSG_PUSH_MAPSIZE_OK_BUTTON:
		{
			// 맵 사이즈 설정
			m_uMapSize.set( static_cast<irr::u32>(messege.fData[0]), static_cast<irr::u32>(messege.fData[1]));

			// 리셋 하기
			Reset();

			// 에디터모드로 설정
			ChangeMode(EDITOR);
		}
		break;

	case Messege::MSG_PUSH_SAVE_BUTTON:
		{
			// 세이브 윈도우 만들기
			CreateSaveWindow();
		}
		break;

	case Messege::MSG_PUSH_SAVE_WINDOW_OK_BUTTON:
		{
			if(m_pDevice->getFileSystem()->changeWorkingDirectoryTo("./Map"))
			{
				// 씬 저장하기
				if(SaveScene(messege.cData[0]))
					MessegeLogToStaticText(L"[SAVE] success !!");
				m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../");
			}
			else
				MessegeLogToStaticText(L"[SAVE] failed !!");
		}
		break;

	case Messege::MSG_PUSH_LOAD_BUTTON:
		{
			if(m_pDevice->getFileSystem()->changeWorkingDirectoryTo("./Map"))
			{
				// 오픈파일다이얼로그 만들기
				CreateLoadFileDialog();

				m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../");
			}
		}
		break;

	case Messege::MSG_LOAD_SCENE_FROM_FILENAME:
		{
			// 씬 로딩하기 - 로딩에 성공하였다면
			if(LoadScene(messege.cData[0]))
			{
				// 카메라 만들기
				CreateCamera();

				// 에디터모드로  상태변환
				ChangeMode(EDITOR);

				MessegeLogToStaticText(L"[Load] success !!");
			}
		}
		break;

	case Messege::MSG_PUSH_MAKE_BUTTON:
		{
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			switch(value)
			{
			case GUI_MAKE_BLOCK_BUTTON:
				{
					ChangeEditorMode(MODE_MAKE_BLOCK);
				}
				break;

			case GUI_MAKE_ITEM_BUTTON:
				{
					ChangeEditorMode(MODE_MAKE_ITEM);
				}
				break;

			case GUI_MAKE_PORTAL_BUTTON:
				{
					ChangeEditorMode(MODE_MAKE_PORTAL);
				}
				break;

			case GUI_MAKE_ARROW_BUTTON:
				{
					ChangeEditorMode(MODE_MAKE_ARROW);
				}
				break;
			}
		}
		break;

	case Messege::MSG_PUSH_COPY_BUTTON:
		{
			CopySceneNode(m_pPickingNode);

			MessegeLogToStaticText(L"[COPY] a scene !!");
		}
		break;

	case Messege::MSG_PUSH_DELETE_BUTTON:
		{
			switch(m_pPickingNode->getType())
			{

				// 블럭노드이면 바로 블럭노드 삭제
			case irr::scene::jz3d::CBlockNode::TypeID:
				{
					m_pPickingNode->remove();
					MessegeLogToStaticText(L"[DELETE] Block Node !!");
				}
				break;

				// 바운딩박스노드이면 자식으로 아이템노드가 있다.
				// 따라서 바운딩박스노드를 삭제하면 아이템노드를 삭제하는 효과가 있음.
			case irr::scene::jz3d::CEmptyBBoxNode::TypeID:
				{
					m_pPickingNode->remove();
					MessegeLogToStaticText(L"[DELETE] ITEM Node !!");
				}
				break;

				// 포탈노드는 한쌍으로 이루어져 있고 그 부모가 있으므로, 부모를 삭제하여 포탈노드세트를 모두 삭제.
			case irr::scene::jz3d::CPortalNode::TypeID:
				{
					m_pPickingNode->getParent()->remove();
					MessegeLogToStaticText(L"[DELETE] Portal Node !!");
				}
				break;
			}

			// 스텐바이 상태로 전환
			ChangeEditorMode(MODE_STAND_BY);
		}
		break;

	case Messege::MSG_PUSH_RESTAET_ALL_ANIMATOR_BUTTON:
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName("Block");
			
			const irr::core::list<irr::scene::ISceneNode*> children = pNode->getChildren();
			irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = children.begin();

			for(it ; it != children.end() ; ++it)
			{
				if( (*it)->getType() == irr::scene::jz3d::CBlockNode::TypeID )
				{
					static_cast<irr::scene::jz3d::CBlockNode*>( (*it) )->startAnimation();
				}
			}
		}
		break;

	case Messege::MSG_PUSH_TEXTURE_BUTTON:
		{
			if(m_pDevice->getFileSystem()->changeWorkingDirectoryTo("./BackGround"))
			{
				// 텍스쳐 파일 선택 다이얼로그 만들기
				CreateBGSceneTextureFileDialog();
				m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../");
			}
		}
		break;

	case Messege::MSG_SETTEXTURE_TO_BGSCENE_FROM_FILENAME:
		{
			//-------------------------------------------------------
			// irr::core::stringc가 아닌 std::string을 이용하였다.
			// algorithm의 transform이 사용하기능하기 때문.
			//-------------------------------------------------------

			// 파일오픈다이얼로그로부터 파일명 얻어오기
			std::string fileName(messege.cData[0].c_str());

			// 현재 작업디렉토리를 얻어오기
			std::string path = m_pDevice->getFileSystem()->getWorkingDirectory().c_str();
			// 작업디렉토리는 대소문자를 구분하므로 모두 소문자로 바꿔줌.
			std::transform(path.begin(), path.end(), path.begin(), tolower);

			// 파일명문자열로부터 작업디렉토리명의 사이즈+1만큼 제거한다. 즉, 상대경로만 남게 된다.
			std::string relative_fileName = fileName.substr(path.size()+1);
			
			// 상대경로로 되어있는 파일명을 텍스쳐로 사용한다.
			irr::video::ITexture* textrue = m_pVideo->getTexture(relative_fileName.c_str());
			if(textrue)
			{
				m_pBackGroundSceneNode->getMaterial(0).setTexture(0, textrue);
				m_pBackGroundSceneNode->getMaterial(0).Wireframe = false;
				//m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_CHANGE_ALL_BLOCK_PROPERTY_BUTTON)->setText(L"Wireframe\nOFF");
				MessegeLogToStaticText(L"[LOAD] a backgroundNode texture success !!");
			}
			else
			{
				MessegeLogToStaticText(L"[LOAD] a backgroundNode texture failed !!");
			}
		}
		break;

	case Messege::MSG_PUSH_CHANGE_ALL_BLOCK_PROPERTY_BUTTON:
		{
			// 배경외의 블럭노드 값 변화
			{
				irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName("Block");
				const irr::core::list<irr::scene::ISceneNode*> children = pNode->getChildren();
				irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = children.begin();

				for(it ; it != children.end() ; ++it)
				{
					if( (*it)->getType() == irr::scene::jz3d::CBlockNode::TypeID )
					{
						irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>( (*it) );
						pBlockNode->setType(m_uBasicBlockTexture);
						pBlockNode->m_fFriction = m_fBasicFrictionValue;
						pBlockNode->m_fRestitution = m_fBasicRestitutionValue;
					}
				}
			}


			// 배경 블럭노드 값 변화
			{
				irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName("BackGround");
				const irr::core::list<irr::scene::ISceneNode*> children = pNode->getChildren();
				irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = children.begin();

				for(it ; it != children.end() ; ++it)
				{
					if( (*it)->getType() == irr::scene::jz3d::CBlockNode::TypeID )
					{
						irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>( (*it) );
						pBlockNode->setType(m_uBasicBlockTexture);
						pBlockNode->m_fFriction = m_fBasicFrictionValue;
						pBlockNode->m_fRestitution = m_fBasicRestitutionValue;
					}
				}
			}
		}
		break;

	case Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON:
		{
			// 메시지의 fData값으로 미세움직임 조정
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			switch(value)
			{
				// 신노드를 위로 이동
			case GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, 0, m_fSceneNodeMicroMoveValue ));
				}
				break;

				// 신노드를 아래로 이동
			case GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, 0, -m_fSceneNodeMicroMoveValue ));
				}
				break;

				// 신노드를 좌로 이동
			case GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( -m_fSceneNodeMicroMoveValue, 0, 0 ));
				}
				break;

				// 신노드를 우로 이동
			case GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( m_fSceneNodeMicroMoveValue, 0, 0 ));
				}
				break;

				// 신노드를 앞으로 이동
			case GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, m_fSceneNodeMicroMoveValue, 0 ) );
				}
				break;

				// 신노드를 뒤로 이동
			case GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, -m_fSceneNodeMicroMoveValue, 0 ) );
				}
				break;
			}

			// 로그 메시지 보내기
			MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
		}
		break;

	case Messege::MSG_PUSH_PROPERTY_MICROROTATION_BUTTON:
		{
			// 메시지의 fData값으로 미세움직임 조정
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			switch(value)
			{
				// 신노드를 Y축을 기준으로 m_fSceneNodeMicroRotationValue만큼 좌회전
			case GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON:
				{
					m_pPickingNode->setRotation(m_pPickingNode->getRotation() + irr::core::vector3df( 0, -m_fSceneNodeMicroRotationValue ,0));
				}
				break;

				// 신노드를 Y축을 기준으로 m_fSceneNodeMicroRotationValue만큼 우회전
			case GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON:
				{
					m_pPickingNode->setRotation(m_pPickingNode->getRotation() + irr::core::vector3df( 0, +m_fSceneNodeMicroRotationValue ,0));
				}
				break;
			}

			if(m_pPickingNode->getType() == irr::scene::jz3d::CBlockNode::TypeID)
			{
				irr::core::vector3df rotation = m_pPickingNode->getRotation();
				static_cast<irr::scene::jz3d::CBlockNode*>(m_pPickingNode)->setStartRotation(rotation);
			}

			// 로그 메시지 보내기
			MessegeLogToStaticText(L"[Rotation] ", m_pPickingNode->getRotation());
		}
		break;

	case Messege::MSG_PUSH_PROPERTY_ANIMATION_BUTTON:
		{
			// 블럭노드로 형변환
			irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode );
			
			// 블럭노드의 애니메이션 시작
			pBlockNode->startAnimation();
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_TYPE:
		{
			// 블럭노드의 타입 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setType( static_cast<irr::u32>(messege.fData[0]) );

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node type !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY:
		{
			// 값 형변환
			irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			// 형변환 하기
			irr::scene::jz3d::CBlockNode* pBlockNode =	static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode );

			// 블럭노드의 애니메이션 삭제
			pBlockNode->deleteAnimation();

			// 블럭노드의 속성값들 모두 초기화
			pBlockNode->deleteBlockPropertyValues();

			// 블럭노드의 타입 바꾸기
			pBlockNode->setProperty( static_cast<irr::u32>(messege.fData[0]) );

			// 블럭 속성창 만들기
			CreateBlockPropertyWindow(m_pPickingNode, value );

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node property !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0:
		{
			// 블럭노드의 속성값1 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setPropertyValue(0, messege.fData[0]);

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node property value1 !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE1:
		{
			// 블럭노드의 속성값1 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setPropertyValue(1, messege.fData[0]);

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node property value1 !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE2:
		{
			// 블럭노드의 속성값1 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setPropertyValue(2, messege.fData[0]);

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node property value2 !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_FRICTION:
		{
			// 블럭노드의 마찰력 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->m_fFriction = messege.fData[0];

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node frinction !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_RESTITUTION:
		{
			// 블럭노드의 탄성력 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->m_fRestitution = messege.fData[0];

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node restitution !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_SIZE:
		{
			// 블럭노드의 사이즈 바꾸기
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setSize( messege.fData[0] );

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Block Node size !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_ITEM_TYPE:
		{
			// 아이템노드의 타입 바꾸기
			irr::scene::ISceneNode* pNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( m_pPickingNode )->getFirstChild();
			static_cast<irr::scene::jz3d::CItemNode*>( pNode )->setType( static_cast<irr::s32>(messege.fData[0]) );

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Item Node type!!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_ITEM_VALUE:
		{
			// 아이템노드의 값 바꾸기
			irr::scene::ISceneNode* pNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( m_pPickingNode )->getFirstChild();
			static_cast<irr::scene::jz3d::CItemNode*>( pNode )->m_fValue1 = messege.fData[0];

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Item Node Value !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_ITEM_DURATIONTIME:
		{
			// 아이템노드의 지속시간 바꾸기
			irr::scene::ISceneNode* pNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( m_pPickingNode )->getFirstChild();
			static_cast<irr::scene::jz3d::CItemNode*>( pNode )->m_fDurationTime = messege.fData[0];

			// 로그메시지 보내기
			MessegeLogToStaticText(L"[CHANGE] Item Node DurationTime !!");
		}
		break;

	case Messege::MSG_CHANGE_BLOCK_BASIC_FRICTION:
		{
			m_fBasicFrictionValue = messege.fData[0];
		}
		break;

	case Messege::MSG_CHANGE_BLOCK_BASIC_RESTITUTION:
		{
			m_fBasicRestitutionValue = messege.fData[0];
		}
		break;

	case Messege::MSG_CHANGE_BLOCK_BASIC_TEXTURE:
		{
			m_uBasicBlockTexture = static_cast<irr::u32>(messege.fData[0]);
		}
		break;
	
	case Messege::MSG_CHANGE_BLOCK_BASIC_SIZE:
		{
			m_fBasicBlockSize = messege.fData[0];
		}
		break;
	}

	return false;
}

bool CMapEditor::ProccessingIOEvent()
{
	switch(m_uMainStatus)
	{
	case EDITOR:
		{
			//-------------------------------------------------------------------------------------------
			// 마우스 포인터가 GUI위에 놓여있지 않는 경우에만 입출력 이벤트 발생 - 코드 시작
			//-------------------------------------------------------------------------------------------
			if(!m_bGUIHoveredEvent)
			{
				// 카메라를 위로 스크롤 하기
				if(m_Key[irr::KEY_UP])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, 0, m_fCameraMoveValue );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// 카메라를 아래로 스크롤 하기
				if(m_Key[irr::KEY_DOWN])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, 0, -m_fCameraMoveValue );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// 카메라를 오른쪽으로 스크롤 하기
				if(m_Key[irr::KEY_RIGHT])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( m_fCameraMoveValue, 0, 0 );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// 카메라를 왼쪽으로 스크롤 하기
				if(m_Key[irr::KEY_LEFT])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( -m_fCameraMoveValue, 0, 0 );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// 카메라 줌 인
				if(m_Key_MouseWheel_Up)
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, -m_fCameraZoomValue, 0 );
					m_pCameraNode->setPosition(pos);

					m_Key_MouseWheel_Up = false;
				}

				// 카메라 줌 아웃
				if(m_Key_MouseWheel_Down)
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, m_fCameraZoomValue, 0 );
					m_pCameraNode->setPosition(pos);

					m_Key_MouseWheel_Down = false;
				}

				// 스텐바이 모드로 전환
				if(m_Key[irr::KEY_ESCAPE])
				{
					ChangeEditorMode(MODE_STAND_BY);
				}

				// 마우스 클릭을 위한 스태틱 변수와 드래그를 위한 플래그변수
				static irr::core::dimension2di first_click_pos, last_click_pos, end_click_pos;
				static bool dragFlag = false;

				if(m_Key[irr::KEY_LBUTTON])
				{
					//------------------- 첫번째 클릭 코드 시작-------------------
					if(!dragFlag)
					{
						// 드래그 상태로 전환
						dragFlag = true;

						// 마우스의 처음과 마지막 위치값 저장
						first_click_pos = last_click_pos = end_click_pos = GetMouse2DPosition();

						switch(m_uEditorStatus)
						{

						case MODE_STAND_BY:
							{
								// 피킹하기
								m_pPickingNode = GetPickingNode(first_click_pos);

								// 피킹된 노드가 있다면 피킹모드로 전환
								if(m_pPickingNode)
								{
									ChangeEditorMode(MODE_PICK_NODE);
									// 화면에 노드의 위치 표시
									MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
								}
							}
							break;

						case MODE_PICK_NODE:
							{
								irr::scene::ISceneNode* pPickedNode = m_pPickingNode;

								// 피킹하기
								m_pPickingNode = GetPickingNode(first_click_pos);

								// 피킹이 된 노드가 있다면
								if(m_pPickingNode)
								{
									// 이전에 피킹한 노드와 현재 피킹한 노드가 다른경우만 피킹모드 재 실시
									if(m_pPickingNode != pPickedNode)
									{
										ChangeEditorMode(MODE_PICK_NODE);
										// 화면에 노드의 위치 표시
										MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
									}
								}
								// 피킹이 된 노드가 없으면 이전에 피킹 되었던 노드가 다시 선택
								else
								{
									m_pPickingNode = pPickedNode;
								}
							}
							break;

						case MODE_MAKE_BLOCK:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_MAKE_ITEM:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_MAKE_PORTAL:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;
						}

					}
					//------------------- 첫번째 클릭 코드 종료-------------------

					//------------------- 드래그 상태 코드 시작-------------------
					else
					{
						// 드래그 중의 마우스 위치값 저장
						end_click_pos = GetMouse2DPosition();

						switch(m_uEditorStatus)
						{

						case MODE_STAND_BY:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_PICK_NODE:
							{
								// 피킹된 노드가 외곽블럭노드이면 이동하면 안된다.
								// 따라서 피킹된 노드가 외곽블럭노드인지 체크해보고 아닌 경우에만
								// 드래그를 통해 노드를 이동시킨다.
								if( !CheckBackgoundNode(m_pPickingNode) )
								{
									//---------------------------------------------------------
									// 드래그를 통하여 노드위치를 제어하는 코드 - 시작
									//---------------------------------------------------------

									// 마우스의 좌표값이 변한다면 움직이는 드래그 상태중을 의미
									// 즉 드래그 상태로 이동하는것을 나타냄.
									if(last_click_pos != end_click_pos)
									{
										//-------------------------------------------------------
										// 피킹된 신노드의 실제위치와 상대적인 위치 계산
										// 즉, 드래그를 통해 신노드의 위치를 재조정.
										// 쉬운말로 신노드 위치 이동하는 코드
										//--------------------------------------------------------

										// 신노드의 위치 이동에 필요한 벡터변수
										irr::core::vector3df current_pos, last_pos , relative_pos, result_pos;

										// 현재 마우스의 3D 위치
										GetPositionFromBackgourndNode(end_click_pos, current_pos);

										// 이전 마우스의 3D 위치
										GetPositionFromBackgourndNode(last_click_pos, last_pos);

										// 현재 마우스로 위치에서 이전의 마우스 위치 차이 계산
										relative_pos = current_pos - last_pos;

										// 최종적인 위치는 현재 신노드의 위치에 상대적인 거리만큼 움직이면 된다.
										result_pos = m_pPickingNode->getPosition() + relative_pos;
										//result_pos.Y = 0;

										
										// 만약에 블럭 노드이면
										if(m_pPickingNode->getType() == irr::scene::jz3d::CBlockNode::TypeID)
										{
											irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>(m_pPickingNode);
											irr::core::vector3df block_lastPos = pBlockNode->getStartPosition();
											irr::core::vector3df result = block_lastPos + relative_pos;
											pBlockNode->setPosition(result);
											pBlockNode->setStartPosition(result);
											pBlockNode->setStartRotation(pBlockNode->getStartRotation());
											pBlockNode->startAnimation();
										}
										else
										{
											// 계산한 최종거리를 적용.
											m_pPickingNode->setPosition(result_pos);
										}

										// 로그 메시지 출력
										MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
									}

									//---------------------------------------------------------
									// 드래그를 통하여 노드위치를 제어하는 코드 - 종료
									//---------------------------------------------------------
								}
							}
							break;

						case MODE_MAKE_BLOCK:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_MAKE_ITEM:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_MAKE_PORTAL:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;
						}


						// 마우스의 마지막 위치를 저장
						last_click_pos = end_click_pos;
					}
					//------------------- 드래그 상태 코드 종료-------------------
				}
				else
				{
					if(dragFlag)
					{// 드래그 상태

						// 드래그 상태 해제
						dragFlag = false;

						// 드래그 종료 후 마우스 위치값 저장
						end_click_pos = GetMouse2DPosition();

						switch(m_uEditorStatus)
						{// switch - start

						case MODE_STAND_BY:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_PICK_NODE:
							{
								// 사용하지 않음... 코드가 헷갈릴까봐 case문에 추가...
							}
							break;

						case MODE_MAKE_BLOCK:
							{
								// 시작과 종료 위치값
								irr::core::vector3df start, end;

								// 마우스의 첫번째 클릭지점과 마지막 드래그 지점으로 부터 3D좌표 값 얻음.
								// 3D 좌표값(시작, 종료 위치)으로 부터 블럭노드 만들기
								if(GetPositionFromBackgourndNode(first_click_pos, start) && GetPositionFromBackgourndNode(end_click_pos, end))
								{
									// 블럭노드 만들기
									irr::scene::ISceneNode* pNode = MakeBlockNode(start, end);

									// 작업히스토리에 저장하기
									WorkingHistory history;
									history.type = WorkingHistory::WORKING_MAKE_BLOCK_NODE;
									history.start = start;
									history.end = end;
									history.sceneNode = pNode;
									AddWorkingHitory(history);
								}

								// 스텐바이 모드로 전환
								ChangeEditorMode(MODE_STAND_BY);
							}
							break;

						case MODE_MAKE_ITEM:
							{
								// 종료 위치값
								irr::core::vector3df end;

								// 마지막 드래그 지점으로 부터 3D좌표 값 얻음.
								// 3D 좌표값(종료 위치)으로 부터 아이템노드 만들기
								if( GetPositionFromBackgourndNode(end_click_pos, end) )
								{
									// 아이템노드 만들기
									irr::scene::ISceneNode* pNode = MakeItemNode(end);

									// 작업히스토리에 저장하기
									WorkingHistory history;
									history.type = WorkingHistory::WORKING_MAKE_BLOCK_NODE;
									history.end = end;
									history.sceneNode = pNode;
									AddWorkingHitory(history);
								}

								// 스텐바이 모드로 전환
								ChangeEditorMode(MODE_STAND_BY);

							}
							break;

						case MODE_MAKE_PORTAL:
							{
								// 종료 위치값
								irr::core::vector3df end;

								// 마지막 드래그 지점으로 부터 3D좌표 값 얻음.
								// 3D 좌표값(종료 위치)으로 부터 포탈노드 만들기
								if( GetPositionFromBackgourndNode(last_click_pos, end) )
								{
									// 아이템노드 만들기
									irr::scene::ISceneNode* pNode = MakePortalNode(end);

									// 작업히스토리에 저장하기
									WorkingHistory history;
									history.type = WorkingHistory::WORKING_MAKE_BLOCK_NODE;
									history.end = end;
									history.sceneNode = pNode;
									AddWorkingHitory(history);
								}

								// 스텐바이 모드로 전환
								ChangeEditorMode(MODE_STAND_BY);
							}
							break;

						case MODE_MAKE_ARROW:
							{
								// 종료 위치값
								irr::core::vector3df end;

								// 마지막 드래그 지점으로 부터 3D좌표 값 얻음.
								// 3D 좌표값(종료 위치)으로 부터 포탈노드 만들기
								if( GetPositionFromBackgourndNode(last_click_pos, end) )
								{
									// 아이템노드 만들기
									irr::scene::ISceneNode* pNode = MakeAroowNode(end);
								}

								// 스텐바이 모드로 전환
								ChangeEditorMode(MODE_STAND_BY);
							}
							break;

						}// switch - end

					}// 드래그 상태 종료
				}

			}
			//-------------------------------------------------------------------------------------------
			// 마우스 포인터가 GUI위에 놓여있지 않는 경우에만 입출력 이벤트 발생 - 코드 종료
			//-------------------------------------------------------------------------------------------
		}
		break;
	}

	return true;
}

void CMapEditor::Loop()
{
	UpDate();
	Render();
}

void CMapEditor::UpDate()
{
	// IO 프로세싱
	{
		ProccessingIOEvent();
	}

	// 메시지 큐 처리
	{
		irr::u32 i;
		for( i = 0 ; i < m_messegeQueue.size() ; i++)
		{
			ProccessingMessegeQueue(m_messegeQueue[i]);
		}
		m_messegeQueue.clear();
	}
}

void CMapEditor::Render()
{
	m_pVideo->beginScene(true, true, irr::video::SColor(255,128,128,128));

	m_pSmgr->drawAll();
	m_pGuiEnv->drawAll();

	m_pVideo->endScene();
}

bool CMapEditor::OnEvent(const irr::SEvent& event)
{
	switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{
			irr::s32 id = event.GUIEvent.Caller->getID();

			// 마우스 커서가 GUI 위에 있는경우에는 에디터의 마우스클릭이벤트를 사용하지 않는다.
			// -------------------------- 마우스 커서 이벤트 제어 코드 - 시작 --------------------------
			if(event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED)
			{
				m_bGUIHoveredEvent = true;
			}
			else if(event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT)
			{
				m_bGUIHoveredEvent = false;
			}
			// -------------------------- 마우스 커서 이벤트 제어 코드 - 종료 --------------------------

			switch(event.GUIEvent.EventType)
			{
			case irr::gui::EGET_CHECKBOX_CHANGED:
				{
					
				}
				break;

			case irr::gui::EGET_COMBO_BOX_CHANGED:
				{
					switch(id)
					{
						// 블럭의 기본 텍스쳐 콤보박스 이벤트
					case GUI_BLOCK_TEXTURE_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_TEXTURE;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭타입 콤보박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_TYPE_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_TYPE;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭속성 콤보박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 회전블럭의 회전방향 콤보박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ROTATION_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 스프링블럭의 방향 콤보박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_SPRING_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 아이템타입 콤보박스 이벤트
					case GUI_PROPERTY_WINDOW_ITEM_TYPE_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_ITEM_TYPE;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;
					}
				}
				break;

			case irr::gui::EGET_SPINBOX_CHANGED:
				{
					switch (id)
					{
						// 블럭의 기본 마찰력 스핀박스 이벤트
					case GUI_BLOCK_FRICTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_FRICTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 블럭의 기본 탄성력 스핀박스 이벤트
					case GUI_BLOCK_RESTITUTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_RESTITUTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 블럭의 기본 사이즈 스핀박스 이벤트
					case GUI_BLOCK_SIZE_SPINEBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_SIZE;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭 마찰력 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_FRICTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_FRICTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭 탄성력 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_RESTITUTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_RESTITUTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭 사이즈 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_SIZE_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_SIZE;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭 속성값0 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE0_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭 속성값1 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE1_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE1;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 블럭 속성값2 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE2_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE2;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 아이템 값 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_ITEM_VALUE_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_ITEM_VALUE;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// 속성윈도우의 아이템 지속시간 스핀박스 이벤트
					case GUI_PROPERTY_WINDOW_ITEM_DURATIONTIME_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// 메시지 만들기
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_ITEM_DURATIONTIME;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;
					}
				}
				break;

			case irr::gui::EGET_FILE_SELECTED:
				{
					// 파일다이얼로그 포인터 얻어오기
					irr::gui::IGUIFileOpenDialog* dialog = static_cast<irr::gui::IGUIFileOpenDialog*>( event.GUIEvent.Caller );

					// 메시지 만들기
					Messege msg;
					msg.cData.push_back(irr::core::stringc(dialog->getFileName()));

					// 메시지 타입
					switch(id)
					{
					case GUI_LOAD_FILEDIALOG:
						{
							msg.type = Messege::MSG_LOAD_SCENE_FROM_FILENAME;
						}
						break;

					case GUI_TEXTURE_FILEDIALOG:
						{
							msg.type = Messege::MSG_SETTEXTURE_TO_BGSCENE_FROM_FILENAME;
						}
						break;
					}

					// 메시지큐에 저장
					AddMessege(msg);

					// GUI horvered 이벤트가 계속 남아 있기 때문에 강제로 플래그를 false값 대입.
					m_bGUIHoveredEvent = false;
				}
				break;

				// 파일 선택 다이얼로그를 취소할때 이벤트
			case irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				{
					// GUI horvered 이벤트가 계속 남아 있기 때문에 강제로 플래그를 false값 대입.
					m_bGUIHoveredEvent = false;			
				}
				break;

			case irr::gui::EGET_BUTTON_CLICKED:
				{
					switch(id)
					{
						// 새로 만들기 버튼 이벤트
					case GUI_NEW_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_NEW_BUTTON;
							AddMessege(msg);
						}
						break;

						// 불러오기 버튼 이벤트
					case GUI_LOAD_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_LOAD_BUTTON;
							AddMessege(msg);
						}
						break;

						// 나가기 버튼 이벤트
					case GUI_EXIT_BUTTON:
						{
							// 종료하기
							m_pDevice->closeDevice();
						}
						break;

						// 메뉴로 돌아가기 버튼 이벤트
					case  GUI_RETURN_MENU_BUTTON:
						{
							// 클리어
							Messege msg1;
							msg1.type = Messege::MSG_CLEAR;
							AddMessege(msg1);

							// 메뉴 모드로 바꾸기
							Messege msg2;
							msg2.type = Messege::MSG_CHANGE_MAIN_MODE;
							msg2.fData.push_back(MENU);
							AddMessege(msg2);
						}
						break;

						// 맵사이즈 OK 버튼 이벤트
					case GUI_MAPSIZE_OK_BUTTON:
						{
							//---------------------------
							// 가로의 사이즈 가져오기
							//---------------------------

							// 에디트 박스(가로 사이즈) 포인터 얻어오기
							irr::gui::IGUIEditBox *box1 = static_cast<irr::gui::IGUIEditBox *>( m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAPSIZE_EDITBOX_HORIZONTAL, true) );
							// 에디트 박스로부터 가로사이즈(텍스트 형식) 얻어오기
							irr::core::stringc size1_text(box1->getText());
							// 텍스트에서 숫자로, int에서 float으로 형변환 하기
							irr::f32 size1 = static_cast<irr::f32>(atoi(size1_text.c_str()));


							//---------------------------
							// 세로의 사이즈 가져오기
							//---------------------------

							// 에디트 박스(세로 사이즈) 포인터 얻어오기
							irr::gui::IGUIEditBox *box2 = static_cast<irr::gui::IGUIEditBox *>( m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAPSIZE_EDITBOX_VERTICAL, true) );
							// 에디트 박스로부터 세로사이즈(텍스트 형식) 얻어오기
							irr::core::stringc size2_text(box2->getText());
							// 텍스트에서 숫자로, int에서 float으로 형변환 하기
							irr::f32 size2 = static_cast<irr::f32>(atoi(size2_text.c_str()));

							//-----------------
							// 메시지 만들기
							//-----------------
							Messege msg;
							// 메시지 타입
							msg.type = Messege::MSG_PUSH_MAPSIZE_OK_BUTTON;
							// 메시지 데이터
							msg.fData.push_back(size1);
							msg.fData.push_back(size2);
							// 메시지큐에 저장
							AddMessege(msg);

							// 맵사이즈 윈도우 창 삭제
							event.GUIEvent.Caller->getParent()->remove();

							// GUI삭제 후에는 GUI horverd 이벤트 상태이므로 강제로 플래그를 false값 대입
							m_bGUIHoveredEvent = false;
						}
						break;

						// 맵사이즈 CANCEL 버튼 이벤트
					case GUI_MAPSIZE_CANCEL_BUTTON:
						{
							// 맵사이즈 윈도우 창 삭제
							event.GUIEvent.Caller->getParent()->remove();

							// GUI삭제 후에는 GUI horverd 이벤트 상태이므로 강제로 플래그를 false값 대입
							m_bGUIHoveredEvent = false;
						}
						break;

						// 세이브 버튼 이벤트
					case GUI_SAVE_BUTTON:
						{
							// 메시지 만들기
							Messege msg;
							// 메시지 타입
							msg.type = Messege::MSG_PUSH_SAVE_BUTTON;							
							// 메시지큐에 저장
							AddMessege(msg);
						}
						break;

						// 세이브 OK버튼 이벤트
					case GUI_SAVE_WINDOW_OK_BUTTON:
						{
							// 메시지 만들기
							Messege msg;
							//메시지 타입
							msg.type = Messege::MSG_PUSH_SAVE_WINDOW_OK_BUTTON;

							// 세이브윈도우의 에디트 박스 포인터 얻어오기
							irr::gui::IGUIEditBox *box = static_cast<irr::gui::IGUIEditBox *>( m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_SAVE_WINDOW_EDITBOX, true) );
							// 에디트 박스로부터 세이브 할 파일 이름 얻어서 저장.
							msg.cData.push_back(irr::core::stringc(box->getText()));

							// 메시지큐에 저장
							AddMessege(msg);

							// 세이브 윈도우 창 삭제
							event.GUIEvent.Caller->getParent()->remove();

							// GUI삭제 후에는 GUI horverd 이벤트 상태이므로 강제로 플래그를 false값 대입
							m_bGUIHoveredEvent = false;
						}
						break;

						// 세이브 CANCEL버튼 이벤트
					case GUI_SAVE_WINDOW_CANCEL_BUTTON:
						{
							// 세이브 윈도우 창 삭제
							event.GUIEvent.Caller->getParent()->remove();

							// GUI삭제 후에는 GUI horverd 이벤트 상태이므로 강제로 플래그를 false값 대입
							m_bGUIHoveredEvent = false;
						}
						break;

						// 블럭만들기 버튼 이벤트
					case GUI_MAKE_BLOCK_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_BLOCK_BUTTON);
							AddMessege(msg);
						}
						break;

						// 아이템만들기 버튼 이벤트
					case GUI_MAKE_ITEM_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_ITEM_BUTTON);
							AddMessege(msg);
						}
						break;

						// 포탈만들기 버튼 이벤트
					case GUI_MAKE_PORTAL_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_PORTAL_BUTTON);
							AddMessege(msg);
						}
						break;

						// 화살표만들기 버튼 이벤트
					case GUI_MAKE_ARROW_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_ARROW_BUTTON);
							AddMessege(msg);
						}
						break;

						// 복사 버튼 이벤트
					case GUI_COPY_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_COPY_BUTTON;
							AddMessege(msg);
						}
						break;

						// 삭제 버튼 이벤트
					case GUI_DELETE_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_DELETE_BUTTON;
							AddMessege(msg);
						}
						break;

					case GUI_RESTART_ALL_ANIMATOR_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_RESTAET_ALL_ANIMATOR_BUTTON;
							AddMessege(msg);
						}
						break;

						// 배경노드의 텍스쳐 선택 버튼 이벤트
					case GUI_TEXTURE_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_TEXTURE_BUTTON;
							AddMessege(msg);
						}
						break;

						// 블럭 전체 속성값 바꾸기 버튼 이벤트
					case GUI_CHANGE_ALL_BLOCK_PROPERTY_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_CHANGE_ALL_BLOCK_PROPERTY_BUTTON;
							msg.pData = event.GUIEvent.Caller;
							AddMessege(msg);
						}
						break;

						// 미세조정 (위로 이동) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (아래로 이동) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (좌로 이동) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (우로 이동) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (앞으로 이동) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (뒤로 이동) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (좌로 회전) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROROTATION_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON);
							AddMessege(msg);
						}
						break;

						// 미세조정 (우로 회전) 버튼 이벤트
					case GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROROTATION_BUTTON;
							// GUI 열거값으로 데이터 구분
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON);
							AddMessege(msg);
						}
						break;

						// 블럭노드 애니메이션 시작 버튼 이벤트
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ANIMATION_BUTTON:
						{
							Messege msg;
							// 미세움직임 메시지
							msg.type = Messege::MSG_PUSH_PROPERTY_ANIMATION_BUTTON;
							AddMessege(msg);
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

	case irr::EET_MOUSE_INPUT_EVENT:
		{
			// 마우스 왼쪽 버튼
			if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
			{
				m_Key[irr::KEY_LBUTTON] = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
			{
				m_Key[irr::KEY_LBUTTON] = false;
			}

			// 마우스 오른쪽 버튼
			if(event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
			{
				m_Key[irr::KEY_RBUTTON] = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP)
			{
				m_Key[irr::KEY_RBUTTON] = false;
			}

			if(event.MouseInput.Event == irr::EMIE_MOUSE_WHEEL)
			{
				if(event.MouseInput.Wheel == 1)
					m_Key_MouseWheel_Up = true;
				else
					m_Key_MouseWheel_Down = true;
			}
			else
			{
				m_Key_MouseWheel_Up = false;
				m_Key_MouseWheel_Down = false;
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

void CMapEditor::CopySceneNode(irr::scene::ISceneNode* pNode)
{
	irr::scene::ISceneNode* pParentNode = pNode->getParent();
	pNode->clone(pParentNode, m_pSmgr);
}