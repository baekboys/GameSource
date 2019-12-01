#include "ItemNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	int CItemNode::itemCount = 0;
	const char *CItemNode::Name  = "CItemNode";


	const wchar_t* CItemNode::typeNames[] =			
			{
				L"CHANGE BALL",
				L"CHANGE JUMP FORCE",
				L"CHANGE SIZE",
				/*L"DIRECTION ON",*/
				L"TIME +/-",
				L"CHANGE SPEED",
				L"ZERO_GRAVITY",
				0
			};

	CItemNode::CItemNode(
		ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position,
		const core::vector3df& rotation,
		const core::vector3df& scale,
		irr::f32 cubeSize
		)
		: IMeshSceneNode(parent,mgr,id, position, rotation, scale), m_fValue1(0), m_fDurationTime(0), m_fCubeSize(cubeSize)
	{
#ifdef _DEBUG
		setDebugName(CItemNode::Name);
#endif
		setAutomaticCulling(scene::EAC_OFF);

		// 아이템의 카운트 1개 증가 및 노드이름 정하기
		{
			itemCount++;
			irr::core::stringc temp("Item/");
			temp += itemCount;
			this->setName(temp.c_str());
		}

		// 바운딩박스 초기화
		{
			m_Box.MaxEdge = irr::core::vector3df(m_fCubeSize/2.f);
			m_Box.MinEdge = irr::core::vector3df(m_fCubeSize/-2.f);
		}

		// 매트리얼 설정
		{
			// 매트리얼 설정
			irr::video::SMaterial m;
			m.Lighting = false;
			m.BackfaceCulling = false;
			
			// 텍스쳐 설정
			m.setTexture(0, SceneManager->getVideoDriver()->getTexture("texture/itemnode.png"));

			irr::video::SMaterial mt;

			// 0번 매트리얼은 m, 나머지는 mt로 초기화
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
		}


		// 아이템 속성 설정
		{
			setType(JUMP);
		}

		// 큐브메쉬 만들기
		{
			Mesh = createCubeMesh();
		}

		// 애니메이션 추가
		{
			// x축으로 90도 회전
			this->setRotation(irr::core::vector3df(90, 0,0));
			
			// 상하로 움직이는 애니메이터 생성
			irr::core::array<irr::core::vector3df> vectorArray;
			irr::core::vector3df start(0,0,-2);
			irr::core::vector3df end(0,0,2);
			vectorArray.push_back(start);
			vectorArray.push_back(end);
			irr::scene::ISceneNodeAnimator *pFlwSpinAnim = SceneManager->createFollowSplineAnimator(0, vectorArray, 1.0f, 0.5f);
			this->addAnimator(pFlwSpinAnim);
			pFlwSpinAnim->drop();
		}
	}

	CItemNode::~CItemNode()
	{
		itemCount--;

		if(Mesh)
			Mesh->drop();
	}

	irr::scene::IMesh* CItemNode::createCubeMesh()
	{
		IMesh* mesh = SceneManager->getGeometryCreator()->createCubeMesh( irr::core::vector3df(m_fCubeSize) );	
		return mesh;
	}

	void CItemNode::setSize(irr::f32 size)
	{
		if(size != m_fCubeSize)
		{
			m_fCubeSize = size;

			if(Mesh)
				Mesh->drop();

			Mesh = createCubeMesh();
		}
	}

	const core::aabbox3d<f32>& CItemNode::getBoundingBox() const
	{
		return m_Box;
	}

	void CItemNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	void CItemNode::render()
	{
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		driver->setMaterial(getMaterial(0));
		driver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
		
		// 매쉬버퍼로 그리기
		driver->drawMeshBuffer(Mesh->getMeshBuffer(0));


		if(DebugDataVisible)
		{
			driver->draw3DBox(m_Box, irr::video::SColor(255,255,0,0));
		}
	}

	ISceneNode* CItemNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent) newParent = Parent;
		if (!newManager) newManager = SceneManager;

		CItemNode* nb = new CItemNode(newParent, 
			newManager, ID);

		nb->cloneMembers(this, newManager);
		
		// 신노드의 이름 다시 설정하기.
		// 그 이유는 위의 cloneMembers함수를 호출하여 신노드의 이름까지 복사되었다.
		irr::core::stringc temp("Item/");
		temp += itemCount;
		nb->setName(temp.c_str());

		// 멤버변수 복사
		nb->m_sItemType = m_sItemType;
		nb->m_fValue1 = m_fValue1;
		nb->m_fDurationTime = m_fDurationTime;
		nb->setSize(m_fCubeSize);

		nb->drop();
		return nb;
	}		


	void CItemNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		ISceneNode::serializeAttributes(out, options);		

		//데이터 쓰기 코드추가
		//...
		out->addInt("item type", m_sItemType);
		out->addFloat("Effect Value", m_fValue1);
		out->addFloat("Duration Time", m_fDurationTime);
		out->addFloat("item size", m_fCubeSize);
	}

	//! Reads attributes of the scene node.
	void CItemNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{	
		//데이터 읽기 코드추가
		//...

		m_sItemType = in->getAttributeAsInt("item type");
		setType(m_sItemType);
		m_fValue1 = in->getAttributeAsFloat("Effect Value");
		m_fDurationTime = in->getAttributeAsFloat("Duration Time");
		setSize( in->getAttributeAsFloat("item size") );

		ISceneNode::deserializeAttributes(in, options);				
	}

	void CItemNode::setType(irr::s32 type)
	{
		m_sItemType = type;

		switch(m_sItemType)
		{
		case CHANGE_TYPE :
			{
				m_fValue1 = 0.f;
				m_fDurationTime = 30.f;
			}
			break;

		case JUMP :
			{
				m_fValue1 = 3.f;
				m_fDurationTime = 15.f;
			}
			break;

		case SIZE :
			{
				m_fValue1 = 2.f;
				m_fDurationTime = 10.f;
			}
			break;

		/*case DIRECTION :
			{
				m_fValue1 = 0;
				m_fDurationTime = 15.f;
			}*/

		case TIME :
			{
				m_fValue1 = 30;
				m_fDurationTime = 0.f;
			}
			break;

		case SPEED :
			{
				m_fValue1 = 1.5f;
				m_fDurationTime = 20.f;
			}
			break;

		case ZERO_GRAVITY :
			{
				m_fValue1 = 0;
				m_fDurationTime = 10.f;
			}
			break;
		}
	}


	irr::scene::jz3d::CItemNode* CItemNode::CreateItemNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df position)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CItemNode* pNode = new CItemNode(parent, mgr, id);
		position.Y = 0;
		pNode->setPosition(position);
		pNode->drop();
		
		return pNode;
	}

}
}
}