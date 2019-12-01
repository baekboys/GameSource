#include "PositionNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{

	const char *CPositionNode::Name  = "CPositionNode";

	CPositionNode::CPositionNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		const irr::core::vector3df& position,
		const irr::core::vector3df& rotation,
		const irr::core::vector3df& scale,
		bool start,
		irr::f32 size
		)
		: IMeshSceneNode(parent,mgr,id, position, rotation, scale), Mesh(NULL), m_fSize(size), m_bIsStartNode(start)
	{
#ifdef _DEBUG
		setDebugName(CPositionNode::Name);
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
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
		}
		
		// 화살표 메쉬 만들기
		{
			Mesh = createArrowMesh(getMeshName().c_str());
			
			if(Mesh)
				Mesh->grab();
		}

		// 애니메이션 추가
		{
			// x축으로 90도 회전
			this->setRotation(irr::core::vector3df(270, 0,0));

			// 제자리 회전 애니메이터 생성 및 큐브신노드에 추가
			irr::scene::ISceneNodeAnimator *pRotAnim = SceneManager->createRotationAnimator(irr::core::vector3df(0,0,-0.5f));
			this->addAnimator(pRotAnim);
			pRotAnim->drop();
			
			// 상하로 움직이는 애니메이터 생성
			irr::core::array<irr::core::vector3df> vectorArray;
			irr::core::vector3df start(0,0,7);
			irr::core::vector3df end(0,0,5);
			vectorArray.push_back(start);
			vectorArray.push_back(end);
			irr::scene::ISceneNodeAnimator *pFlwSpinAnim = SceneManager->createFollowSplineAnimator(0, vectorArray, 1.0f, 0.5f);
			this->addAnimator(pFlwSpinAnim);
			pFlwSpinAnim->drop();
		}

		setIsStart(start);
	}

	CPositionNode::~CPositionNode()
	{
		if(Mesh)
			Mesh->drop();
	}

	irr::core::stringc CPositionNode::getMeshName()
	{
		irr::core::stringc name(m_fSize);
		name += "ArrowMesh";

		return name;
	}

	irr::scene::IMesh* CPositionNode::createArrowMesh(irr::core::stringc meshName)
	{
		irr::scene::IMesh* pMesh;

		if(SceneManager->getMeshCache()->isMeshLoaded(meshName.c_str()))
		{
			pMesh = SceneManager->getMesh(meshName.c_str());
		}
		else
		{
			pMesh = SceneManager->addArrowMesh(
				meshName.c_str(),
				irr::video::SColor(255,255,255,255),
				irr::video::SColor(255,255,255,255),
				4, 8, m_fSize, m_fSize * 0.6f, m_fSize * 0.3f, m_fSize * 0.5f
				);
		}

		return pMesh;
	}

	void CPositionNode::setSize(irr::f32 size)
	{
		if(m_fSize != size)
		{
			if(Mesh)
				Mesh->drop();

			m_fSize = size;

			Mesh = createArrowMesh(getMeshName());

			if(Mesh)
				Mesh->grab();
		}
	}

	void CPositionNode::setIsStart(bool start)
	{
		m_bIsStartNode = start;

		// 텍스쳐 설정
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		if(m_bIsStartNode)
		{
			irr::video::ITexture* texture = driver->getTexture("texture/startarrow.png");
			this->getMaterial(0).setTexture(0, texture);
			this->getMaterial(1).setTexture(0, texture);
			this->setName("StartMeshNode");
		}
		else
		{
			irr::video::ITexture* texture = driver->getTexture("texture/endarrow.png");
			this->getMaterial(0).setTexture(0, texture);
			this->getMaterial(1).setTexture(0, texture);
			this->setName("EndMeshNode");
		}
	}

	void CPositionNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	void CPositionNode::render()
	{
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		driver->setMaterial(getMaterial(0));
		driver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
		
		// 매쉬버퍼로 그리기

		irr::u32 i;
		for(i = 0 ; i < Mesh->getMeshBufferCount() ; ++i)
			driver->drawMeshBuffer(Mesh->getMeshBuffer(i));

		
		if(DebugDataVisible)
		{
			driver->draw3DBox(Mesh->getBoundingBox(), irr::video::SColor(255,255,0,0));
		}
	}


	const core::aabbox3d<f32>& CPositionNode::getBoundingBox() const
	{
		return Mesh->getBoundingBox();
	}


	ISceneNode* CPositionNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent) newParent = Parent;
		if (!newManager) newManager = SceneManager;

		CPositionNode* nb = new CPositionNode(newParent, 
			newManager, ID);

		nb->cloneMembers(this, newManager);

		nb->m_bIsStartNode = m_bIsStartNode;
		nb->m_Matrial = m_Matrial;
		
		nb->setSize(m_fSize);

		nb->drop();
		return nb;
	}		


	void CPositionNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		ISceneNode::serializeAttributes(out, options);		

		//데이터 쓰기 코드추가
		//...
		out->addFloat("Arrow size", m_fSize);
		out->addBool("Start node", m_bIsStartNode);
	}

	//! Reads attributes of the scene node.
	void CPositionNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{	
		//데이터 읽기 코드추가
		//...
		setSize(in->getAttributeAsFloat("Arrow size"));
		setIsStart(in->getAttributeAsBool("Start node"));
		
		ISceneNode::deserializeAttributes(in, options);				
	}

	irr::scene::jz3d::CPositionNode* CPositionNode::CreatePositionNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df position, bool startFlag)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CPositionNode* pNode = new CPositionNode(parent, mgr, id);
		
		position.Y = 0;

		pNode->setPosition(position);
		pNode->setIsStart(startFlag);

		pNode->drop();

		return pNode;
	}

}
}
}