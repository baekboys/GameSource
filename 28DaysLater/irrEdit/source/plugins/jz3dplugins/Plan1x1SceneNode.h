#pragma once
#include <irrlicht.h>

namespace irr {
	namespace scene
	{
		namespace jz3d
		{
			class CPlan1x1SceneNode : public scene::IMeshSceneNode
			{
			public:
				CPlan1x1SceneNode(
					ISceneNode* parent, ISceneManager* mgr, s32 id,
					const core::vector3df& position = core::vector3df(0,0,0),
					const core::vector3df& rotation = core::vector3df(0,0,0),
					const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
				);
				virtual ~CPlan1x1SceneNode(void);



				static const int TypeID = MAKE_IRR_ID('@','p','l','n');

				virtual void OnRegisterSceneNode();

				//! renders the node.
				virtual void render();

				//! returns the axis aligned bounding box of this node
				virtual const core::aabbox3d<f32>& getBoundingBox() const;

				//! returns the material based on the zero based index i. To get the amount
				//! of materials used by this scene node, use getMaterialCount().
				//! This function is needed for inserting the node into the scene hirachy on a
				//! optimal position for minimizing renderstate changes, but can also be used
				//! to directly modify the material of a scene node.
				virtual video::SMaterial& getMaterial(u32 i);

				//! returns amount of materials used by this scene node.
				virtual u32 getMaterialCount() const;

				//! Returns type of the scene node
				virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID;}//ESNT_CUBE; }

				//! Writes attributes of the scene node.
				virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

				//! Reads attributes of the scene node.
				virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

				//! Creates a clone of this scene node and its children.
				virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

				//! Sets a new mesh to display
				virtual void setMesh(IMesh* mesh);

				//! Returns the current mesh
				virtual IMesh* getMesh(void) { return Mesh; }

				//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
				/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
				referencing this mesh to change too. */
				virtual void setReadOnlyMaterials(bool readonly) {}

				//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
				virtual bool isReadOnlyMaterials() const { return false; }

			private:
				//void setSize();

				IMesh *Mesh;
				core::array<video::SMaterial> Materials;				
				core::aabbox3d<f32> Box;
				video::SMaterial tmpReadOnlyMaterial;	

				s32 PassCount;
				bool ReadOnlyMaterials;

				void copyMaterials();
			};
		}
	}
}
