#ifndef HILLPLANE_NODE_H
#define HILLPLANE_NODE_H

#include "irrlicht.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{

	class CHillPlaneNode : public IMeshSceneNode
	{
		irr::scene::IMesh* Mesh;
		irr::core::dimension2d<irr::u32> m_uSize;
		irr::u32 m_uBasicSize;
		
		irr::core::stringc getMeshName();
		irr::scene::IMesh* createHillPlaneMesh(irr::core::stringc meshName);
		irr::scene::IMesh* createHillPlaneMesh(irr::core::stringc meshName, irr::core::dimension2d<irr::f32>& size);

	public:

		irr::core::array<irr::video::SMaterial> m_Matrial;

		static const int TypeID = MAKE_IRR_ID('@','H','P','N');
		static const char *Name;

		CHillPlaneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const irr::core::vector3df& position = core::vector3df(0,0,0),
			const irr::core::vector3df& rotation = core::vector3df(0,0,0),
			const irr::core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			irr::u32 basicSize = 10.f,
			irr::core::dimension2d<irr::u32> size = irr::core::dimension2d<irr::u32>(100, 100)
			);

		virtual ~CHillPlaneNode();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		inline virtual video::SMaterial& getMaterial(u32 num)
		{
			return m_Matrial[num];
		}

		inline virtual u32 getMaterialCount() const
		{
			return m_Matrial.size();
		}

		//! This method is called just before the rendering process of the whole scene.
		virtual void OnRegisterSceneNode();

		virtual void render();				

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		// IMeshSceneNode 순수가상함수 구현
		virtual void setMesh(IMesh* mesh) {};
		virtual IMesh* getMesh(void) { return Mesh; }
		virtual void setReadOnlyMaterials(bool readonly) {}
		virtual bool isReadOnlyMaterials() const { return false; }


		static irr::scene::jz3d::CHillPlaneNode* CreateHillPlaneNode(
			ISceneNode* parent, ISceneManager* mgr, s32 id,
			irr::core::vector3df position,
			irr::u32 basicSize,
			irr::core::dimension2d<irr::u32> size
			);

		static irr::scene::jz3d::CHillPlaneNode* CreateHillPlaneNode(
			ISceneNode* parent, ISceneManager* mgr, s32 id,
			irr::core::vector3df position,
			irr::core::dimension2d<irr::f32> size
			);

		void setSize(irr::u32 basicSize, irr::core::dimension2d<irr::u32> size);
		
		void setSize(irr::core::dimension2d<irr::f32> size);
		
		void setTexture(irr::video::ITexture* texture);
	};

}
}
}

#endif