#include "EmptyPluginNode.h"

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const char *CEmptyPluginNode::Name  = "CEmptyPluginNode";

			CEmptyPluginNode::CEmptyPluginNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id)
			{
#ifdef _DEBUG
				setDebugName(CEmptyPluginNode::Name);
#endif
				setAutomaticCulling(scene::EAC_OFF);
			}
			

			//! pre render event
			void CEmptyPluginNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);

				ISceneNode::OnRegisterSceneNode();
			}


			//! render
			void CEmptyPluginNode::render()
			{
				// do nothing
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CEmptyPluginNode::getBoundingBox() const
			{
				return Box;
			}

			//! Creates a clone of this scene node and its children.
			ISceneNode* CEmptyPluginNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CEmptyPluginNode* nb = new CEmptyPluginNode(newParent, 
					newManager, ID);

				nb->cloneMembers(this, newManager);
				nb->Box = Box;

				nb->drop();
				return nb;
			}		


			void CEmptyPluginNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);		
				
				//데이터 쓰기 코드추가
				//...

			}

			//! Reads attributes of the scene node.
			void CEmptyPluginNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				//데이터 읽기 코드추가
				//...

				ISceneNode::deserializeAttributes(in, options);				
			}			
		}
	}
}
