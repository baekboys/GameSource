#ifndef PORTAL_NODE_H
#define PORTAL_NODE_H

#include "irrlicht.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	enum PORTAL_TYPE
	{
		PORTAL_IN,
		PORTAL_OUT,
		PORTAL_END
	};


	class CPortalNode : public IBillboardSceneNode
	{
	public:

		static const int TypeID = MAKE_IRR_ID('@','P','O','T');
		static const char *Name;

		//! constructor
	CPortalNode(ISceneNode* parent, ISceneManager* mgr, s32 id,	
		const core::vector3df& position = irr::core::vector3df(0,0,0), 
		const core::dimension2d<f32>& size = irr::core::dimension2df(10,10),
		video::SColor colorTop=video::SColor(0xFFFFFFFF),video::SColor colorBottom=video::SColor(0xFFFFFFFF),
		PORTAL_TYPE type = PORTAL_IN
		);

	//! pre render event
	virtual void OnRegisterSceneNode();

	//! render
	virtual void render();

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! sets the size of the billboard
	virtual void setSize(const core::dimension2d<f32>& size);

	//! gets the size of the billboard
	virtual const core::dimension2d<f32>& getSize() const;

	virtual video::SMaterial& getMaterial(u32 i);
	
	//! returns amount of materials used by this scene node.
	virtual u32 getMaterialCount() const;
	
	//! Set the color of all vertices of the billboard
	//! \param overallColor: the color to set
	virtual void setColor(const video::SColor & overallColor);

	//! Set the color of the top and bottom vertices of the billboard
	//! \param topColor: the color to set the top vertices
	//! \param bottomColor: the color to set the bottom vertices
	virtual void setColor(const video::SColor & topColor, const video::SColor & bottomColor);

	//! Gets the color of the top and bottom vertices of the billboard
	//! \param[out] topColor: stores the color of the top vertices
	//! \param[out] bottomColor: stores the color of the bottom vertices
	virtual void getColor(video::SColor& topColor, video::SColor& bottomColor) const;

	//! Writes attributes of the scene node.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the scene node.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

	//! Creates a clone of this scene node and its children.
	virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

	void setPortalType(irr::s32 type);
	
	static irr::scene::jz3d::CPortalNode* CreatePortalNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::dimension2df size, irr::core::vector3df position, /*bool inPortal,*/ PORTAL_TYPE type);

private:

	core::dimension2d<f32> Size;
	core::aabbox3d<f32> BBox;
	video::SMaterial Material;

	video::S3DVertex vertices[4];
	u16 indices[6];
	
	irr::u32 m_uPortalType;
	};

}
}
}

#endif