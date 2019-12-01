#pragma once

#include <irrlicht.h>

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			class CBulletPhysicsObjectNode : public ISceneNode
			{
			public:

				core::aabbox3d<f32> Box;

				irr::s32 m_WorldID;
				btDynamicsWorld *m_pWorld;
				//irr::scene::CBulletWorldAnimator *m_pWorldAnimator;
				irr::core::stringw m_strShape;
				irr::core::stringw m_strPhysicsObjectTypeName;

				bool m_bUseBulletAnimator; //수동삭제모드인가

				//물리객체
				std::tr1::shared_ptr<btCollisionShape> m_spColShape;
				std::tr1::shared_ptr<btDefaultMotionState>  m_spMotionState;			
				std::tr1::shared_ptr<btRigidBody> m_spRigidBody;

				CBulletObjectAnimatorGeometry m_GeometryInfo;
				CBulletObjectAnimatorParams m_PhysicsParam;
				

				static const int TypeID = MAKE_IRR_ID('@','B','P','O');
				static const char *Name;

				CBulletPhysicsObjectNode(ISceneNode* parent, ISceneManager* mgr, s32 id);				
				virtual ~CBulletPhysicsObjectNode();

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

				//!크기변환를 뺀 정보를 반환한다.
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
				static inline CBulletPhysicsObjectNode *upcast(irr::scene::ISceneNode *pnode)
				{
					if(pnode->getType() == TypeID)
					{
						return static_cast<CBulletPhysicsObjectNode *>(pnode);
					}

					return NULL;
				}

				//물리 애니메이터 등록
				bool register2BulletPhysicsWorld(btDynamicsWorld *pWorld);
			};
		}
	}
}
