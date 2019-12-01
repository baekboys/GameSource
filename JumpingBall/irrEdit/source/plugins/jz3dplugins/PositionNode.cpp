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

		// ���� ����̹� ������ ���
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();

		// ��Ʈ���� ����
		{
			// ��Ʈ���� ����
			irr::video::SMaterial m;
			m.Lighting = false;
			m.BackfaceCulling = false;
			
			irr::video::SMaterial mt;

			// 0�� ��Ʈ������ m, �������� mt�� �ʱ�ȭ
			m_Matrial.push_back(m);
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
		}
		
		// ȭ��ǥ �޽� �����
		{
			Mesh = createArrowMesh(getMeshName().c_str());
			
			if(Mesh)
				Mesh->grab();
		}

		// �ִϸ��̼� �߰�
		{
			// x������ 90�� ȸ��
			this->setRotation(irr::core::vector3df(270, 0,0));

			// ���ڸ� ȸ�� �ִϸ����� ���� �� ť��ų�忡 �߰�
			irr::scene::ISceneNodeAnimator *pRotAnim = SceneManager->createRotationAnimator(irr::core::vector3df(0,0,-0.5f));
			this->addAnimator(pRotAnim);
			pRotAnim->drop();
			
			// ���Ϸ� �����̴� �ִϸ����� ����
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

		// �ؽ��� ����
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
		
		// �Ž����۷� �׸���

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

		//������ ���� �ڵ��߰�
		//...
		out->addFloat("Arrow size", m_fSize);
		out->addBool("Start node", m_bIsStartNode);
	}

	//! Reads attributes of the scene node.
	void CPositionNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{	
		//������ �б� �ڵ��߰�
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