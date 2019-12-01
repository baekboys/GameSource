#pragma once

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			class CCollusionMngNode : public ISceneNode
			{
				core::aabbox3d<f32> Box;
				irr::core::stringw m_strBone;
				irr::core::stringw m_strShape;
				
				irr::core::matrix4 m_matJoint;
				std::tr1::shared_ptr<btCollisionShape> m_spColShape;
				CBulletObjectAnimatorGeometry m_GeometryInfo;

				//scene::CBulletObjectAnimator* m_pAnimator;

				bool m_bUseBulletAnimator; //수동삭제모드인가

			public:

				static const c8 *CollShapeNames[];				

				CBulletObjectAnimatorGeometry *getGeometryInfo(){return &m_GeometryInfo;}
				btCollisionShape *getCollisionShape() {return m_spColShape.get();}
				//scene::CBulletObjectAnimator *getAnimator() {return m_pAnimator;} 
				
				inline irr::core::matrix4 getCollisionShapeTransform()
				{
					irr::core::matrix4 mat;

					if(m_strBone != "")
					{
						//각도와 이동변환만 골라서 적용시켜준다.
						irr::core::matrix4 mat_j = m_matJoint * getRelativeTransformation();
						irr::core::matrix4 mat_t,mat_r;

						mat_t.setTranslation(mat_j.getTranslation());
						mat_r.setRotationDegrees(mat_j.getRotationDegrees());	
						mat = mat_t * mat_r;
					}
					else
					{	
						irr::core::matrix4 mat_j = getAbsoluteTransformation();
						irr::core::matrix4 mat_t,mat_r;

						mat_t.setTranslation(mat_j.getTranslation());
						mat_r.setRotationDegrees(mat_j.getRotationDegrees());	
						mat = mat_t * mat_r;
					}
					return mat;
				}				

				//irr::core::array<char *> m_BoneList;

				static const int TypeID = MAKE_IRR_ID('@','C','L','M');
				static const char *Name;

				CCollusionMngNode(ISceneNode* parent, ISceneManager* mgr, s32 id);	
				virtual ~CCollusionMngNode();

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

				virtual void OnAnimate(u32 timeMs);

				static inline CCollusionMngNode *upcast(irr::scene::ISceneNode *pnode)
				{
					if(pnode->getType() == TypeID)
					{
						return static_cast<CCollusionMngNode *>(pnode);
					}

					return NULL;
				}

				//물리 애니메이터 등록
				bool register2BulletPhysicsWorld(irr::s32 worldID);



				////! Updates the absolute position based on the relative and the parents position
				////본이 존재하면 본의 변환을 상속받는다.
				//inline virtual void updateAbsolutePosition()
				//{					
				//	if (Parent)
				//	{
				//		if(Parent->getType() == ESNT_ANIMATED_MESH)
				//		{
				//			irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;
				//			
				//			//irr::core::stringc strc =  m_strBone.c_str();
				//			//irr::scene::IBoneSceneNode *pBone = pNode->getJointNode(strc.c_str());

				//			irr::core::matrix4 jointmat;
				//			jointmat.makeIdentity();
				//			irr::core::stringc strc =  m_strBone.c_str();

				//			if(pNode->getMesh()->getMeshType() == irr::scene::EAMT_SKINNED)
				//			{
				//				irr::scene::ISkinnedMesh *pMesh = (irr::scene::ISkinnedMesh *)pNode->getMesh();								
				//				irr::scene::ISkinnedMesh::SJoint *pJoint = pMesh->getAllJoints()[pMesh->getJointNumber(strc.c_str())];
				//				jointmat = pJoint->GlobalAnimatedMatrix;								
				//			}
				//			else
				//			{
				//			
				//			}

				//			//if(pBone)
				//			{
				//				AbsoluteTransformation =
				//					//pBone->getAbsoluteTransformation() * getRelativeTransformation();
				//					jointmat * getRelativeTransformation();
				//			}
				//			/*else
				//			{
				//				AbsoluteTransformation =
				//					Parent->getAbsoluteTransformation() * getRelativeTransformation();
				//			}*/
				//		}
				//		else
				//		{
				//			AbsoluteTransformation =
				//				Parent->getAbsoluteTransformation() * getRelativeTransformation();
				//		}
				//	}
				//	else
				//		AbsoluteTransformation = getRelativeTransformation();
				//}
				
			};		
		}
	}
}
