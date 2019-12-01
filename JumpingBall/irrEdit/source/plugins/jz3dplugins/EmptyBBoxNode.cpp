#include "EmptyBBoxNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	const char *CEmptyBBoxNode::Name  = "CEmptyBBoxNode";

	CEmptyBBoxNode::CEmptyBBoxNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::f32 x, irr::f32 y, irr::f32 z)
		: ISceneNode(parent,mgr,id), m_fSize(x, y, z)
	{
#ifdef _DEBUG
		setDebugName(CEmptyBBoxNode::Name);
#endif
		setAutomaticCulling(scene::EAC_OFF);

		// 바운딩박스 초기화
		{
			m_Box.MaxEdge = m_fSize/2.f;
			m_Box.MinEdge = m_fSize/-2.f;
		}

		// 사이즈 설정
		{
			setSize(m_fSize);
		}

		// 이름 설정
		{
			this->setName("BoundingBox");
		}

		this->setDebugDataVisible(false);
	}

	CEmptyBBoxNode::~CEmptyBBoxNode()
	{
		
	}

	void CEmptyBBoxNode::setSize(irr::core::vector3df size)
	{
		if(size != m_fSize)
		{
			m_fSize = size;

			m_Box.MaxEdge = m_fSize/2.f;
			m_Box.MinEdge = m_fSize/-2.f;
		}
	}

	const core::aabbox3d<f32>& CEmptyBBoxNode::getBoundingBox() const
	{
		return m_Box;
	}

	void CEmptyBBoxNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	void CEmptyBBoxNode::render()
	{
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		driver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
		
		
		if(DebugDataVisible)
		{
			driver->draw3DBox(m_Box, irr::video::SColor(255,255,0,0));
		}
	}

	ISceneNode* CEmptyBBoxNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent) newParent = Parent;
		if (!newManager) newManager = SceneManager;

		CEmptyBBoxNode* nb = new CEmptyBBoxNode(newParent, 
			newManager, ID);

		nb->cloneMembers(this, newManager);

		nb->m_Box = m_Box;
		nb->m_fSize = m_fSize;

		nb->drop();

		return nb;
	}		


	void CEmptyBBoxNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		ISceneNode::serializeAttributes(out, options);		

		//데이터 쓰기 코드추가
		//...
		out->addVector3d("BoundingBox size", m_fSize);
	}

	//! Reads attributes of the scene node.
	void CEmptyBBoxNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{	
		//데이터 읽기 코드추가
		//...
		setSize(in->getAttributeAsVector3d("BoundingBox size"));

		ISceneNode::deserializeAttributes(in, options);				
	}

	irr::scene::ISceneNode* CEmptyBBoxNode::getFirstChild()
	{
		irr::scene::ISceneNode *pNode = NULL;

		irr::core::list<irr::scene::ISceneNode *> children = this->getChildren();

		if(children.getSize())
		{
			irr::core::list<irr::scene::ISceneNode *>::Iterator it = children.begin();
			pNode = (*it);
		}

		return pNode;
	}

	irr::scene::jz3d::CEmptyBBoxNode* CEmptyBBoxNode::CreateEmptyBBoxNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df position, irr::core::vector3df size)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CEmptyBBoxNode* pNode = new CEmptyBBoxNode(parent, mgr, id);
		
		position.Y = 0;

		pNode->setPosition(position);
		pNode->setSize(size);

		pNode->drop();

		return pNode;
	}

}
}
}