#pragma once

#include <irrlicht.h>

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			class CBulletPhysicsObjPluginNode : public ISceneNode
			{
			public:

				static const c8* StatusLsit[];

				core::aabbox3d<f32> Box;

				CBulletObjectAnimatorGeometry m_GeometryInfo;
				CBulletObjectAnimatorParams m_PhysicsParam;
				
				irr::core::stringc m_strWorldName;
				irr::core::stringw m_strShape;
				irr::core::stringw m_strPhysicsObjectTypeName;
				irr::core::stringc m_strStatus;

				//CBulletObjectAnimator *m_pAnimator;
				

				static const int TypeID = MAKE_IRR_ID('@','B','P','*');
				static const char *Name;

				CBulletPhysicsObjPluginNode(ISceneNode* parent, ISceneManager* mgr, s32 id);
				virtual ~CBulletPhysicsObjPluginNode();

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

				static inline CBulletPhysicsObjPluginNode *upcast(irr::scene::ISceneNode *pnode)
				{
					if(pnode->getType() == TypeID)
					{
						return static_cast<CBulletPhysicsObjPluginNode *>(pnode);
					}

					return NULL;
				}

				inline irr::core::matrix4 getCollisionShapeTransform()
				{
					irr::core::matrix4 mat;

					irr::core::matrix4 mat_j = getAbsoluteTransformation();
					irr::core::matrix4 mat_t,mat_r;

					mat_t.setTranslation(mat_j.getTranslation());
					mat_r.setRotationDegrees(mat_j.getRotationDegrees());	
					mat = mat_t * mat_r;

					return mat;
				}	

				bool RegisterPhysics(const c8 *worldName=NULL);				
			};
		}
	}
}
