#pragma once

#include <irrlicht.h>

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			class CBulletPhysicsWorldPluginNode : public ISceneNode
			{
				static const c8* CmdLsit[];

				irr::u32 m_uLastTick;

			public:

				core::aabbox3d<f32> Box;

				static const int TypeID = MAKE_IRR_ID('@','B','P','W');
				static const char *Name;

				CBulletPhysicsWorldPluginNode(ISceneNode* parent, ISceneManager* mgr, s32 id);				
				//~CBulletPhysicsWorldPluginNode();

				//! returns the axis aligned bounding box of this node
				virtual const core::aabbox3d<f32>& getBoundingBox() const;

				//! This method is called just before the rendering process of the whole scene.
				virtual void OnRegisterSceneNode();
				
				virtual void render();			

				virtual void OnAnimate(u32 timeMs);

				//! Returns type of the scene node
				virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

				//! Creates a clone of this scene node and its children.
				virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

				//! Writes attributes of the scene node.
				virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

				//! Reads attributes of the scene node.
				virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

				CBulletWorldAnimator *registerDynamics(ISceneNode *pNode);				

				static inline CBulletPhysicsWorldPluginNode *upcast(irr::scene::ISceneNode *pnode)
				{
					if(pnode->getType() == TypeID)
					{
						return static_cast<CBulletPhysicsWorldPluginNode *>(pnode);
					}

					return NULL;
				}

				void start_Children();
				void stop_Children();
				


				//월드정보
				CBulletWorldAnimator::Params m_WorldParam;
				irr::core::vector3df m_WorldAxiesMax;
				irr::core::vector3df m_WorldAxiesMin;
				
				//스탭관리
				int m_maxSubStep;
				f32 m_fixedTimeStep;

				CBulletWorldAnimator *m_pWorldAnimator;

				core::stringc m_strCmd;


			};
		}
	}
}
