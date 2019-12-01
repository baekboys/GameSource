#pragma once

#include <irrlicht.h>

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			class CABVSceneNode : public ISceneNode
			{
			public:
				core::aabbox3d<f32> Box;
				irr::scene::ICameraSceneNode *m_pCamera;

				irr::core::stringc m_strCamName;

				static const int TypeID = MAKE_IRR_ID('@','C','A','B');
				static const char *Name;

				CABVSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id);
				virtual ~CABVSceneNode();

				//! returns the axis aligned bounding box of this node
				virtual const core::aabbox3d<f32>& getBoundingBox() const;

				//! This method is called just before the rendering process of the whole scene.
				virtual void OnRegisterSceneNode();

				virtual void OnAnimate(u32 timeMs);
				
				virtual void render();				

				//! Returns type of the scene node
				virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

				//! Creates a clone of this scene node and its children.
				virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

				//! Writes attributes of the scene node.
				virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

				//! Reads attributes of the scene node.
				virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);			

				bool isCulled();

				void setCameraToCulling()
				{
					irr::scene::ICameraSceneNode *pCam = 
						(irr::scene::ICameraSceneNode *)SceneManager->getSceneNodeFromName("m_strCamName");

					if(pCam) 
						m_pCamera = pCam;
				}

			};
		}
	}
}
