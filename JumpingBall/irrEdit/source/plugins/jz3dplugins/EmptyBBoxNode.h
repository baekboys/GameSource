#ifndef EMPTYBBOX_NODE_H
#define EMPTYBBOX_NODE_H

#include "irrlicht.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	
	class CEmptyBBoxNode : public irr::scene::ISceneNode
			{
				
				// 바운딩박스
				irr::core::aabbox3d<f32> m_Box;

				// 바운딩박스 사이즈
				irr::core::vector3df m_fSize;

			public:

				static const int TypeID = MAKE_IRR_ID('@','E','B','B');
				static const char *Name;

				CEmptyBBoxNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::f32 x = 10.f, irr::f32 y = 10.f, irr::f32 z = 10.f);

				virtual ~CEmptyBBoxNode();

				//! returns the axis aligned bounding box of this node
				virtual const core::aabbox3d<f32>& getBoundingBox() const;

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

				void setSize(irr::core::vector3df size);

				// 자식노드 얻기
				irr::scene::ISceneNode* getFirstChild();

				static irr::scene::jz3d::CEmptyBBoxNode* CreateEmptyBBoxNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df position, irr::core::vector3df size);
			};

}
}
}

#endif