#include "jz3d.h"

#include "CABVSceneNode.h"

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const char *CABVSceneNode::Name  = "CABVSceneNode";

			CABVSceneNode::CABVSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id), m_strCamName("camera name")
			{
#ifdef _DEBUG
				setDebugName(CABVSceneNode::Name);
#endif			
				m_pCamera = NULL;
			}

			CABVSceneNode::~CABVSceneNode()
			{
			}			

			//! pre render event
			void CABVSceneNode::OnRegisterSceneNode()
			{						
				bool bCull;

				setAutomaticCulling(irr::scene::EAC_BOX);				
				m_pCamera = (irr::scene::ICameraSceneNode *)SceneManager->getSceneNodeFromName(m_strCamName.c_str());

				if(m_pCamera)
				{					
					irr::scene::ICameraSceneNode *pOldCamera;
					pOldCamera = SceneManager->getActiveCamera();
					SceneManager->setActiveCamera(m_pCamera);
					bCull = isCulled();
					SceneManager->setActiveCamera(pOldCamera);					
				}
				else
				{
					bCull = isCulled();
				}				

				if( bCull )
				{

				}
				else
				{
					if (IsVisible)    
						SceneManager->registerNodeForRendering(this);
					ISceneNode::OnRegisterSceneNode();
				}
			}

			void CABVSceneNode::OnAnimate(irr::u32 timeMs)
			{
				ISceneNode::OnAnimate(timeMs);
			}

			//! render
			void CABVSceneNode::render()
			{
				if ( DebugDataVisible )
				{			
					video::IVideoDriver* driver = SceneManager->getVideoDriver();

					//재질설정 
					{
						irr::video::SMaterial m;
						m.Lighting = false;
						m.ZWriteEnable = true;																	
						driver->setMaterial(m);
					}

					irr::video::SColor draw_color(255,255,0,0);						

					driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

					driver->draw3DBox(Box, draw_color);
				}
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CABVSceneNode::getBoundingBox() const
			{
				return Box;
			}

			//! Creates a clone of this scene node and its children.
			ISceneNode* CABVSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CABVSceneNode* nb = new CABVSceneNode(newParent, newManager, ID);

				nb->cloneMembers(this, newManager);
				nb->Box = Box;
				nb->m_strCamName = m_strCamName;

				nb->drop();
				return nb;
			}		


			void CABVSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);
				//데이터 쓰기 코드추가

				out->addString("CullingCamName", m_strCamName.c_str());
				out->addVector3d("BoxVolume",Box.getExtent());
			}

			//! Reads attributes of the scene node.
			void CABVSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				m_strCamName = in->getAttributeAsString("CullingCamName");		
				irr::core::vector3df size = in->getAttributeAsVector3d("BoxVolume") / 2;
				Box.reset( core::aabbox3df( -size, size ) );

				//데이터 읽기 코드추가
				ISceneNode::deserializeAttributes(in, options);
			}

			bool CABVSceneNode::isCulled()
			{
				const ICameraSceneNode* cam = SceneManager->getActiveCamera();
				if (!cam)
				{
					_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
					return false;
				}

				switch ( this->getAutomaticCulling() )
				{
					// can be seen by a bounding box ?
				case scene::EAC_BOX:
					{
						core::aabbox3d<f32> tbox = this->getBoundingBox();
						this->getAbsoluteTransformation().transformBoxEx(tbox);
						_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
						return !(tbox.intersectsWithBox(cam->getViewFrustum()->getBoundingBox() ));
					}

					// can be seen by a bounding sphere
				case scene::EAC_FRUSTUM_SPHERE:
					{ // requires bbox diameter
					}
					break;

					// can be seen by cam pyramid planes ?
				case scene::EAC_FRUSTUM_BOX:
					{
						SViewFrustum frust = *cam->getViewFrustum();

						//transform the frustum to the node's current absolute transformation
						core::matrix4 invTrans(this->getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE);
						//invTrans.makeInverse();
						frust.transform(invTrans);

						core::vector3df edges[8];
						this->getBoundingBox().getEdges(edges);

						for (s32 i=0; i<scene::SViewFrustum::VF_PLANE_COUNT; ++i)
						{
							bool boxInFrustum=false;
							for (u32 j=0; j<8; ++j)
							{
								if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT)
								{
									boxInFrustum=true;
									break;
								}
							}

							if (!boxInFrustum)
								return true;
						}
					}
					break;

				case scene::EAC_OFF:
					break;
				}

				_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
				return false;
			}		
		}//namespace jz3d end
	}// namespace scene end
} //namespace irr end
