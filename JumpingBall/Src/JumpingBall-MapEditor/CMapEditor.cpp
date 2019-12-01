#include "CMapEditor.h"
#include "irrFactory.h"

#include <string>
#include <algorithm>

#include <iostream>

CMapEditor::CMapEditor(void)
: m_uBlockBasicSize(10), m_pPickingNode(NULL),
// �� �̸�
m_sMapName("stage"),
// ������ �ʿ��� ���� ���� �ʱ�ȭ - ī�޶� �̵�, ��, �ų���� �̵�
m_fCameraMoveValue(1), m_fCameraZoomValue(15), m_fSceneNodeMicroMoveValue(0.1f), m_fSceneNodeMicroRotationValue(5),
// GUI �̺�Ʈ��  ���� ó��
m_bGUIHoveredEvent(false),
// ���� ���� - ������
m_fFrictionMin(0), m_fFrictionMax(10), m_fFrictionStep(0.1f),
// ���� ���� - ź����
m_fRestitutionMin(0), m_fRestitutionMax(10), m_fRestitutionStep(0.1f),
// �� ������ ���ɹڽ� ���
m_fSizeMin(1), m_fSizeMax(100), m_fSizeStep(1),
// ������ �� ���ɹڽ� ���
m_fItemMin(-10000), m_fItemMax(10000), m_fItemStep(1),
// �� �Ӽ���
m_fValueMin(-10000), m_fValueMax(10000), m_fValueStep(10),
// ���� �⺻ ������, ź����, �ؽ���, ������
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
	//����� �߰� Ȯ�� ���丮���
	irr::scene::jz3d::CJZ3DSceneNodeFactory *factory = new irr::scene::jz3d::CJZ3DSceneNodeFactory(m_pSmgr);
	m_pSmgr->registerSceneNodeFactory(factory);
	factory->drop();

	if(!factory)
		return false;
	return true;
}

bool CMapEditor::InitGUI()
{
	// ��Ų �÷��� ����
	{
		for (u32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
		{
			irr::video::SColor col = m_pGuiEnv->getSkin()->getColor((irr::gui::EGUI_DEFAULT_COLOR)i);
			col.setAlpha(255);
			m_pGuiEnv->getSkin()->setColor((irr::gui::EGUI_DEFAULT_COLOR)i, col);
		}
	}

	// ���θ���� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(100,200,250,400),
			0,
			GUI_NEW_BUTTON,
			L"Creating a new map",
			0);

		m_pGUI_menu.push_back(button);
	}

	// �� �ε� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(300,200,450,400),
			0,
			GUI_LOAD_BUTTON,
			L"Loading a map",
			0);

		m_pGUI_menu.push_back(button);
	}

	// ���� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(500,200,650,400),
			0,
			GUI_EXIT_BUTTON,
			L"EXIT",
			0);

		m_pGUI_menu.push_back(button);
	}


	// �޴��� ���ư��� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(0,0,50,50),
			0,
			GUI_RETURN_MENU_BUTTON,
			L"RETURN TO\nMAINMENU",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// ���̺� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(50,0,100,50),
			0,
			GUI_SAVE_BUTTON,
			L"SAVE",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// �� ����� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(150,0,200,50),
			0,
			GUI_MAKE_BLOCK_BUTTON,
			L"BLOCK",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// ������ ����� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(200,0,250,50),
			0,
			GUI_MAKE_ITEM_BUTTON,
			L"ITEM",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// ��Ż ����� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(250,0,300,50),
			0,
			GUI_MAKE_PORTAL_BUTTON,
			L"PORTAL",
			0);

		m_pGUI_Editor.push_back(button);
	}

	// ȭ��ǥ ����� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(300,0,350,50),
			0,
			GUI_MAKE_ARROW_BUTTON,
			L"ARROW",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// ���� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(350,0,400,50),
			0,
			GUI_COPY_BUTTON,
			L"COPY",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// ���� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(400,0,450,50),
			0,
			GUI_DELETE_BUTTON,
			L"DELETE",
			0);

		m_pGUI_Editor.push_back(button);
	}

	// ���� ��� �ִϸ��̼� ����ŸƮ ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(450,0,500,50),
			0,
			GUI_RESTART_ALL_ANIMATOR_BUTTON,
			L"RESTART\nANIMATOR",
			0);

		m_pGUI_Editor.push_back(button);
	}



	// �ؽ��� ���� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(500,0,550,50),
			0,
			GUI_TEXTURE_BUTTON,
			L"BackGround\nTEXTURE",
			0);

		m_pGUI_Editor.push_back(button);
	}

	// �� ��ü �Ӽ��� �ٲٱ� ��ư
	{
		irr::gui::IGUIButton* button = m_pGuiEnv->addButton(
			irr::core::recti(550,0,600,50),
			0,
			GUI_CHANGE_ALL_BLOCK_PROPERTY_BUTTON,
			L"Change\nAll Block\nProperty",
			0);

		m_pGUI_Editor.push_back(button);
	}


	// �� ������ ����ƽ �ؽ�Ʈ
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

	// �� ������ ���ɹڽ� ����� ---
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

	// �� ź���� ����ƽ �ؽ�Ʈ
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

	// �� ź���� ���ɹڽ� ����� ---
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

	// �� �⺻ �ؽ��� ����ƽ �ؽ�Ʈ �����
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
	
	// �� �⺻ �ؽ��� �޺��ڽ� �����
	{
		irr::gui::IGUIComboBox *box_type = m_pGuiEnv->addComboBox(
			irr::core::rect<irr::s32>(irr::core::position2di(600,100), irr::core::dimension2du(100,20)),
			0,
			GUI_BLOCK_TEXTURE_COMBOBOX
			);

		// �� �⺻ �ؽ��� �޺��ڽ� ������ �߰�
		irr::u16 i;
		for(i = 0 ; i < irr::scene::jz3d::BLOCK_TOTAL_COUNT ; ++i)
		{
			box_type->addItem(irr::scene::jz3d::CBlockNode::typeNames[i], i);
		}

		box_type->setSelected(m_uBasicBlockTexture);
		m_pGUI_Editor.push_back(box_type);
	}

	// �� ������ ����ƽ �ؽ�Ʈ
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

	// �� ������ ���ɹڽ� ����� ---
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
	

	// �α׸޽��� �ڽ�
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
	// �ʻ����� ������ �����
	irr::gui::IGUIWindow *window = m_pGuiEnv->addWindow(
		irr::core::rect<irr::s32>(300,250,500,350),
		true,
		L"Map size",
		0,
		GUI_MAPSIZE_WINDOW
		);
	// ������ �ݱ��ư ��Ȱ��ȭ �� ���߱�, �巡�� ����
	window->getCloseButton()->setEnabled(false);
	window->getCloseButton()->setVisible(false);
	window->setDraggable(false);

	// �ʻ����� - ���λ����� �Է� ����Ʈ �ڽ� �����
	irr::gui::IGUIEditBox *box1 = m_pGuiEnv->addEditBox(
		L"400",
		irr::core::rect<irr::s32>(10,25,90,55),
		true,
		window,
		GUI_MAPSIZE_EDITBOX_HORIZONTAL
		);

	// �ʻ����� - ���λ����� �Է� ����Ʈ �ڽ� �����
	irr::gui::IGUIEditBox *box2 = m_pGuiEnv->addEditBox(
		L"300",
		irr::core::rect<irr::s32>(110,25,190,55),
		true,
		window,
		GUI_MAPSIZE_EDITBOX_VERTICAL
		);

	// �ʻ����� - OK��ư �����
	irr::gui::IGUIButton* button1 = m_pGuiEnv->addButton(
		irr::core::recti(10,60,90,90),
		window,
		GUI_MAPSIZE_OK_BUTTON,
		L"O      K",
		0);

	// �ʻ����� - CANCEL��ư �����
	irr::gui::IGUIButton* button2 = m_pGuiEnv->addButton(
		irr::core::recti(110,60,190,90),
		window,
		GUI_MAPSIZE_CANCEL_BUTTON,
		L"CANCEL",
		0);
}

void CMapEditor::CreateSaveWindow()
{
	// ���̺� ������ �����
	irr::gui::IGUIWindow *window = m_pGuiEnv->addWindow(
		irr::core::rect<irr::s32>(300,250,500,350),
		true,
		L"SAVE",
		0,
		GUI_SAVE_WINDOW
		);
	// ������ �ݱ��ư ��Ȱ��ȭ �� ���߱�, �巡�� ����
	window->getCloseButton()->setEnabled(false);
	window->getCloseButton()->setVisible(false);
	window->setDraggable(false);

	// ���̺� �����̸� ����Ʈ�ڽ� �����
	irr::gui::IGUIEditBox *box1 = m_pGuiEnv->addEditBox(
		m_sMapName.c_str(),
		irr::core::rect<irr::s32>(10,25,190,55),
		true,
		window,
		GUI_SAVE_WINDOW_EDITBOX
		);

	// ���̺� OK��ư �����
	irr::gui::IGUIButton* button1 = m_pGuiEnv->addButton(
		irr::core::recti(10,60,90,90),
		window,
		GUI_SAVE_WINDOW_OK_BUTTON,
		L"O      K",
		0);

	// ���̺� CANCEL��ư �����
	irr::gui::IGUIButton* button2 = m_pGuiEnv->addButton(
		irr::core::recti(110,60,190,90),
		window,
		GUI_SAVE_WINDOW_CANCEL_BUTTON,
		L"CANCEL",
		0);
}

