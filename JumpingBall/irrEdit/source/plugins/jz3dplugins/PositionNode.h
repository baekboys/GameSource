#ifndef POSITION_NODE_H
#define POSITION_NODE_H

#include "irrlicht.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{

	class CPositionNode : public IMeshSceneNode
	{
		irr::scene::IMesh* Mesh;
		irr::f32 m_fSize;
		bool m_bIsStartNode;
		irr::core::array<irr::video::SMaterial> m_Matrial;

		irr::scene::IMesh* createArrowMesh(irr::core::stringc meshName);
		irr::core::stringc getMeshName();

	public:

		static const int TypeID = MAKE_IRR_ID('@','P','O','S');
		static const char *Name;

		CPositionNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const irr::core::vector3df& position = core::vector3df(0,0,0),
			const irr::core::vector3df& rotation = core::vector3df(0,0,0),
			const irr::core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			bool start = true,
			irr::f32 size = 10.f
			);

		virtual ~CPositionNode();

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


		static irr::scene::jz3d::CPositionNode* CreatePositionNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df position,  bool startFlag);

		void setIsStart(bool start);
		void setSize(irr::f32 size);
	};

}
}
}

#endif