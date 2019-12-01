#include "PortalNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	const char *CPortalNode::Name  = "CPortalNode";	
	
	CPortalNode::CPortalNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position, const core::dimension2d<f32>& size,
		video::SColor colorTop, video::SColor colorBottom,
		PORTAL_TYPE type
		)
		: IBillboardSceneNode(parent, mgr, id, position), /*isInPortal(inPortal),*/ m_uPortalType(type)
	{
#ifdef _DEBUG
		setDebugName("CPortalNode");
#endif

		setSize(size);

		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		vertices[0].TCoords.set(1.0f, 1.0f);
		vertices[0].Color = colorBottom;

		vertices[1].TCoords.set(1.0f, 0.0f);
		vertices[1].Color = colorTop;

		vertices[2].TCoords.set(0.0f, 0.0f);
		vertices[2].Color = colorTop;

		vertices[3].TCoords.set(0.0f, 1.0f);
		vertices[3].Color = colorBottom;

		// 비디오 드라이버 얻기
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();

		// 매트리얼 설정
		{
			//Material.MaterialType = irr::video::EMT_TRANSPARENT_ADD_COLOR;
			Material.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
			Material.Lighting = false;
		}

		// 입구포탈인지 출구포탈인지 결정
		{
			//setIsInPortal(inPortal);
		}
	}


	//! pre render event
	void CPortalNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	//! render
	void CPortalNode::render()
	{
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		ICameraSceneNode* camera = SceneManager->getActiveCamera();

		if (!camera || !driver)
			return;

		// make billboard look to camera

		core::vector3df pos = getAbsolutePosition();

		core::vector3df campos = camera->getAbsolutePosition();
		core::vector3df target = camera->getTarget();
		core::vector3df up = camera->getUpVector();
		core::vector3df view = target - campos;
		view.normalize();

		core::vector3df horizontal = up.crossProduct(view);
		if ( horizontal.getLength() == 0 )
		{
			horizontal.set(up.Y,up.X,up.Z);
		}
		horizontal.normalize();
		horizontal *= 0.5f * Size.Width;

		core::vector3df vertical = horizontal.crossProduct(view);
		vertical.normalize();
		vertical *= 0.5f * Size.Height;

		view *= -1.0f;

		for (s32 i=0; i<4; ++i)
			vertices[i].Normal = view;

		vertices[0].Pos = pos + horizontal + vertical;
		vertices[1].Pos = pos + horizontal - vertical;
		vertices[2].Pos = pos - horizontal - vertical;
		vertices[3].Pos = pos - horizontal + vertical;

		// draw

		if ( DebugDataVisible & scene::EDS_BBOX )
		{
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			driver->draw3DBox(BBox, video::SColor(0,208,195,152));
		}

		//driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
		irr::core::matrix4 mat;
		mat.makeIdentity();
		driver->setTransform(video::ETS_WORLD, mat);

		driver->setMaterial(Material);

		driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	}


	//! returns the axis aligned bounding box of this node
	const core::aabbox3d<f32>& CPortalNode::getBoundingBox() const
	{
		return BBox;
	}


	//! sets the size of the billboard
	void CPortalNode::setSize(const core::dimension2d<f32>& size)
	{
		Size = size;

		if (Size.Width == 0.0f)
			Size.Width = 1.0f;

		if (Size.Height == 0.0f )
			Size.Height = 1.0f;

		f32 avg = (size.Width + size.Height)/6;
		BBox.MinEdge.set(-avg,-avg,-avg);
		BBox.MaxEdge.set(avg,avg,avg);
	}


	video::SMaterial& CPortalNode::getMaterial(u32 i)
	{
		return Material;
	}


	//! returns amount of materials used by this scene node.
	u32 CPortalNode::getMaterialCount() const
	{
		return 1;
	}


	//! gets the size of the billboard
	const core::dimension2d<f32>& CPortalNode::getSize() const
	{
		return Size;
	}


	//! Writes attributes of the scene node.
	void CPortalNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		IBillboardSceneNode::serializeAttributes(out, options);

		out->addFloat("Width", Size.Width);
		out->addFloat("Height", Size.Height);
		out->addColor ("Shade_Top", vertices[1].Color );
		out->addColor ("Shade_Down", vertices[0].Color );
		//out->addBool("InPortal", isInPortal);

		out->addInt("Portal type", m_uPortalType);
	}


	//! Reads attributes of the scene node.
	void CPortalNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		IBillboardSceneNode::deserializeAttributes(in, options);

		Size.Width = in->getAttributeAsFloat("Width");
		Size.Height = in->getAttributeAsFloat("Height");
		vertices[1].Color = in->getAttributeAsColor ( "Shade_Top" );
		vertices[0].Color = in->getAttributeAsColor ( "Shade_Down" );
		//isInPortal = in->getAttributeAsBool("InPortal");

		setSize(Size);
		//setIsInPortal(isInPortal);

		setPortalType(in->getAttributeAsInt("Portal type"));
	}


	//! Set the color of all vertices of the billboard
	//! \param overallColor: the color to set
	void CPortalNode::setColor(const video::SColor & overallColor)
	{
		for(u32 vertex = 0; vertex < 4; ++vertex)
			vertices[vertex].Color = overallColor;
	}


	//! Set the color of the top and bottom vertices of the billboard
	//! \param topColor: the color to set the top vertices
	//! \param bottomColor: the color to set the bottom vertices
	void CPortalNode::setColor(const video::SColor & topColor, const video::SColor & bottomColor)
	{
		vertices[0].Color = bottomColor;
		vertices[1].Color = topColor;
		vertices[2].Color = topColor;
		vertices[3].Color = bottomColor;
	}


	//! Gets the color of the top and bottom vertices of the billboard
	//! \param[out] topColor: stores the color of the top vertices
	//! \param[out] bottomColor: stores the color of the bottom vertices
	void CPortalNode::getColor(video::SColor & topColor, video::SColor & bottomColor) const
	{
		bottomColor = vertices[0].Color;
		topColor = vertices[1].Color;
	}


	//! Creates a clone of this scene node and its children.
	ISceneNode* CPortalNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent)
			newParent = Parent;
		if (!newManager)
			newManager = SceneManager;

		CPortalNode* nb = new CPortalNode(newParent, 
			newManager, ID, RelativeTranslation, Size);

		nb->cloneMembers(this, newManager);
		nb->Material = Material;

		nb->setPortalType(m_uPortalType);

		nb->drop();
		return nb;
	}

	void CPortalNode::setPortalType(irr::s32 type)
	{
		m_uPortalType = static_cast<irr::u32>(type);

		irr::core::stringc fileName;

		switch(m_uPortalType)
		{
		case PORTAL_IN :
			{
				this->setName("InPortal");
				fileName = "texture/portalin";
			}
			break;

		case PORTAL_OUT :
			{
				this->setName("OutPortal");
				fileName = "texture/portalout";
			}
			break;

		case PORTAL_END :
			{
				this->setName("EndPortal");
				fileName = "texture/portalend";
			}
			break;
		}

		// 기존의 텍스쳐애니메이터 삭제
		this->removeAnimators();

		// 텍스쳐 애니메이션 추가
		irr::scene::ISceneNodeAnimator* pTextureAnim;
		{
			irr::core::array<video::ITexture*> textures;
			for (s32 g=1; g<8; ++g)
			{
				irr::core::stringc portalName = fileName;
				portalName += g;
				portalName += ".png";
				irr::video::ITexture* t = SceneManager->getVideoDriver()->getTexture( portalName );
				textures.push_back(t);
			}

			pTextureAnim = SceneManager->createTextureAnimator(textures, 100);

			this->addAnimator(pTextureAnim);
		}
		pTextureAnim->drop();
	}

	irr::scene::jz3d::CPortalNode* CPortalNode::CreatePortalNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::dimension2df size, irr::core::vector3df position, /*bool inPortal,*/ PORTAL_TYPE type)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CPortalNode* pNode = new CPortalNode(parent, mgr, id);
		
		position.Y = 0;

		pNode->setSize(size);
		pNode->setPosition(position);
		pNode->setPortalType(type);

		pNode->drop();

		return pNode;
	}
}
}
}