void CMapEditor::CreateLoadFileDialog()
{
	// ���Ͽ��´��̾�α� �����
	irr::gui::IGUIFileOpenDialog* dialog = m_pGuiEnv->addFileOpenDialog(
		L"Select a  map-name.irr", 
		true, 
		0, 
		GUI_LOAD_FILEDIALOG
		);
}

void CMapEditor::CreateBGSceneTextureFileDialog()
{
	// ���ų���� �ؽ��ĸ� ������ ���̾�α� �����
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
	// GUI �Ӽ������츦 ���� �ʿ��� ���� ���� �ڵ� - ����
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
	// GUI �Ӽ������츦 ���� �ʿ��� ���� ���� �ڵ� - ����
	//------------------------------------------------------------



	//--------------------------------------------------------------------
	// ����� Ÿ�Կ� ���� ������ �������� �Ҵ��ϴ� �ڵ� - ����
	//--------------------------------------------------------------------
	switch(pNode->getType())
	{
		// �����
	case irr::scene::jz3d::CBlockNode::TypeID:
		{
			// �������̸�
			windowName = L"Block property";
			// ������ ������
			windowSize = irr::core::recti(700,0,800,700);
			// ȸ����Ʈ�� ��ư ����
			blockPropertyFlag = true;
			// �ܰ� ���̸� �̵�,ȸ�� ��Ʈ�� �Ұ���.
			if( CheckBackgoundNode(pNode) )
			{
				moveControlFlag = rotationControlFlag = false;
			}
			else
			{
				moveControlFlag = rotationControlFlag = true;
			}
			// ����ȯ ���� ������ ������
			blockNode = static_cast<irr::scene::jz3d::CBlockNode*>(pNode);
			// ������ �Ӽ� �÷���
			itemPropertyFlag = false;
		}
		break;

		// �ٿ���ڽ� ��� - �ڽ����� �ٽ� ����
	case irr::scene::jz3d::CEmptyBBoxNode::TypeID:
		{
			switch( static_cast<irr::scene::jz3d::CEmptyBBoxNode*>(pNode)->getFirstChild()->getType() )
			{
				// ������ ���
			case irr::scene::jz3d::CItemNode::TypeID:
				{
					// �������̸�
					windowName = L"Item property";
					// ������ ������
					windowSize = irr::core::recti(700,0,800,500);
					// �̵���Ʈ�� ����
					moveControlFlag = true;
					// ȸ����Ʈ�� ��ư �Ұ���
					rotationControlFlag = blockPropertyFlag = false;
					// ������ �Ӽ� �÷���
					itemPropertyFlag = true;
				}
				break;

				// ��ġ��� - ����, ���� ȭ��ǥ ���
			case irr::scene::jz3d::CPositionNode::TypeID:
				{
					// �������̸�
					windowName = L"Position property";
					// ������ ������
					windowSize = irr::core::recti(700,0,800,220);
					// �̵���Ʈ�� ����
					moveControlFlag = true;
					// ȸ����Ʈ�� ��ư �Ұ���
					rotationControlFlag = true;
					// �� �Ӽ� �÷���
					blockPropertyFlag = false;
					// ������ �Ӽ� �÷���
					itemPropertyFlag = false;
				}
				break;
			}
		}
		break;

	case irr::scene::jz3d::CPortalNode::TypeID:
		{
			// �������̸�
			windowName = L"Portal property";
			// ������ ������
			windowSize = irr::core::recti(700,0,800,150);
			// �̵���Ʈ�� ����
			moveControlFlag = true;
			// ȸ����Ʈ�� ��ư �Ұ���
			rotationControlFlag = blockPropertyFlag = false;
			// ������ �Ӽ� �÷���
			itemPropertyFlag = false;
		}
		break;
	}
	//--------------------------------------------------------------------
	// ����� Ÿ�Կ� ���� ������ �������� �Ҵ��ϴ� �ڵ� - ����
	//--------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	// ������ �Ҵ��� ������ ������ �����Ͽ� ����GUI(������, �̵���ư)�� �����ϴ� �ڵ� - ����
	//----------------------------------------------------------------------------------------------------
	{
		// --- �Ӽ�â ����� ---
		window = m_pGuiEnv->addWindow(
			windowSize,
			false,
			windowName.c_str(),
			0,
			GUI_PROPERTY_WINDOW
			);
		// ������ �ݱ��ư ��Ȱ��ȭ �� ���߱�, �巡�� ����
		window->getCloseButton()->setEnabled(false);
		window->getCloseButton()->setVisible(false);
		window->setDraggable(false);

		// --- ����ƽ�ؽ�Ʈ(�̼� ������) ����� ---
		staticText[0] = m_pGuiEnv->addStaticText(
			L"Micro-moving Control",
			irr::core::rect<irr::s32>(irr::core::position2di(5,20), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- �̼�����(���� �̵�) ��ư ����� ---
		button[0] = m_pGuiEnv->addButton(
			irr::core::recti(35,40,65,70),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON,
			L"UP",
			0);
		button[0]->setEnabled(moveControlFlag);

		// --- �̼�����(�Ʒ��� �̵�) ��ư ����� ---
		button[1] = m_pGuiEnv->addButton(
			irr::core::recti(35,70,65,100),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON,
			L"DN",
			0);
		button[1]->setEnabled(moveControlFlag);

		// --- �̼�����(�������� �̵�) ��ư ����� ---
		button[2] = m_pGuiEnv->addButton(
			irr::core::recti(5,70,35,100),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON,
			L"LT",
			0);
		button[2]->setEnabled(moveControlFlag);

		// --- �̼�����(���������� �̵�) ��ư ����� ---
		button[3] = m_pGuiEnv->addButton(
			irr::core::recti(65,70,95,100),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON,
			L"RT",
			0);
		button[3]->setEnabled(moveControlFlag);

		// --- �̼�����(������ �̵�) ��ư ����� ---
		button[4] = m_pGuiEnv->addButton(
			irr::core::recti(5,100,35,130),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON,
			L"FRONT",
			0);
		button[4]->setEnabled(moveControlFlag);

		// --- �̼�����(�ڷ� �̵�) ��ư ����� ---
		button[5] = m_pGuiEnv->addButton(
			irr::core::recti(65,100,95,130),
			window,
			GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON,
			L"BACK",
			0);
		button[5]->setEnabled(moveControlFlag);


		// --- ȸ���� �ʿ��� ��� ȸ����ư ����� ---
		if(rotationControlFlag)
		{
			// --- ����ƽ�ؽ�Ʈ(�̼� ȸ��) ����� ---
			staticText[1] = m_pGuiEnv->addStaticText(
				L"Rotating Control",
				irr::core::rect<irr::s32>(irr::core::position2di(5,150), irr::core::dimension2du(90,20)),
				false, true,
				window,
				0,
				false
				);

			// --- �̼�����(�·� ȸ��) ��ư ����� ---
			button[6] = m_pGuiEnv->addButton(
				irr::core::recti(5,170,50,200),
				window,
				GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON,
				L"LT",
				0);
			button[6]->setEnabled(rotationControlFlag);

			// --- �̼�����(��� ȸ��) ��ư ����� ---
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
	// ������ �Ҵ��� ������ ������ �����Ͽ� ����GUI(������, �̵���ư)�� �����ϴ� �ڵ� - ����
	//----------------------------------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// �ų�� Ÿ�Կ� ���� ���� �ʿ��� �Ӽ�GUI�� �����ϴ� �ڵ� - ����
	//-------------------------------------------------------------------------
	// ����� �Ӽ� GUI ����
	if(blockPropertyFlag)
	{
		// --- �� Ÿ�� ����ƽ �ؽ�Ʈ ����� ---
		irr::gui::IGUIStaticText* staticText_type = m_pGuiEnv->addStaticText(
			L"Block Texture",
			irr::core::rect<irr::s32>(irr::core::position2di(5,220), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- �� Ÿ�� �޺��ڽ� ����� ---
		irr::gui::IGUIComboBox *box_type = m_pGuiEnv->addComboBox(
			irr::core::rect<irr::s32>(5,240,90,260),
			window,
			GUI_PROPERTY_WINDOW_BLOCK_TYPE_COMBOBOX
			);

		// --- �� Ÿ�� �޺��ڽ� ������ �߰� ---
		{
			irr::u16 i;
			for(i = 0 ; i < irr::scene::jz3d::BLOCK_TOTAL_COUNT ; ++i)
			{
				box_type->addItem(irr::scene::jz3d::CBlockNode::typeNames[i], i);
			}

			box_type->setSelected(blockNode->getBlockType());
		}

		// --- �� ������ ����ƽ �ؽ�Ʈ ����� ---
		irr::gui::IGUIStaticText* staticText_friction = m_pGuiEnv->addStaticText(
			L"Block Friction",
			irr::core::rect<irr::s32>(irr::core::position2di(5,280), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- �� ������ ���ɹڽ� ����� ---
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

		// --- �� ź���� ����ƽ �ؽ�Ʈ ����� ---
		irr::gui::IGUIStaticText* staticText_restitution = m_pGuiEnv->addStaticText(
			L"Block Restitution",
			irr::core::rect<irr::s32>(irr::core::position2di(5,340), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- �� ź���� ���ɹڽ� ����� ---
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

		// --- �� ������ ����ƽ �ؽ�Ʈ ����� ---
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
		
		// --- �� ������ ���ɹڽ� ����� ---
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

		// --- �� �Ӽ� ����ƽ �ؽ�Ʈ ����� ---
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

		// --- �� �Ӽ� �޺��ڽ� ����� ---
		{
			irr::gui::IGUIComboBox *box_property = m_pGuiEnv->addComboBox(
				irr::core::rect<irr::s32>(5,480,90,500),
				window,
				GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_COMBOBOX
				);

			box_property->setVisible(moveControlFlag);
			// --- �� Ÿ�� �޺��ڽ� ������ �߰� ---
			{
				irr::u16 i;
				for(i = 0 ; i < irr::scene::jz3d::BLOCK_PROPERTY_TOTAL_COUNT ; ++i)
				{
					box_property->addItem(irr::scene::jz3d::CBlockNode::propertyNames[i], i);
				}

				box_property->setSelected(blockNode->getBlockProperty());
			}
			
			// --- �� Ÿ�� �Ӽ������� ����� ---
			{
				CreateBlockPropertyWindow(pNode, blockNode->getBlockProperty());
			}
		}
	}

	// �����۳�� �Ӽ� GUI ����
	if(itemPropertyFlag)
	{
		// �����۳��� ����ȯ
		irr::scene::jz3d::CItemNode* itemNode = static_cast<irr::scene::jz3d::CItemNode*>( static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( pNode )->getFirstChild() );

		// --- �� Ÿ�� ����ƽ �ؽ�Ʈ ����� ---
		irr::gui::IGUIStaticText* staticText_type = m_pGuiEnv->addStaticText(
			L"Item Type",
			irr::core::rect<irr::s32>(irr::core::position2di(5,220), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- ������ Ÿ�� �޺��ڽ� ����� ---
		irr::gui::IGUIComboBox *box_type = m_pGuiEnv->addComboBox(
			irr::core::rect<irr::s32>(5,240,90,260),
			window,
			GUI_PROPERTY_WINDOW_ITEM_TYPE_COMBOBOX
			);

		// --- ������ Ÿ�� �޺��ڽ� ������ �߰� ---
		{
			irr::u16 i;
			for(i = 0 ; i < irr::scene::jz3d::ITEM_TOTAL_COUNT ; ++i)
			{
				box_type->addItem(irr::scene::jz3d::CItemNode::typeNames[i], i);
			}
			box_type->setSelected( itemNode->getItemType() );
		}

		// --- ������ �� ����ƽ �ؽ�Ʈ ����� ---
		irr::gui::IGUIStaticText* staticText_friction = m_pGuiEnv->addStaticText(
			L"Item Value",
			irr::core::rect<irr::s32>(irr::core::position2di(5,280), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- ������ �� ���ɹڽ� ����� ---
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

		// --- ������ ���ӽð� ����ƽ �ؽ�Ʈ ����� ---
		irr::gui::IGUIStaticText* staticText_restitution = m_pGuiEnv->addStaticText(
			L"Item DurationTime",
			irr::core::rect<irr::s32>(irr::core::position2di(5,340), irr::core::dimension2du(90,20)),
			false, true,
			window,
			0,
			false
			);

		// --- ������ ���ӽð� ���ɹڽ� ����� ---
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
	// �ų�� Ÿ�Կ� ���� ���� �ʿ��� �Ӽ�GUI�� �����ϴ� �ڵ� - ����
	//-------------------------------------------------------------------------
}

void CMapEditor::CreateBlockPropertyWindow(irr::scene::ISceneNode* pNode, irr::u32 block_property_type)
{
	// ���ڰ����� ������ pNode�� ���� ���� ���̹Ƿ� ���ų��� ����ȯ �Ѵ�.
	irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>(pNode);

	// �θ� �� �Ӽ������츦 �����´�
	irr::gui::IGUIElement* propertywindow = m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_PROPERTY_WINDOW);
	irr::gui::IGUIElement* blockPropertyWindow = m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_WINDOW, true);
	if(blockPropertyWindow)
		blockPropertyWindow->remove();

	if(propertywindow)
	{
		// �Ӽ������쿡 ���Ӽ������츦 �߰��Ѵ�.
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
				// --- �ı���  �浹Ƚ�� ����ƽ �ؽ�Ʈ ����� ---
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

				// --- �ı��� �浹Ƚ�� ���ɹڽ� �����
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
				// --- �̵���  X��ǥ ����ƽ �ؽ�Ʈ ����� ---
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

				// --- �̵��� X��ǥ ���ɹڽ� �����
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

				// --- �̵���  Y��ǥ ����ƽ �ؽ�Ʈ ����� ---
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

				// --- �̵��� Y��ǥ ���ɹڽ� �����
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

				// --- �̵���  �̵��ӵ� ����ƽ �ؽ�Ʈ ����� ---
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

				// --- �̵��� �̵��ӵ� ���ɹڽ� �����
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

				// �ִϸ��̼� ���� ��ư �����
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
				// --- ȸ����  ȸ������ ����ƽ �ؽ�Ʈ ����� ---
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

				// --- ȸ����  ȸ������ �޺��ڽ� ����� ---
				{
					irr::gui::IGUIComboBox *box = m_pGuiEnv->addComboBox(
						irr::core::rect<irr::s32>(5,40,90,60),
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ROTATION_COMBOBOX
						);

					// --- ȸ���� ȸ������ �޺��ڽ� ������ �߰� ---

					irr::u16 i;
					for(i = 0 ; i < irr::scene::jz3d::BLOCK_PROPERTY_ROTATING_DIRECTION_TOTAL_COUNT ; ++i)
					{
						box->addItem(irr::scene::jz3d::CBlockNode::rotationBlockDirectionNames[i], i);
					}
					box->setSelected(static_cast<irr::u32>(pBlockNode->getPropertyValue(0)));
				}

				// --- ȸ����  ȸ���ӵ� ����ƽ �ؽ�Ʈ ����� ---
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
				
				// --- ȸ���� ȸ���ӵ� ���ɹڽ� �����
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

				// �ִϸ��̼� ���� ��ư �����
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
				// --- ��������  ������ź�¹��� ����ƽ �ؽ�Ʈ ����� ---
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

				// --- ��������  ������ź�¹��� �޺��ڽ� ����� ---
				{
					irr::gui::IGUIComboBox *box = m_pGuiEnv->addComboBox(
						irr::core::rect<irr::s32>(5,40,90,60),
						window,
						GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_SPRING_COMBOBOX
						);

					// --- �������� ������ź�¹��� �޺��ڽ� ������ �߰� ---
					irr::u16 i;
					for(i = 0 ; i < irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_TOTAL_COUNT ; ++i)
					{
						box->addItem(irr::scene::jz3d::CBlockNode::springBlockDirectionNames[i], i);
					}
					box->setSelected(static_cast<irr::u32>(pBlockNode->getPropertyValue(0)));
				}

				// --- ��������  ������ź�� ����ƽ �ؽ�Ʈ ����� ---
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

				// --- �������� ������ź�� ���ɹڽ� �����
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
	// �Ÿ޴��� Ŭ����
	{
		m_pSmgr->clear();
	}

	// Ŭ���� ���� �÷���
	{
		m_bIsClear = true;
	}
}

void CMapEditor::Reset()
{
	// (1) Ŭ���� �ǽ�
	if(!m_bIsClear)
		Clear();

	// (2) �ű׷��� �����
	CreateSceneGraph();

	// (3) ��� �����(���÷���) 
	CreateHillPlane();

	// (4) ī�޶� �����
	CreateCamera();

	// Ŭ���� ���� �÷���
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
	// ��� ���� ��ȯ
	m_uEditorStatus = mode;

	// �Ӽ������찡 ��������ִٸ� �Ӽ������츦 �����Ѵ�.
	{
		// �Ӽ������� ������ ��������
		irr::gui::IGUIElement* propertywindow = m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_PROPERTY_WINDOW);

		// NULL�����Ͱ� �ƴϸ�, �� �Ӽ������찡 �ִٸ� �����Ѵ�.
		if(propertywindow)
			propertywindow->remove();
	}

	switch(mode)
	{
	case MODE_STAND_BY:
		{
			// ��ŷ�� �ų�� ����
			m_pPickingNode = NULL;

			// ��� �̿밡���� ���·� ��ȯ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(true);

			// ������ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// �����ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// �α� �޽��� ���
			MessegeLogToStaticText(L"");
		}
		break;

	case MODE_MAKE_BLOCK:
		{
			// ������� ��ư ��Ȱ�� ����
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(false);

			// ������ Ȱ������
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(true);

			// ������ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// �����ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// �α� �޽��� ���
			MessegeLogToStaticText(L"Creating a Block Node : Drag a mouse if you want to create..!!");
		}
		break;

	case MODE_MAKE_ITEM:
		{
			// �����۸���� ��ư ��Ȱ�� ����
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(false);

			// ������ Ȱ������
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(true);

			// ������ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// �����ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// �α� �޽��� ���
			MessegeLogToStaticText(L"Creating a Item Node : Click the mouse if you want to create..!!");
		}
		break;

	case MODE_MAKE_PORTAL:
		{
			// ��Ż����� ��ư ��Ȱ�� ����
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_PORTAL_BUTTON)->setEnabled(false);

			// ������ Ȱ������
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_ITEM_BUTTON)->setEnabled(true);
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAKE_BLOCK_BUTTON)->setEnabled(true);

			// ������ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);

			// �����ư ��Ȱ��ȭ
			m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);

			// �α� �޽��� ���
			MessegeLogToStaticText(L"Creating Portal Node set : Click the mouse if you want to create..!!");
		}
		break;

	case MODE_PICK_NODE:
		{
			// ��ŷ ��� ����
			m_uEditorStatus = MODE_PICK_NODE;

			// �Ӽ������츦 ����.
			CreatePropertyWindow(m_pPickingNode);

			// ��ŷ�� ��尡 �����(����, ����, �ܰ� �����)�̸� ������ư�� ��Ȱ��ȭ
			// ����尡 �ƴϸ� ������ư�� Ȱ��ȭ ��Ŵ
			if(	CheckBackgoundNode(m_pPickingNode) || CheckPointNode(m_pPickingNode) )
			{
				// ������ư ��Ȱ��ȭ
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(false);
				// �����ư ��Ȱ��ȭ
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(false);
			}
			else
			{
				// ������ư ��Ȱ��ȭ
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_DELETE_BUTTON)->setEnabled(true);
				// �����ư ��Ȱ��ȭ
				m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_COPY_BUTTON)->setEnabled(true);
			}
		}
		break;

	}
}

bool CMapEditor::CheckBackgoundNode(irr::scene::ISceneNode* pNode)
{
	// �ܰ� ������ΰ� üũ
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
	// ����-���� ������ ��Ÿ���� ȭ��ǥ ����ΰ� üũ
	if(pNode == m_pStartNode || pNode == m_pEndNode)
		return true;
	return false;
}

bool CMapEditor::SaveScene(irr::core::stringc file_name)
{
	// ī�޶� ����
	irr::core::vector3df cam_pos;
	irr::core::vector3df cam_target;
	irr::core::vector3df cam_upVector;

	// ī�޶� �����
	// �ʿ� ���� ������ �����ϹǷ� ī�޶�� ���� ������ �ʿ䰡 ����!!!
	{
		m_pSmgr->setActiveCamera(NULL);

		cam_pos = m_pCameraNode->getAbsolutePosition();
		cam_target = m_pCameraNode->getTarget();
		cam_upVector = m_pCameraNode->getUpVector();

		m_pCameraNode->remove();
		m_pCameraNode = NULL;
	}

	// ����� �;������������� �Ǿ� �ִٸ� ���̾������� ����
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

	// ������ġ ��� visible ����
	m_pStartNode->setVisible(false);
	
	// ��Ʈ�ų���� �̸� ����
	m_pMainSceneNode->setName(file_name.c_str());

	// ���� �̸� ����
	m_sMapName = file_name;

	// irr Ȯ���� �߰�
	file_name.append(".irr");
	//�� �����ϱ�
	bool successFlag = m_pSmgr->saveScene(file_name.c_str());

	//���̾������� �����·� ������
	if(wireframe)
		m_pBackGroundSceneNode->getMaterial(0).Wireframe = true;

	// ������ġ��� �����·� ������
	m_pStartNode->setVisible(true);

	// ����� �����·� ������
	m_pBackGroundSceneNode->setVisible(true);

	// ī�޶���� �ٽ� �����ϱ�
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
	// ���� �ε��ϱ� ���� ���� Ŭ���� �ǽ�
	if(!m_bIsClear)	
		Clear();

	// ���Ŵ����� ���� �� �ε� �ϱ�
	if(!m_pSmgr->loadScene(file_name.c_str()))
		return false;

	// ���ξ���� ������ �����ϱ�
	irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = m_pSmgr->getRootSceneNode()->getChildren().begin();
	
	m_pMainSceneNode = static_cast<irr::scene::ISceneNode*>( (*it) );

	// ��ŷ�� Ʈ���̾ޱ� �����͸� ���� ������ ���� �� Ʈ���̾ޱ� ������ ���
	{
		// "HillPlane"���� �Ǿ� �ִ� �ų�� �˻�
		irr::scene::ISceneNode* pNode = m_pSmgr->getSceneNodeFromName("HillPlane", m_pMainSceneNode);
		// ������ ����
		m_pBackGroundSceneNode = static_cast<irr::scene::IMeshSceneNode *>(pNode);

		// Ʈ���̾ޱ� ������ ���
		irr::scene::IMeshSceneNode *pMeshNode = m_pBackGroundSceneNode;
		irr::scene::ITriangleSelector *pSelector = m_pSmgr->createTriangleSelector(pMeshNode->getMesh(), pMeshNode);
		pMeshNode->setTriangleSelector(pSelector);
		pSelector->drop();
	}

	// ��ŷ�� �����ÿ� �ʿ��� ��������� �ٽ� ����
	{
		irr::scene::ISceneNode* pNode1,* pNode2;
		pNode1 = m_pSmgr->getSceneNodeFromName("BackGround", m_pMainSceneNode);
		pNode2 = m_pSmgr->getSceneNodeFromName("Point", m_pMainSceneNode);

		// �ܰ� �� ���
		m_pBackGroundBlockNode[0] = m_pSmgr->getSceneNodeFromName("Up", pNode1);
		m_pBackGroundBlockNode[1] = m_pSmgr->getSceneNodeFromName("Down", pNode1);
		m_pBackGroundBlockNode[2] = m_pSmgr->getSceneNodeFromName("Left", pNode1);
		m_pBackGroundBlockNode[3] = m_pSmgr->getSceneNodeFromName("Right", pNode1);

		// �������� ���
		m_pStartNode = m_pSmgr->getSceneNodeFromName("Start", pNode2);
		m_pStartNode->setVisible(true);
		// �������� ���
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

	// �⺻ ������ ����
	pNode->m_fFriction = m_fBasicFrictionValue;
	// �⺻ ź���� ����
	pNode->m_fRestitution = m_fBasicRestitutionValue;
	// �⺻ �ؽ��� ����
	pNode->setType(m_uBasicBlockTexture);
	// �⺻ ������ ����
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

	// ȭ��ǥ��带 ��ŷ�� �ٿ���ڽ� ��� ����
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

	// ��Ż�� 2���� ��Ʈ�� �̷���� �����Ƿ� �� ��Ʈ�� ������ ���带 ���� ����� 2���� �Ա�, �ⱸ��Ż�� �ڽ����� ���δ�.
	irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode();

	// �ڽ����� �ٿ��� ��Ż����� �̸��� ��ġ ����
	irr::core::stringc name("PortalSet/");
	name += node->getChildren().getSize() + 1;
	pNode->setName(name.c_str());
	irr::core::vector3df newPos = position;
	newPos.Y = 0;
	pNode->setPosition(newPos);

	//�θ� ����
	node->addChild(pNode);

	// �Ա���Ż ����
	{
		// ���μ��� ������(10, 10), �θ� pNode�� �ڽ�����, �θ�κ��� x������ -20��ŭ ���������� ����
		irr::scene::jz3d::CPortalNode* pPortalNode = irr::scene::jz3d::CPortalNode::CreatePortalNode(
			pNode, m_pSmgr, -1,
			irr::core::dimension2df(30, 30),
			irr::core::vector3df(-20, 0, 0),
			irr::scene::jz3d::PORTAL_IN
			);
	}

	// �ⱸ��Ż ����
	{
		// ���μ��� ������(10, 10), �θ� pNode�� �ڽ�����, �θ�κ��� x������ -20��ŭ ���������� ����
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

	// ���� ���
	m_pMainSceneNode = m_pSmgr->addEmptySceneNode(m_pRootNode);

	// �������� ���
	{
		irr::scene::ISceneNode* pStageNode = m_pSmgr->addEmptySceneNode(m_pMainSceneNode);
		pStageNode->setName("Stage");

		// ������
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Item");
		}

		// ��
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Block");
		}

		// ��Ż
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Portal");
		}

		// ȭ��ǥ
		{
			irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(pStageNode);
			pNode->setName("Arrow");
		}
	}

	// Point ���
	{
		irr::scene::ISceneNode* pPointNode = m_pSmgr->addEmptySceneNode(m_pMainSceneNode);
		pPointNode->setName("Point");
	}

	// ���
	{
		irr::scene::ISceneNode* pNode = m_pSmgr->addEmptySceneNode(m_pMainSceneNode);
		pNode->setName("BackGround");
	}

	return true;
}

bool CMapEditor::CreateHillPlane()
{
	// ��׶��� �ų�� ��������
	irr::scene::ISceneNode* node = m_pSmgr->getSceneNodeFromName("BackGround", m_pMainSceneNode);

	// ��׶��� �ų�带 �θ�� �ϴ� ���÷��� ��� �����
	// ���ڰ� : �θ���, �ŸŴ���, ID, ���÷��γ���� ��ġ, �⺻������, �ʻ�����
	irr::scene::jz3d::CHillPlaneNode* pNode = irr::scene::jz3d::CHillPlaneNode::CreateHillPlaneNode(
		node, m_pSmgr, -1, 
		irr::core::vector3df( 0, m_uBlockBasicSize * -1.f , 0 ),
		irr::core::dimension2df( static_cast<irr::f32>(m_uMapSize.Width), static_cast<irr::f32>(m_uMapSize.Height) )
		);

	// ��������. ����Ʈ�װ� ���̾�������
	pNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	pNode->setMaterialFlag(irr::video::EMF_WIREFRAME, true);

	// �̸� ����
	pNode->setName("HillPlane");

	// ��ŷ�� Ʈ���̾ޱ� �����͸� ���� ������ ���� �� Ʈ���̾ޱ� ������ ���
	{
		// ������ ����
		m_pBackGroundSceneNode = pNode;

		// Ʈ���̾ޱ� ������ ���
		irr::scene::IMeshSceneNode *pMeshNode = static_cast<irr::scene::IMeshSceneNode *>(pNode);
		irr::scene::ITriangleSelector *pSelector = m_pSmgr->createTriangleSelector(pMeshNode->getMesh(), pMeshNode);
		pMeshNode->setTriangleSelector(pSelector);
		pSelector->drop();
	}


	// ���ȭ�鿡 �������
	{
		// �����
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("BackGround", m_pMainSceneNode);

		// ���κ� ��
		irr::scene::jz3d::CBlockNode* pNode0 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/2.f),
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/2.f),
			0
			);

		pNode0->setName("Up");
		m_pBackGroundBlockNode[0] = pNode0;

		// �Ʒ��κ� ��
		irr::scene::jz3d::CBlockNode* pNode1 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/-2.f),
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/-2.f),
			0
			);
		pNode1->setName("Down");
		m_pBackGroundBlockNode[1] = pNode1;

		// ���ʺκ� ��
		irr::scene::jz3d::CBlockNode* pNode2 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/2.f),
			irr::core::vector3df(m_uMapSize.Width/-2.f, 0, m_uMapSize.Height/-2.f),
			0
			);
		pNode2->setName("Left");
		m_pBackGroundBlockNode[2] = pNode2;

		// �����ʺκ� ��
		irr::scene::jz3d::CBlockNode* pNode3 = irr::scene::jz3d::CBlockNode::CreateBlockNode(
			pNode, m_pSmgr, -1,
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/2.f),
			irr::core::vector3df(m_uMapSize.Width/2.f, 0, m_uMapSize.Height/-2.f),
			0
			);
		pNode3->setName("Right");
		m_pBackGroundBlockNode[3] = pNode3;
	}


	// ������ġ�� ������ġ �����ϱ�
	{
		// ����Ʈ���
		irr::scene::ISceneNode *pNode = m_pSmgr->getSceneNodeFromName("Point", m_pMainSceneNode);

		// ������ġ���
		{
			// ������ġ ���
			irr::core::vector3df position( m_uMapSize.Width/-2.f + (irr::f32) m_uBlockBasicSize * 3, 0, m_uMapSize.Height/2.f - (irr::f32) m_uBlockBasicSize * 3 );

			// ��ġ��带 ��ŷ�� �ٿ���ڽ� ��� ����, ������ ����� ��ġ�� ����
			irr::scene::jz3d::CEmptyBBoxNode *pStartNode = irr::scene::jz3d::CEmptyBBoxNode::CreateEmptyBBoxNode(
				pNode, m_pSmgr, -1,
				position,
				irr::core::vector3df(20)
				);

			// ��ġ��带 �����ϰ� �ٿ���ڽ������ �ڽ����� ���̴�.
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

		// ������ġ���
		{
			// ������ġ ���
			irr::core::vector3df position( m_uMapSize.Width/2.f - (irr::f32) m_uBlockBasicSize * 3, 0, m_uMapSize.Height/-2.f + (irr::f32) m_uBlockBasicSize * 3 );

			//// ��ġ��带 ��ŷ�� �ٿ���ڽ� ��� ����, ������ ����� ��ġ�� ����
			//irr::scene::jz3d::CEmptyBBoxNode *pEndNode = irr::scene::jz3d::CEmptyBBoxNode::CreateEmptyBBoxNode(
			//	pNode, m_pSmgr, -1,
			//	position,
			//	irr::core::vector3df(15)
			//	);

			// ��ġ��带 �����ϰ� �ٿ���ڽ������ �ڽ����� ���̴�.
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
	// ���콺��ǥ�� ī�޶�κ��� ��ŷ���� ������
	irr::core::line3df Ray = m_pSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates(position_mouse, m_pCameraNode);

	// �浹��ǥ�� ����Ҷ� �ʿ��� ������.
	irr::core::triangle3df tri;
	const irr::scene::ISceneNode* pNode;

	// ��ŷ�������� �浹��ǥ ������. �� ���÷��θ޽��� �浹������ �浹 ��ǥ�� ������
	// �浹�Ͽ� ��ǥ���� �ִٸ� true�� ��ȯ.
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

	// ���콺��ǥ�� ���� ��ŷ���� �����
	Ray = m_pSmgr->getSceneCollisionManager()->getRayFromScreenCoordinates(position_mouse);

	// ��ŷ���̷� ��ŷ�� �ų�� ���
	irr::scene::ISceneNode* pNode = m_pSmgr->getSceneCollisionManager()->getSceneNodeFromRayBB(Ray);

	// ��ŷ�� �ų�尡 ���, ī�޶�, �����̸� NULL������ ��ȯ. �� ��ŷ���� �ʾҴٴ� �ǹ�.
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
			// ���θ޴��� ��ȯ
			case MENU:
				{
					Clear();
					ChangeMode(MENU);
				}
				break;

			// �����ͷ� ��ȯ
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
			// �ʻ����� ������ �����
			CreateMapSizeWindow();
		}
		break;

	case Messege::MSG_PUSH_MAPSIZE_OK_BUTTON:
		{
			// �� ������ ����
			m_uMapSize.set( static_cast<irr::u32>(messege.fData[0]), static_cast<irr::u32>(messege.fData[1]));

			// ���� �ϱ�
			Reset();

			// �����͸��� ����
			ChangeMode(EDITOR);
		}
		break;

	case Messege::MSG_PUSH_SAVE_BUTTON:
		{
			// ���̺� ������ �����
			CreateSaveWindow();
		}
		break;

	case Messege::MSG_PUSH_SAVE_WINDOW_OK_BUTTON:
		{
			if(m_pDevice->getFileSystem()->changeWorkingDirectoryTo("./Map"))
			{
				// �� �����ϱ�
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
				// �������ϴ��̾�α� �����
				CreateLoadFileDialog();

				m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../");
			}
		}
		break;

	case Messege::MSG_LOAD_SCENE_FROM_FILENAME:
		{
			// �� �ε��ϱ� - �ε��� �����Ͽ��ٸ�
			if(LoadScene(messege.cData[0]))
			{
				// ī�޶� �����
				CreateCamera();

				// �����͸���  ���º�ȯ
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

				// ������̸� �ٷ� ����� ����
			case irr::scene::jz3d::CBlockNode::TypeID:
				{
					m_pPickingNode->remove();
					MessegeLogToStaticText(L"[DELETE] Block Node !!");
				}
				break;

				// �ٿ���ڽ�����̸� �ڽ����� �����۳�尡 �ִ�.
				// ���� �ٿ���ڽ���带 �����ϸ� �����۳�带 �����ϴ� ȿ���� ����.
			case irr::scene::jz3d::CEmptyBBoxNode::TypeID:
				{
					m_pPickingNode->remove();
					MessegeLogToStaticText(L"[DELETE] ITEM Node !!");
				}
				break;

				// ��Ż���� �ѽ����� �̷���� �ְ� �� �θ� �����Ƿ�, �θ� �����Ͽ� ��Ż��弼Ʈ�� ��� ����.
			case irr::scene::jz3d::CPortalNode::TypeID:
				{
					m_pPickingNode->getParent()->remove();
					MessegeLogToStaticText(L"[DELETE] Portal Node !!");
				}
				break;
			}

			// ���ٹ��� ���·� ��ȯ
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
				// �ؽ��� ���� ���� ���̾�α� �����
				CreateBGSceneTextureFileDialog();
				m_pDevice->getFileSystem()->changeWorkingDirectoryTo("../");
			}
		}
		break;

	case Messege::MSG_SETTEXTURE_TO_BGSCENE_FROM_FILENAME:
		{
			//-------------------------------------------------------
			// irr::core::stringc�� �ƴ� std::string�� �̿��Ͽ���.
			// algorithm�� transform�� ����ϱ���ϱ� ����.
			//-------------------------------------------------------

			// ���Ͽ��´��̾�α׷κ��� ���ϸ� ������
			std::string fileName(messege.cData[0].c_str());

			// ���� �۾����丮�� ������
			std::string path = m_pDevice->getFileSystem()->getWorkingDirectory().c_str();
			// �۾����丮�� ��ҹ��ڸ� �����ϹǷ� ��� �ҹ��ڷ� �ٲ���.
			std::transform(path.begin(), path.end(), path.begin(), tolower);

			// ���ϸ��ڿ��κ��� �۾����丮���� ������+1��ŭ �����Ѵ�. ��, ����θ� ���� �ȴ�.
			std::string relative_fileName = fileName.substr(path.size()+1);
			
			// ����η� �Ǿ��ִ� ���ϸ��� �ؽ��ķ� ����Ѵ�.
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
			// ������ ����� �� ��ȭ
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


			// ��� ����� �� ��ȭ
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
			// �޽����� fData������ �̼������� ����
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			switch(value)
			{
				// �ų�带 ���� �̵�
			case GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, 0, m_fSceneNodeMicroMoveValue ));
				}
				break;

				// �ų�带 �Ʒ��� �̵�
			case GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, 0, -m_fSceneNodeMicroMoveValue ));
				}
				break;

				// �ų�带 �·� �̵�
			case GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( -m_fSceneNodeMicroMoveValue, 0, 0 ));
				}
				break;

				// �ų�带 ��� �̵�
			case GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( m_fSceneNodeMicroMoveValue, 0, 0 ));
				}
				break;

				// �ų�带 ������ �̵�
			case GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, m_fSceneNodeMicroMoveValue, 0 ) );
				}
				break;

				// �ų�带 �ڷ� �̵�
			case GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON:
				{
					m_pPickingNode->setPosition(m_pPickingNode->getPosition() + irr::core::vector3df( 0, -m_fSceneNodeMicroMoveValue, 0 ) );
				}
				break;
			}

			// �α� �޽��� ������
			MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
		}
		break;

	case Messege::MSG_PUSH_PROPERTY_MICROROTATION_BUTTON:
		{
			// �޽����� fData������ �̼������� ����
			const irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			switch(value)
			{
				// �ų�带 Y���� �������� m_fSceneNodeMicroRotationValue��ŭ ��ȸ��
			case GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON:
				{
					m_pPickingNode->setRotation(m_pPickingNode->getRotation() + irr::core::vector3df( 0, -m_fSceneNodeMicroRotationValue ,0));
				}
				break;

				// �ų�带 Y���� �������� m_fSceneNodeMicroRotationValue��ŭ ��ȸ��
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

			// �α� �޽��� ������
			MessegeLogToStaticText(L"[Rotation] ", m_pPickingNode->getRotation());
		}
		break;

	case Messege::MSG_PUSH_PROPERTY_ANIMATION_BUTTON:
		{
			// ������ ����ȯ
			irr::scene::jz3d::CBlockNode* pBlockNode = static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode );
			
			// ������� �ִϸ��̼� ����
			pBlockNode->startAnimation();
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_TYPE:
		{
			// ������� Ÿ�� �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setType( static_cast<irr::u32>(messege.fData[0]) );

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node type !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY:
		{
			// �� ����ȯ
			irr::u32 value = static_cast<irr::u32>(messege.fData[0]);

			// ����ȯ �ϱ�
			irr::scene::jz3d::CBlockNode* pBlockNode =	static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode );

			// ������� �ִϸ��̼� ����
			pBlockNode->deleteAnimation();

			// ������� �Ӽ����� ��� �ʱ�ȭ
			pBlockNode->deleteBlockPropertyValues();

			// ������� Ÿ�� �ٲٱ�
			pBlockNode->setProperty( static_cast<irr::u32>(messege.fData[0]) );

			// �� �Ӽ�â �����
			CreateBlockPropertyWindow(m_pPickingNode, value );

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node property !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0:
		{
			// ������� �Ӽ���1 �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setPropertyValue(0, messege.fData[0]);

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node property value1 !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE1:
		{
			// ������� �Ӽ���1 �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setPropertyValue(1, messege.fData[0]);

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node property value1 !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE2:
		{
			// ������� �Ӽ���1 �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setPropertyValue(2, messege.fData[0]);

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node property value2 !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_FRICTION:
		{
			// ������� ������ �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->m_fFriction = messege.fData[0];

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node frinction !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_RESTITUTION:
		{
			// ������� ź���� �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->m_fRestitution = messege.fData[0];

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node restitution !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_BLOCK_SIZE:
		{
			// ������� ������ �ٲٱ�
			static_cast<irr::scene::jz3d::CBlockNode*>( m_pPickingNode )->setSize( messege.fData[0] );

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Block Node size !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_ITEM_TYPE:
		{
			// �����۳���� Ÿ�� �ٲٱ�
			irr::scene::ISceneNode* pNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( m_pPickingNode )->getFirstChild();
			static_cast<irr::scene::jz3d::CItemNode*>( pNode )->setType( static_cast<irr::s32>(messege.fData[0]) );

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Item Node type!!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_ITEM_VALUE:
		{
			// �����۳���� �� �ٲٱ�
			irr::scene::ISceneNode* pNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( m_pPickingNode )->getFirstChild();
			static_cast<irr::scene::jz3d::CItemNode*>( pNode )->m_fValue1 = messege.fData[0];

			// �α׸޽��� ������
			MessegeLogToStaticText(L"[CHANGE] Item Node Value !!");
		}
		break;

	case Messege::MSG_CHANGE_PROPERTY_ITEM_DURATIONTIME:
		{
			// �����۳���� ���ӽð� �ٲٱ�
			irr::scene::ISceneNode* pNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode*>( m_pPickingNode )->getFirstChild();
			static_cast<irr::scene::jz3d::CItemNode*>( pNode )->m_fDurationTime = messege.fData[0];

			// �α׸޽��� ������
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
			// ���콺 �����Ͱ� GUI���� �������� �ʴ� ��쿡�� ����� �̺�Ʈ �߻� - �ڵ� ����
			//-------------------------------------------------------------------------------------------
			if(!m_bGUIHoveredEvent)
			{
				// ī�޶� ���� ��ũ�� �ϱ�
				if(m_Key[irr::KEY_UP])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, 0, m_fCameraMoveValue );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// ī�޶� �Ʒ��� ��ũ�� �ϱ�
				if(m_Key[irr::KEY_DOWN])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, 0, -m_fCameraMoveValue );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// ī�޶� ���������� ��ũ�� �ϱ�
				if(m_Key[irr::KEY_RIGHT])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( m_fCameraMoveValue, 0, 0 );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// ī�޶� �������� ��ũ�� �ϱ�
				if(m_Key[irr::KEY_LEFT])
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( -m_fCameraMoveValue, 0, 0 );
					m_pCameraNode->setPosition(pos);
					pos.Y = -300;
					m_pCameraNode->setTarget(pos);
				}

				// ī�޶� �� ��
				if(m_Key_MouseWheel_Up)
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, -m_fCameraZoomValue, 0 );
					m_pCameraNode->setPosition(pos);

					m_Key_MouseWheel_Up = false;
				}

				// ī�޶� �� �ƿ�
				if(m_Key_MouseWheel_Down)
				{
					irr::core::vector3df pos = m_pCameraNode->getPosition() + irr::core::vector3df( 0, m_fCameraZoomValue, 0 );
					m_pCameraNode->setPosition(pos);

					m_Key_MouseWheel_Down = false;
				}

				// ���ٹ��� ���� ��ȯ
				if(m_Key[irr::KEY_ESCAPE])
				{
					ChangeEditorMode(MODE_STAND_BY);
				}

				// ���콺 Ŭ���� ���� ����ƽ ������ �巡�׸� ���� �÷��׺���
				static irr::core::dimension2di first_click_pos, last_click_pos, end_click_pos;
				static bool dragFlag = false;

				if(m_Key[irr::KEY_LBUTTON])
				{
					//------------------- ù��° Ŭ�� �ڵ� ����-------------------
					if(!dragFlag)
					{
						// �巡�� ���·� ��ȯ
						dragFlag = true;

						// ���콺�� ó���� ������ ��ġ�� ����
						first_click_pos = last_click_pos = end_click_pos = GetMouse2DPosition();

						switch(m_uEditorStatus)
						{

						case MODE_STAND_BY:
							{
								// ��ŷ�ϱ�
								m_pPickingNode = GetPickingNode(first_click_pos);

								// ��ŷ�� ��尡 �ִٸ� ��ŷ���� ��ȯ
								if(m_pPickingNode)
								{
									ChangeEditorMode(MODE_PICK_NODE);
									// ȭ�鿡 ����� ��ġ ǥ��
									MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
								}
							}
							break;

						case MODE_PICK_NODE:
							{
								irr::scene::ISceneNode* pPickedNode = m_pPickingNode;

								// ��ŷ�ϱ�
								m_pPickingNode = GetPickingNode(first_click_pos);

								// ��ŷ�� �� ��尡 �ִٸ�
								if(m_pPickingNode)
								{
									// ������ ��ŷ�� ���� ���� ��ŷ�� ��尡 �ٸ���츸 ��ŷ��� �� �ǽ�
									if(m_pPickingNode != pPickedNode)
									{
										ChangeEditorMode(MODE_PICK_NODE);
										// ȭ�鿡 ����� ��ġ ǥ��
										MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
									}
								}
								// ��ŷ�� �� ��尡 ������ ������ ��ŷ �Ǿ��� ��尡 �ٽ� ����
								else
								{
									m_pPickingNode = pPickedNode;
								}
							}
							break;

						case MODE_MAKE_BLOCK:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_MAKE_ITEM:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_MAKE_PORTAL:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;
						}

					}
					//------------------- ù��° Ŭ�� �ڵ� ����-------------------

					//------------------- �巡�� ���� �ڵ� ����-------------------
					else
					{
						// �巡�� ���� ���콺 ��ġ�� ����
						end_click_pos = GetMouse2DPosition();

						switch(m_uEditorStatus)
						{

						case MODE_STAND_BY:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_PICK_NODE:
							{
								// ��ŷ�� ��尡 �ܰ�������̸� �̵��ϸ� �ȵȴ�.
								// ���� ��ŷ�� ��尡 �ܰ���������� üũ�غ��� �ƴ� ��쿡��
								// �巡�׸� ���� ��带 �̵���Ų��.
								if( !CheckBackgoundNode(m_pPickingNode) )
								{
									//---------------------------------------------------------
									// �巡�׸� ���Ͽ� �����ġ�� �����ϴ� �ڵ� - ����
									//---------------------------------------------------------

									// ���콺�� ��ǥ���� ���Ѵٸ� �����̴� �巡�� �������� �ǹ�
									// �� �巡�� ���·� �̵��ϴ°��� ��Ÿ��.
									if(last_click_pos != end_click_pos)
									{
										//-------------------------------------------------------
										// ��ŷ�� �ų���� ������ġ�� ������� ��ġ ���
										// ��, �巡�׸� ���� �ų���� ��ġ�� ������.
										// ����� �ų�� ��ġ �̵��ϴ� �ڵ�
										//--------------------------------------------------------

										// �ų���� ��ġ �̵��� �ʿ��� ���ͺ���
										irr::core::vector3df current_pos, last_pos , relative_pos, result_pos;

										// ���� ���콺�� 3D ��ġ
										GetPositionFromBackgourndNode(end_click_pos, current_pos);

										// ���� ���콺�� 3D ��ġ
										GetPositionFromBackgourndNode(last_click_pos, last_pos);

										// ���� ���콺�� ��ġ���� ������ ���콺 ��ġ ���� ���
										relative_pos = current_pos - last_pos;

										// �������� ��ġ�� ���� �ų���� ��ġ�� ������� �Ÿ���ŭ �����̸� �ȴ�.
										result_pos = m_pPickingNode->getPosition() + relative_pos;
										//result_pos.Y = 0;

										
										// ���࿡ �� ����̸�
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
											// ����� �����Ÿ��� ����.
											m_pPickingNode->setPosition(result_pos);
										}

										// �α� �޽��� ���
										MessegeLogToStaticText(L"[Position]", m_pPickingNode->getAbsolutePosition());
									}

									//---------------------------------------------------------
									// �巡�׸� ���Ͽ� �����ġ�� �����ϴ� �ڵ� - ����
									//---------------------------------------------------------
								}
							}
							break;

						case MODE_MAKE_BLOCK:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_MAKE_ITEM:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_MAKE_PORTAL:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;
						}


						// ���콺�� ������ ��ġ�� ����
						last_click_pos = end_click_pos;
					}
					//------------------- �巡�� ���� �ڵ� ����-------------------
				}
				else
				{
					if(dragFlag)
					{// �巡�� ����

						// �巡�� ���� ����
						dragFlag = false;

						// �巡�� ���� �� ���콺 ��ġ�� ����
						end_click_pos = GetMouse2DPosition();

						switch(m_uEditorStatus)
						{// switch - start

						case MODE_STAND_BY:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_PICK_NODE:
							{
								// ������� ����... �ڵ尡 �򰥸���� case���� �߰�...
							}
							break;

						case MODE_MAKE_BLOCK:
							{
								// ���۰� ���� ��ġ��
								irr::core::vector3df start, end;

								// ���콺�� ù��° Ŭ�������� ������ �巡�� �������� ���� 3D��ǥ �� ����.
								// 3D ��ǥ��(����, ���� ��ġ)���� ���� ����� �����
								if(GetPositionFromBackgourndNode(first_click_pos, start) && GetPositionFromBackgourndNode(end_click_pos, end))
								{
									// ����� �����
									irr::scene::ISceneNode* pNode = MakeBlockNode(start, end);

									// �۾������丮�� �����ϱ�
									WorkingHistory history;
									history.type = WorkingHistory::WORKING_MAKE_BLOCK_NODE;
									history.start = start;
									history.end = end;
									history.sceneNode = pNode;
									AddWorkingHitory(history);
								}

								// ���ٹ��� ���� ��ȯ
								ChangeEditorMode(MODE_STAND_BY);
							}
							break;

						case MODE_MAKE_ITEM:
							{
								// ���� ��ġ��
								irr::core::vector3df end;

								// ������ �巡�� �������� ���� 3D��ǥ �� ����.
								// 3D ��ǥ��(���� ��ġ)���� ���� �����۳�� �����
								if( GetPositionFromBackgourndNode(end_click_pos, end) )
								{
									// �����۳�� �����
									irr::scene::ISceneNode* pNode = MakeItemNode(end);

									// �۾������丮�� �����ϱ�
									WorkingHistory history;
									history.type = WorkingHistory::WORKING_MAKE_BLOCK_NODE;
									history.end = end;
									history.sceneNode = pNode;
									AddWorkingHitory(history);
								}

								// ���ٹ��� ���� ��ȯ
								ChangeEditorMode(MODE_STAND_BY);

							}
							break;

						case MODE_MAKE_PORTAL:
							{
								// ���� ��ġ��
								irr::core::vector3df end;

								// ������ �巡�� �������� ���� 3D��ǥ �� ����.
								// 3D ��ǥ��(���� ��ġ)���� ���� ��Ż��� �����
								if( GetPositionFromBackgourndNode(last_click_pos, end) )
								{
									// �����۳�� �����
									irr::scene::ISceneNode* pNode = MakePortalNode(end);

									// �۾������丮�� �����ϱ�
									WorkingHistory history;
									history.type = WorkingHistory::WORKING_MAKE_BLOCK_NODE;
									history.end = end;
									history.sceneNode = pNode;
									AddWorkingHitory(history);
								}

								// ���ٹ��� ���� ��ȯ
								ChangeEditorMode(MODE_STAND_BY);
							}
							break;

						case MODE_MAKE_ARROW:
							{
								// ���� ��ġ��
								irr::core::vector3df end;

								// ������ �巡�� �������� ���� 3D��ǥ �� ����.
								// 3D ��ǥ��(���� ��ġ)���� ���� ��Ż��� �����
								if( GetPositionFromBackgourndNode(last_click_pos, end) )
								{
									// �����۳�� �����
									irr::scene::ISceneNode* pNode = MakeAroowNode(end);
								}

								// ���ٹ��� ���� ��ȯ
								ChangeEditorMode(MODE_STAND_BY);
							}
							break;

						}// switch - end

					}// �巡�� ���� ����
				}

			}
			//-------------------------------------------------------------------------------------------
			// ���콺 �����Ͱ� GUI���� �������� �ʴ� ��쿡�� ����� �̺�Ʈ �߻� - �ڵ� ����
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
	// IO ���μ���
	{
		ProccessingIOEvent();
	}

	// �޽��� ť ó��
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

			// ���콺 Ŀ���� GUI ���� �ִ°�쿡�� �������� ���콺Ŭ���̺�Ʈ�� ������� �ʴ´�.
			// -------------------------- ���콺 Ŀ�� �̺�Ʈ ���� �ڵ� - ���� --------------------------
			if(event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED)
			{
				m_bGUIHoveredEvent = true;
			}
			else if(event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT)
			{
				m_bGUIHoveredEvent = false;
			}
			// -------------------------- ���콺 Ŀ�� �̺�Ʈ ���� �ڵ� - ���� --------------------------

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
						// ���� �⺻ �ؽ��� �޺��ڽ� �̺�Ʈ
					case GUI_BLOCK_TEXTURE_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_TEXTURE;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ��Ÿ�� �޺��ڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_TYPE_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_TYPE;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ���Ӽ� �޺��ڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ȸ������ ȸ������ �޺��ڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ROTATION_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ���������� ���� �޺��ڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_SPRING_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0;
							msg.fData.push_back( static_cast<irr::f32>(box->getSelected()) );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ������Ÿ�� �޺��ڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_ITEM_TYPE_COMBOBOX:
						{
							irr::gui::IGUIComboBox* box = static_cast<irr::gui::IGUIComboBox*>( event.GUIEvent.Caller );
							// �޽��� �����
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
						// ���� �⺻ ������ ���ɹڽ� �̺�Ʈ
					case GUI_BLOCK_FRICTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_FRICTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// ���� �⺻ ź���� ���ɹڽ� �̺�Ʈ
					case GUI_BLOCK_RESTITUTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_RESTITUTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// ���� �⺻ ������ ���ɹڽ� �̺�Ʈ
					case GUI_BLOCK_SIZE_SPINEBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_BLOCK_BASIC_SIZE;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� �� ������ ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_FRICTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_FRICTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� �� ź���� ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_RESTITUTION_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_RESTITUTION;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� �� ������ ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_SIZE_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_SIZE;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� �� �Ӽ���0 ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE0_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE0;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� �� �Ӽ���1 ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE1_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE1;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� �� �Ӽ���2 ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_VALUE2_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_BLOCK_PROPERTY_VALUE2;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ������ �� ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_ITEM_VALUE_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
							Messege msg;
							msg.type = Messege::MSG_CHANGE_PROPERTY_ITEM_VALUE;
							msg.fData.push_back( box->getValue() );

							AddMessege(msg);
						}
						break;

						// �Ӽ��������� ������ ���ӽð� ���ɹڽ� �̺�Ʈ
					case GUI_PROPERTY_WINDOW_ITEM_DURATIONTIME_SPINBOX:
						{
							irr::gui::IGUISpinBox* box = static_cast<irr::gui::IGUISpinBox*>( event.GUIEvent.Caller );
							// �޽��� �����
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
					// ���ϴ��̾�α� ������ ������
					irr::gui::IGUIFileOpenDialog* dialog = static_cast<irr::gui::IGUIFileOpenDialog*>( event.GUIEvent.Caller );

					// �޽��� �����
					Messege msg;
					msg.cData.push_back(irr::core::stringc(dialog->getFileName()));

					// �޽��� Ÿ��
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

					// �޽���ť�� ����
					AddMessege(msg);

					// GUI horvered �̺�Ʈ�� ��� ���� �ֱ� ������ ������ �÷��׸� false�� ����.
					m_bGUIHoveredEvent = false;
				}
				break;

				// ���� ���� ���̾�α׸� ����Ҷ� �̺�Ʈ
			case irr::gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				{
					// GUI horvered �̺�Ʈ�� ��� ���� �ֱ� ������ ������ �÷��׸� false�� ����.
					m_bGUIHoveredEvent = false;			
				}
				break;

			case irr::gui::EGET_BUTTON_CLICKED:
				{
					switch(id)
					{
						// ���� ����� ��ư �̺�Ʈ
					case GUI_NEW_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_NEW_BUTTON;
							AddMessege(msg);
						}
						break;

						// �ҷ����� ��ư �̺�Ʈ
					case GUI_LOAD_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_LOAD_BUTTON;
							AddMessege(msg);
						}
						break;

						// ������ ��ư �̺�Ʈ
					case GUI_EXIT_BUTTON:
						{
							// �����ϱ�
							m_pDevice->closeDevice();
						}
						break;

						// �޴��� ���ư��� ��ư �̺�Ʈ
					case  GUI_RETURN_MENU_BUTTON:
						{
							// Ŭ����
							Messege msg1;
							msg1.type = Messege::MSG_CLEAR;
							AddMessege(msg1);

							// �޴� ���� �ٲٱ�
							Messege msg2;
							msg2.type = Messege::MSG_CHANGE_MAIN_MODE;
							msg2.fData.push_back(MENU);
							AddMessege(msg2);
						}
						break;

						// �ʻ����� OK ��ư �̺�Ʈ
					case GUI_MAPSIZE_OK_BUTTON:
						{
							//---------------------------
							// ������ ������ ��������
							//---------------------------

							// ����Ʈ �ڽ�(���� ������) ������ ������
							irr::gui::IGUIEditBox *box1 = static_cast<irr::gui::IGUIEditBox *>( m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAPSIZE_EDITBOX_HORIZONTAL, true) );
							// ����Ʈ �ڽ��κ��� ���λ�����(�ؽ�Ʈ ����) ������
							irr::core::stringc size1_text(box1->getText());
							// �ؽ�Ʈ���� ���ڷ�, int���� float���� ����ȯ �ϱ�
							irr::f32 size1 = static_cast<irr::f32>(atoi(size1_text.c_str()));


							//---------------------------
							// ������ ������ ��������
							//---------------------------

							// ����Ʈ �ڽ�(���� ������) ������ ������
							irr::gui::IGUIEditBox *box2 = static_cast<irr::gui::IGUIEditBox *>( m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_MAPSIZE_EDITBOX_VERTICAL, true) );
							// ����Ʈ �ڽ��κ��� ���λ�����(�ؽ�Ʈ ����) ������
							irr::core::stringc size2_text(box2->getText());
							// �ؽ�Ʈ���� ���ڷ�, int���� float���� ����ȯ �ϱ�
							irr::f32 size2 = static_cast<irr::f32>(atoi(size2_text.c_str()));

							//-----------------
							// �޽��� �����
							//-----------------
							Messege msg;
							// �޽��� Ÿ��
							msg.type = Messege::MSG_PUSH_MAPSIZE_OK_BUTTON;
							// �޽��� ������
							msg.fData.push_back(size1);
							msg.fData.push_back(size2);
							// �޽���ť�� ����
							AddMessege(msg);

							// �ʻ����� ������ â ����
							event.GUIEvent.Caller->getParent()->remove();

							// GUI���� �Ŀ��� GUI horverd �̺�Ʈ �����̹Ƿ� ������ �÷��׸� false�� ����
							m_bGUIHoveredEvent = false;
						}
						break;

						// �ʻ����� CANCEL ��ư �̺�Ʈ
					case GUI_MAPSIZE_CANCEL_BUTTON:
						{
							// �ʻ����� ������ â ����
							event.GUIEvent.Caller->getParent()->remove();

							// GUI���� �Ŀ��� GUI horverd �̺�Ʈ �����̹Ƿ� ������ �÷��׸� false�� ����
							m_bGUIHoveredEvent = false;
						}
						break;

						// ���̺� ��ư �̺�Ʈ
					case GUI_SAVE_BUTTON:
						{
							// �޽��� �����
							Messege msg;
							// �޽��� Ÿ��
							msg.type = Messege::MSG_PUSH_SAVE_BUTTON;							
							// �޽���ť�� ����
							AddMessege(msg);
						}
						break;

						// ���̺� OK��ư �̺�Ʈ
					case GUI_SAVE_WINDOW_OK_BUTTON:
						{
							// �޽��� �����
							Messege msg;
							//�޽��� Ÿ��
							msg.type = Messege::MSG_PUSH_SAVE_WINDOW_OK_BUTTON;

							// ���̺��������� ����Ʈ �ڽ� ������ ������
							irr::gui::IGUIEditBox *box = static_cast<irr::gui::IGUIEditBox *>( m_pGuiEnv->getRootGUIElement()->getElementFromId(GUI_SAVE_WINDOW_EDITBOX, true) );
							// ����Ʈ �ڽ��κ��� ���̺� �� ���� �̸� �� ����.
							msg.cData.push_back(irr::core::stringc(box->getText()));

							// �޽���ť�� ����
							AddMessege(msg);

							// ���̺� ������ â ����
							event.GUIEvent.Caller->getParent()->remove();

							// GUI���� �Ŀ��� GUI horverd �̺�Ʈ �����̹Ƿ� ������ �÷��׸� false�� ����
							m_bGUIHoveredEvent = false;
						}
						break;

						// ���̺� CANCEL��ư �̺�Ʈ
					case GUI_SAVE_WINDOW_CANCEL_BUTTON:
						{
							// ���̺� ������ â ����
							event.GUIEvent.Caller->getParent()->remove();

							// GUI���� �Ŀ��� GUI horverd �̺�Ʈ �����̹Ƿ� ������ �÷��׸� false�� ����
							m_bGUIHoveredEvent = false;
						}
						break;

						// ������� ��ư �̺�Ʈ
					case GUI_MAKE_BLOCK_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_BLOCK_BUTTON);
							AddMessege(msg);
						}
						break;

						// �����۸���� ��ư �̺�Ʈ
					case GUI_MAKE_ITEM_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_ITEM_BUTTON);
							AddMessege(msg);
						}
						break;

						// ��Ż����� ��ư �̺�Ʈ
					case GUI_MAKE_PORTAL_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_PORTAL_BUTTON);
							AddMessege(msg);
						}
						break;

						// ȭ��ǥ����� ��ư �̺�Ʈ
					case GUI_MAKE_ARROW_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_MAKE_BUTTON;
							msg.fData.push_back(GUI_MAKE_ARROW_BUTTON);
							AddMessege(msg);
						}
						break;

						// ���� ��ư �̺�Ʈ
					case GUI_COPY_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_COPY_BUTTON;
							AddMessege(msg);
						}
						break;

						// ���� ��ư �̺�Ʈ
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

						// ������� �ؽ��� ���� ��ư �̺�Ʈ
					case GUI_TEXTURE_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_TEXTURE_BUTTON;
							AddMessege(msg);
						}
						break;

						// �� ��ü �Ӽ��� �ٲٱ� ��ư �̺�Ʈ
					case GUI_CHANGE_ALL_BLOCK_PROPERTY_BUTTON:
						{
							Messege msg;
							msg.type = Messege::MSG_PUSH_CHANGE_ALL_BLOCK_PROPERTY_BUTTON;
							msg.pData = event.GUIEvent.Caller;
							AddMessege(msg);
						}
						break;

						// �̼����� (���� �̵�) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_UP_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (�Ʒ��� �̵�) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_DOWN_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (�·� �̵�) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_LEFT_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (��� �̵�) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_RIGHT_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (������ �̵�) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_FRONT_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (�ڷ� �̵�) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROMOVE_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROMOVE_BACK_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (�·� ȸ��) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROROTATION_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROROTATION_LEFT_BUTTON);
							AddMessege(msg);
						}
						break;

						// �̼����� (��� ȸ��) ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
							msg.type = Messege::MSG_PUSH_PROPERTY_MICROROTATION_BUTTON;
							// GUI ���Ű����� ������ ����
							msg.fData.push_back(GUI_PROPERTY_WINDOW_MICROROTATION_RIGHT_BUTTON);
							AddMessege(msg);
						}
						break;

						// ����� �ִϸ��̼� ���� ��ư �̺�Ʈ
					case GUI_PROPERTY_WINDOW_BLOCK_PROPERTY_ANIMATION_BUTTON:
						{
							Messege msg;
							// �̼������� �޽���
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
			// ���콺 ���� ��ư
			if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
			{
				m_Key[irr::KEY_LBUTTON] = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
			{
				m_Key[irr::KEY_LBUTTON] = false;
			}

			// ���콺 ������ ��ư
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