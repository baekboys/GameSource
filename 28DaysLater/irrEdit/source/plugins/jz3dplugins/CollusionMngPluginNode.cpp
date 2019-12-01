#include "jz3d.h"
#include "CollusionMngPluginNode.h"



namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{

			const c8* CCollusionMngNode::CollShapeNames[] =			
			{
				"sphere",
				"box",
				"cylinder",
				"capsule",
				0
			};	

			const char *CCollusionMngNode::Name  = "CCollusionMngNode";

			CCollusionMngNode::CCollusionMngNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id),
				m_strBone(""),
				m_strShape(CollShapeNames[0])
				//m_matJoint(irr::core::IdentityMatrix)
			{
#ifdef _DEBUG
				setDebugName(CCollusionMngNode::Name);
#endif
				setAutomaticCulling(scene::EAC_OFF);
				m_matJoint.makeIdentity();

				
			}

			CCollusionMngNode::~CCollusionMngNode()
			{
				/*if(m_pWorld  && !m_bUseBulletAnimator)
				{
					
				}*/
			}
			

			//btTransform CCollusionMngNode::getWorldTransform()
			

			//! pre render event
			void CCollusionMngNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);

				ISceneNode::OnRegisterSceneNode();
			}

			void CCollusionMngNode::OnAnimate(irr::u32 timeMs)
			{
				if(IsVisible && Parent->getType() == irr::scene::ESNT_ANIMATED_MESH)
				{
					irr::scene::IAnimatedMeshSceneNode *pNode = (irr::scene::IAnimatedMeshSceneNode *)Parent;
					irr::core::stringc strc =  m_strBone.c_str();

					if(pNode && pNode->getMesh()->getMeshType() == irr::scene::EAMT_SKINNED)
					{
						//각기다른 클론의 스키닝값을 얻기위해서
						irr::scene::ISkinnedMesh *pMesh = (irr::scene::ISkinnedMesh *)pNode->getMesh();								
						int index = pMesh->getJointNumber(strc.c_str());
						if(index >= 0)
						{
							irr::scene::ISkinnedMesh::SJoint *pJoint = pMesh->getAllJoints()[index];
							pMesh->animateMesh(pNode->getFrameNr(),1.0f);
							pMesh->skinMesh();
							m_matJoint = Parent->getAbsoluteTransformation() * pJoint->GlobalAnimatedMatrix;								
						}
					}
					else
					{
						m_matJoint.makeIdentity();

					}
				}

				//기본 루틴 수행
				irr::scene::ISceneNode::OnAnimate(timeMs);

			}


			//! render
			void CCollusionMngNode::render()
			{				

				video::IVideoDriver* driver = SceneManager->getVideoDriver();

				if ( DebugDataVisible & scene::EDS_BBOX )
				{
					if(Parent)
					{
						//재질설정 
						{
							irr::video::SMaterial m;
							m.Lighting = false;
							m.ZWriteEnable = true;																	
							driver->setMaterial(m);
						}

						irr::video::SColor draw_color(255,255,0,0);


						//if(m_strBone == "")
						//{
						//	//각도와 이동변환만 골라서 적용시켜준다.
						//	irr::core::matrix4 mat_t,mat_r;
						//	mat_t.setTranslation(getAbsoluteTransformation().getTranslation());
						//	mat_r.setRotationDegrees(getAbsoluteTransformation().getRotationDegrees());

						//	driver->setTransform(video::ETS_WORLD,(mat_t * mat_r) );							
						//}
						//else if(Parent->getType() == ESNT_ANIMATED_MESH)
						//{
						//	irr::core::matrix4 mat = m_matJoint * getRelativeTransformation();
						//	irr::core::matrix4 mat_t,mat_r;

						//	mat_t.setTranslation(mat.getTranslation());
						//	mat_r.setRotationDegrees(mat.getRotationDegrees());							

						//	driver->setTransform(video::ETS_WORLD,(mat_t * mat_r) );							
						//}

						driver->setTransform(video::ETS_WORLD,getCollisionShapeTransform());

						{
							if(m_GeometryInfo.type == CBPAGT_BOX)
							{
								irr::core::vector3df box_size =
									irr::core::vector3df(m_GeometryInfo.box.X ,m_GeometryInfo.box.Y ,m_GeometryInfo.box.Z );

								irr::core::aabbox3df box(										
									-(box_size),
									(box_size)
									);									

								driver->draw3DBox(box,draw_color);
							}
							else if(m_GeometryInfo.type == CBPAGT_CAPSULE)
							{
								irr::core::quaternion qtY(0, 45*irr::core::DEGTORAD, 0);

								//btCapsuleShape* pCapsuleShape = (btCapsuleShape*)pShape;
								irr::video::IVideoDriver *pVideo = driver;

								btScalar radius =  m_GeometryInfo.Capsule.radius;//pCapsuleShape->getRadius();
								btScalar halfheight = m_GeometryInfo.Capsule.hight * .5f;//pCapsuleShape->getHalfHeight();

								/*{
									pVideo->draw3DLine(irr::core::vector3df(0,0,0), irr::core::vector3df(radius,0,0), irr::video::SColor(255,0,255,0));
									pVideo->draw3DLine(irr::core::vector3df(0,0,0), irr::core::vector3df(0,halfheight,0), irr::video::SColor(255,255,0,0));		
									pVideo->draw3DLine(irr::core::vector3df(0,0,0), irr::core::vector3df(0,0,radius), irr::video::SColor(255,0,0,255));
								}*/

								irr::core::vector3df pointUP( irr::core::vector3df(radius, halfheight, 0) );
								irr::core::vector3df pointBottom( irr::core::vector3df(radius, -halfheight, 0) );
								irr::core::vector3df rotAxis(0,0,1);

								for (int i=0; i<8; i++)
								{
									irr::core::vector3df pointUP2 = qtY*pointUP;

									//캡슐의 몸통부분(실린더) 그리기
									pVideo->draw3DLine(pointUP, pointUP2, draw_color); //윗선그리기
									irr::core::vector3df pointBottom2 = qtY*pointBottom;
									pVideo->draw3DLine(pointBottom, pointBottom2, draw_color); //아랫선 그리기
									pVideo->draw3DLine(pointUP, pointBottom, draw_color); //옆선그리기

									{
										//캡슐의 양쪽 둥근부분 그리기
										irr::core::vector3df pointS_UP = pointUP-irr::core::vector3df(0, halfheight, 0);
										irr::core::vector3df pointS_Bottom = pointBottom+irr::core::vector3df(0, halfheight, 0);
										irr::core::quaternion qtZ, qtZ_I;
										qtZ.fromAngleAxis(30*irr::core::DEGTORAD, rotAxis);
										qtZ_I.fromAngleAxis(-30*irr::core::DEGTORAD, rotAxis);
										for (int j=0; j<5; j++)
										{
											irr::core::vector3df pointS2 = qtZ*pointS_UP; //상단반구
											pVideo->draw3DLine(
												pointS_UP+irr::core::vector3df(0, halfheight, 0), 
												pointS2+irr::core::vector3df(0, halfheight, 0), 
												draw_color);
											pointS_UP = pointS2;

											irr::core::vector3df pointS3 = qtZ_I*pointS_Bottom; //하단반구
											pVideo->draw3DLine(
												pointS_Bottom-irr::core::vector3df(0, halfheight, 0), 
												pointS3-irr::core::vector3df(0, halfheight, 0), 
												draw_color);
											pointS_Bottom = pointS3;
										}
									}

									rotAxis =qtY*rotAxis;
									pointUP = pointUP2;
									pointBottom = pointBottom2;
								}
							}
						}
					}
				}				
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CCollusionMngNode::getBoundingBox() const
			{
				return Box;
			}

			//! Creates a clone of this scene node and its children.
			ISceneNode* CCollusionMngNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CCollusionMngNode* nb = new CCollusionMngNode(newParent,
					newManager, ID);

				//newParent->get

				nb->cloneMembers(this, newManager);
				nb->Box = Box;
				nb->m_strBone = m_strBone;
				nb->m_strShape = m_strShape;
				nb->m_GeometryInfo = m_GeometryInfo;
				nb->m_spColShape = m_spColShape;

				nb->m_bUseBulletAnimator = m_bUseBulletAnimator;


				nb->drop();
				return nb;
			}		


			void CCollusionMngNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);
				
				//데이터 쓰기 코드추가
				//...
				out->addString("bone",m_strBone.c_str());
				out->addEnum("Shape",
					irr::core::stringc(m_strShape.c_str()).c_str(),
					CCollusionMngNode::CollShapeNames
					);

				/*
				"sphere",
				"box",
				"cylinder",
				"capsule",
				*/
				if(m_strShape == L"sphere")
				{
					out->addFloat("radius",m_GeometryInfo.sphere.radius);
				}
				if(m_strShape == L"box")
				{
					out->addVector3d("box_size",irr::core::vector3df(m_GeometryInfo.box.X ,m_GeometryInfo.box.Y ,m_GeometryInfo.box.Z ));
				}
				else if(m_strShape == L"capsule")
				{
					out->addFloat("height",m_GeometryInfo.Capsule.hight);
					out->addFloat("radius",m_GeometryInfo.Capsule.radius);
				}

				//out->addEnum("bones",0,(char **)&m_BoneList[0]);
				/*out->add*/

			}

			//! Reads attributes of the scene node.
			void CCollusionMngNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				//데이터 읽기 코드추가
				//...

				m_strBone = in->getAttributeAsStringW("bone");
				m_strShape = irr::core::stringw(in->getAttributeAsEnumeration("Shape"));


				if(m_strShape == L"sphere")
				{
					m_GeometryInfo.sphere.radius = in->getAttributeAsFloat("radius");
					m_GeometryInfo.type = CBPAGT_SPHERE;
					m_spColShape = std::tr1::shared_ptr<btCollisionShape>(new btSphereShape(m_GeometryInfo.sphere.radius));
				}
				else if(m_strShape == L"box")
				{
					irr::core::vector3df box_size = in->getAttributeAsVector3d("box_size");
					m_GeometryInfo.box.X = box_size.X;
					m_GeometryInfo.box.Y = box_size.Y;
					m_GeometryInfo.box.Z = box_size.Z;
					m_GeometryInfo.type = CBPAGT_BOX;

					btVector3 btbox;

					Irrlicht2Bullet(box_size,btbox);

					m_spColShape = std::tr1::shared_ptr<btCollisionShape>(new btBoxShape(btbox));
				}
				else if(m_strShape == L"capsule")
				{
					m_GeometryInfo.Capsule.hight = in->getAttributeAsFloat("height");
					m_GeometryInfo.Capsule.radius = in->getAttributeAsFloat("radius");
					m_GeometryInfo.type = CBPAGT_CAPSULE;

					m_spColShape = std::tr1::shared_ptr<btCollisionShape>(new btCapsuleShape(m_GeometryInfo.Capsule.radius,
						m_GeometryInfo.Capsule.hight)
						);

				}

				ISceneNode::deserializeAttributes(in, options);				
			}			

			bool CCollusionMngNode::register2BulletPhysicsWorld(irr::s32 worldID)
			{				

				irr::s32 TypeID = ESNAT_BULLET_OBJECT;
				//씬노드 펙토리찾기
				irr::scene::CBulletAnimatorManager *ani_factory =
					static_cast<irr::scene::CBulletAnimatorManager *>(ggf::util::findAnimatorFactory(SceneManager,TypeID));				

				

				if(!ani_factory)
					return false;
				
				irr::scene::CBulletObjectAnimatorParams physicsParams;			
				
				physicsParams.mass = 0.0f;
				physicsParams.friction = .5f;
				physicsParams.restitution = 0.5f;

				scene::CBulletObjectAnimator* levelAnim = 
					ani_factory->createBulletObjectAnimator(
					SceneManager,
					this,
					worldID,					
					&m_GeometryInfo,					
					&physicsParams
					);


				//초기 생성시 로컬 변환만 적용 되므로 다시한번 월드위치를 세팅해준다.
				{
					levelAnim->setPosition(getCollisionShapeTransform().getTranslation());
					levelAnim->setRotation(getCollisionShapeTransform().getRotationDegrees() * irr::core::DEGTORAD);
				}

				this->addAnimator(levelAnim);
				levelAnim->drop();		
				return true;
			}
		}
	}
}
