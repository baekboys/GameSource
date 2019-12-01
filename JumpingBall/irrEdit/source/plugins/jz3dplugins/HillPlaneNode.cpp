#include "HillPlaneNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{

	const char *CHillPlaneNode::Name  = "CHillPlaneNode";

	CHillPlaneNode::CHillPlaneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		const irr::core::vector3df& position,
		const irr::core::vector3df& rotation,
		const irr::core::vector3df& scale,
		irr::u32 basicSize,
		irr::core::dimension2d<irr::u32> size
		)
		: IMeshSceneNode(parent,mgr,id, position, rotation, scale), Mesh(NULL), m_uSize(size), m_uBasicSize(basicSize)
	{
#ifdef _DEBUG
		setDebugName(CHillPlaneNode::Name);
#endif
		setAutomaticCulling(scene::EAC_OFF);

		// 비디오 드라이버 포인터 얻기
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();

		// 매트리얼 설정
		{
			// 매트리얼 설정
			irr::video::SMaterial m;
			m.Lighting = false;
			m.BackfaceCulling = false;
			
			irr::video::SMaterial mt;

			// 0번 매트리얼은 m, 나머지는 mt로 초기화
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
		}
		
		// 힐플레인매쉬 만들기
		{
			Mesh = createHillPlaneMesh(getMeshName().c_str());
			
			if(Mesh)
				Mesh->grab();
		}
	}

	CHillPlaneNode::~CHillPlaneNode()
	{
		if(Mesh)
			Mesh->drop();
	}

	irr::core::stringc CHillPlaneNode::getMeshName()
	{
		irr::core::stringc name;
		name += m_uBasicSize;
		name += "_";
		name += m_uSize.Width;
		name += "_";
		name += m_uSize.Height;
		name += "_";
		name += "HillMesh";

		return name;
	}

	irr::scene::IMesh* CHillPlaneNode::createHillPlaneMesh(irr::core::stringc meshName)
	{
		irr::scene::IMesh* pMesh;

		if(SceneManager->getMeshCache()->isMeshLoaded(meshName.c_str()))
		{
			pMesh = SceneManager->getMesh(meshName.c_str());
		}
		else
		{
			pMesh = SceneManager->addHillPlaneMesh(
				meshName.c_str(),
				irr::core::dimension2df((irr::f32)m_uBasicSize, (irr::f32)m_uBasicSize),
				irr::core::dimension2d<irr::u32>(m_uSize.Width / m_uBasicSize, m_uSize.Height / m_uBasicSize)
				);
		}

		return pMesh;
	}

	void CHillPlaneNode::setSize(irr::u32 basicSize, irr::core::dimension2d<irr::u32> size)
	{
		if(m_uSize != size || m_uBasicSize != basicSize)
		{
			if(Mesh)
				Mesh->drop();

			m_uSize = size;
			m_uBasicSize = basicSize;

			Mesh = createHillPlaneMesh(getMeshName());

			if(Mesh)
				Mesh->grab();
		}
	}

	void CHillPlaneNode::setSize(irr::core::dimension2d<irr::f32> size)
	{
		if(Mesh)
			Mesh->drop();

		m_uSize = size;

		Mesh = createHillPlaneMesh(getMeshName(), size);

		if(Mesh)
			Mesh->grab();
	}

	irr::scene::IMesh* CHillPlaneNode::createHillPlaneMesh(irr::core::stringc meshName, irr::core::dimension2d<irr::f32>& size)
	{
		irr::scene::IMesh* pMesh;

		if(SceneManager->getMeshCache()->isMeshLoaded(meshName.c_str()))
		{
			pMesh = SceneManager->getMesh(meshName.c_str());
		}
		else
		{
			pMesh = SceneManager->addHillPlaneMesh(
				meshName.c_str(),
				size,
				irr::core::dimension2d<irr::u32>(1,1)
				);
		}

		return pMesh;
	}



	void CHillPlaneNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	void CHillPlaneNode::render()
	{
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		driver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
		driver->setMaterial(getMaterial(0));
		
		// 매쉬버퍼로 그리기
		irr::u32 i;
		for(i = 0 ; i < Mesh->getMeshBufferCount() ; ++i)
			driver->drawMeshBuffer(Mesh->getMeshBuffer(i));

		
		if(DebugDataVisible)
		{
			driver->draw3DBox(Mesh->getBoundingBox(), irr::video::SColor(255,255,0,0));
		}
	}


	const core::aabbox3d<f32>& CHillPlaneNode::getBoundingBox() const
	{
		return Mesh->getBoundingBox();
	}


	ISceneNode* CHillPlaneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent) newParent = Parent;
		if (!newManager) newManager = SceneManager;

		CHillPlaneNode* nb = new CHillPlaneNode(newParent, 
			newManager, ID);

		nb->cloneMembers(this, newManager);

		nb->m_Matrial = m_Matrial;
		nb->setSize(m_uBasicSize, m_uSize);

		nb->drop();
		return nb;
	}		

	void CHillPlaneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		ISceneNode::serializeAttributes(out, options);		

		//데이터 쓰기 코드추가
		//...
		out->addInt("Basic size", m_uBasicSize);
		out->addInt("Size width", m_uSize.Width);
		out->addInt("Size height", m_uSize.Height);
	}

	//! Reads attributes of the scene node.
	void CHillPlaneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{	
		//데이터 읽기 코드추가
		//...
		irr::u32 basicSize;
		irr::core::dimension2d<irr::u32> size;
		basicSize = in->getAttributeAsInt("Basic size");
		size.Width = in->getAttributeAsInt("Size width");
		size.Height = in->getAttributeAsInt("Size height");

		ISceneNode::deserializeAttributes(in, options);

		setSize(basicSize, size);
	}

	void CHillPlaneNode::setTexture(irr::video::ITexture* texture)
	{
		if(texture)
			this->getMaterial(0).setTexture(0, texture);
	}

	irr::scene::jz3d::CHillPlaneNode* CHillPlaneNode::CreateHillPlaneNode(
		ISceneNode* parent, ISceneManager* mgr, s32 id,
			irr::core::vector3df position,
			irr::u32 basicSize,
			irr::core::dimension2d<irr::u32> size
			)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CHillPlaneNode* pNode = new CHillPlaneNode(parent, mgr, id);
		
		pNode->setPosition(position);
		pNode->setSize(basicSize, size);
		
		pNode->drop();

		return pNode;
	}

	irr::scene::jz3d::CHillPlaneNode* CHillPlaneNode::CreateHillPlaneNode(
		ISceneNode* parent, ISceneManager* mgr, s32 id,
		irr::core::vector3df position,
		irr::core::dimension2d<irr::f32> size
		)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CHillPlaneNode* pNode = new CHillPlaneNode(parent, mgr, id);

		pNode->setPosition(position);
		pNode->setSize(size);

		pNode->drop();

		return pNode;
	}

}
}
}