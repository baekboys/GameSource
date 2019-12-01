#include "TiledPlaneNode.h"

//타일드 플래인 씬노드
//작성 : 2009.8.18 gbox3d
//v0.001 2009.8.19,속성 재설정시 이전 메트 리얼 보존하도록 수정
//v0.002 2009.8.25, 메쉬 참조 카운터오류오인한 메모리릭 수정

namespace irr 
{
	namespace scene
	{
		namespace jz3d
		{
			const char *CTiledPlaneNode::Name = "TiledPlaneNode";
			const char *MeshNameFormat = "./usr/mesh/tiledplanenodec%d_%ds%f_%f";
			//const char *MeshNameFormat = "../res/usr/mesh/tiledplanenode";

			CTiledPlaneNode::CTiledPlaneNode(ISceneNode* parent, ISceneManager* mgr,s32 id,	const core::vector3df& position,const core::vector3df& rotation, const core::vector3df& scale)
				: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
				Mesh(0), 
				PassCount(0),
				ReadOnlyMaterials(false),
				m_TileCount(irr::core::dimension2d<irr::u32>(1,1)),
				m_TileSize(irr::core::dimension2d<irr::f32>(10,10)),
				m_bLightMapGen_Enable(false)
				

			{	
				#ifdef _DEBUG
					setDebugName("CTiledPlaneNode");
				#endif
				
					setMesh(setupMesh(""));

				////m_nTempType = ESNT_MESH;

				//irr::scene::IAnimatedMesh* pMesh; 		

				//if(SceneManager->getMeshCache()->isMeshLoaded("jz3d/scene/mesh/plane1x1") == false)
				//{
				//	pMesh = SceneManager->addHillPlaneMesh("jz3d/scene/mesh/plane1x1",
				//		irr::core::dimension2d<irr::f32>(10,10),
				//		irr::core::dimension2d<irr::u32>(1,1));
				//}
				//else
				//{
				//	pMesh = SceneManager->getMesh("jz3d/scene/mesh/plane1x1");
				//}
				//
				////Mesh = pMesh;
				//

				//setMesh(pMesh);
			}

			CTiledPlaneNode::~CTiledPlaneNode(void)
			{
				if (Mesh)
					Mesh->drop();
			}			

			void CTiledPlaneNode::OnRegisterSceneNode()
			{
				if (IsVisible)
				{
					// because this node supports rendering of mixed mode meshes consisting of 
					// transparent and solid material at the same time, we need to go through all 
					// materials, check of what type they are and register this node for the right
					// render pass according to that.

					video::IVideoDriver* driver = SceneManager->getVideoDriver();

					PassCount = 0;
					int transparentCount = 0;
					int solidCount = 0;

					// count transparent and solid materials in this scene node
					if (ReadOnlyMaterials && Mesh)
					{
						// count mesh materials 

						for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
						{
							scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
							video::IMaterialRenderer* rnd = mb ? driver->getMaterialRenderer(mb->getMaterial().MaterialType) : 0;

							if (rnd && rnd->isTransparent()) 
								++transparentCount;
							else 
								++solidCount;

							if (solidCount && transparentCount)
								break;
						}
					}
					else
					{
						// count copied materials 

						for (u32 i=0; i<Materials.size(); ++i)
						{
							video::IMaterialRenderer* rnd = 
								driver->getMaterialRenderer(Materials[i].MaterialType);

							if (rnd && rnd->isTransparent()) 
								++transparentCount;
							else 
								++solidCount;

							if (solidCount && transparentCount)
								break;
						}	
					}

					// register according to material types counted

					if (solidCount)
						SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);

					if (transparentCount)
						SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);

					ISceneNode::OnRegisterSceneNode();
				}
			}



			//! renders the node.
			void CTiledPlaneNode::render()
			{
				video::IVideoDriver* driver = SceneManager->getVideoDriver();

				if (!Mesh || !driver)
					return;

				bool isTransparentPass = 
					SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

				++PassCount;

				driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
				Box = Mesh->getBoundingBox();

				// for debug purposes only:

				bool renderMeshes = true;
				video::SMaterial mat;
				if (DebugDataVisible && PassCount==1)
				{
					// overwrite half transparency
					if ( DebugDataVisible & scene::EDS_HALF_TRANSPARENCY )
					{
						for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
						{
							mat = Materials[g];
							mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
							driver->setMaterial(mat);
							driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
						}
						renderMeshes = false;
					}
				}

				// render original meshes
				if ( renderMeshes )
				{
					for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
					{
						scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
						if (mb)
						{
							const video::SMaterial& material = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];

							video::IMaterialRenderer* rnd = driver->getMaterialRenderer(material.MaterialType);
							bool transparent = (rnd && rnd->isTransparent());

							// only render transparent buffer if this is the transparent render pass
							// and solid only in solid pass
							if (transparent == isTransparentPass) 
							{
								driver->setMaterial(material);
								driver->drawMeshBuffer(mb);
							}
						}
					}
				}

				driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

				// for debug purposes only:
				if ( DebugDataVisible && PassCount==1)
				{
					video::SMaterial m;
					m.Lighting = false;
					driver->setMaterial(m);

					if ( DebugDataVisible & scene::EDS_BBOX )
					{
						driver->draw3DBox(Box, video::SColor(255,255,255,255));
					}
					if ( DebugDataVisible & scene::EDS_BBOX_BUFFERS )
					{
						for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
						{
							driver->draw3DBox(
								Mesh->getMeshBuffer(g)->getBoundingBox(),
								video::SColor(255,190,128,128));
						}
					}

					if ( DebugDataVisible & scene::EDS_NORMALS )
					{
						IAnimatedMesh * arrow = SceneManager->addArrowMesh (
							"__debugnormal", 0xFFECEC00,
							0xFF999900, 4, 8, 1.f, 0.6f, 0.05f,
							0.3f);
						if ( 0 == arrow )
						{
							arrow = SceneManager->getMesh ( "__debugnormal" );
						}
						IMesh *mesh = arrow->getMesh(0);

						// find a good scaling factor

						core::matrix4 m2;

						// draw normals
						for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
						{
							const scene::IMeshBuffer* mb = Mesh->getMeshBuffer(g);
							const u32 vSize = video::getVertexPitchFromType(mb->getVertexType());
							const video::S3DVertex* v = ( const video::S3DVertex*)mb->getVertices();
							for ( u32 i=0; i != mb->getVertexCount(); ++i )
							{
								// align to v->Normal
								core::quaternion quatRot(v->Normal.X, 0.f, -v->Normal.X, 1+v->Normal.Y);
								quatRot.normalize();
								//quatRot.getMatrix(m2);
								m2 = quatRot.getMatrix();

								m2.setTranslation(v->Pos);
								m2*=AbsoluteTransformation;

								driver->setTransform(video::ETS_WORLD, m2);
								for (u32 a = 0; a != mesh->getMeshBufferCount(); ++a)
									driver->drawMeshBuffer(mesh->getMeshBuffer(a));

								v = (const video::S3DVertex*) ( (u8*) v + vSize );
							}
						}
						driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
					}

					// show mesh
					if ( DebugDataVisible & scene::EDS_MESH_WIRE_OVERLAY )
					{
						m.Wireframe = true;
						driver->setMaterial(m);

						for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
						{
							driver->drawMeshBuffer( Mesh->getMeshBuffer(g) );
						}
					}
				}
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CTiledPlaneNode::getBoundingBox() const
			{
				//return Box;
				return Mesh ? Mesh->getBoundingBox() : Box;
			}


			//! returns the material based on the zero based index i. To get the amount
			//! of materials used by this scene node, use getMaterialCount().
			//! This function is needed for inserting the node into the scene hierarchy on a
			//! optimal position for minimizing renderstate changes, but can also be used
			//! to directly modify the material of a scene node.
			video::SMaterial& CTiledPlaneNode::getMaterial(u32 i)
			{
				if (Mesh && ReadOnlyMaterials && i<Mesh->getMeshBufferCount())
				{
					tmpReadOnlyMaterial = Mesh->getMeshBuffer(i)->getMaterial();
					return tmpReadOnlyMaterial;
				}

				if ( i >= Materials.size())
					return ISceneNode::getMaterial(i);

				return Materials[i];
			}



			//! returns amount of materials used by this scene node.
			u32 CTiledPlaneNode::getMaterialCount() const
			{
				if (Mesh && ReadOnlyMaterials)
					return Mesh->getMeshBufferCount();

				return Materials.size();
			}


			//! Setup a new mesh
			irr::scene::IMesh *CTiledPlaneNode::setupMesh(char *MeshName)
			{
				irr::scene::IAnimatedMesh* pMesh;

				char szBuf[256];
				if(!strcmp(MeshName,""))
				{
					
					sprintf_s(szBuf,MeshNameFormat,
						m_TileCount.Width,m_TileCount.Height,
						m_TileSize.Width,m_TileSize.Height);
					MeshName = szBuf;
				}

				if(SceneManager->getMeshCache()->isMeshLoaded(MeshName) == false)				
				{
					pMesh = SceneManager->addHillPlaneMesh(MeshName,
						m_TileSize,
						m_TileCount,0,0,
						irr::core::dimension2df(0,0),
						irr::core::dimension2df((f32)m_TileCount.Width,(f32)m_TileCount.Height)
						);
				}
				else
				{
					pMesh = SceneManager->getMesh(MeshName);
				}

				return pMesh;
			}

			//! Sets a new mesh
			void CTiledPlaneNode::setMesh(IMesh* mesh)
			{
				if (!mesh)
					return; // won't set null mesh

				if (Mesh)
					Mesh->drop();

				Mesh = mesh;
				copyMaterials();

				//Mesh->getReferenceCount();

				if (Mesh)
					Mesh->grab();
			}


			void CTiledPlaneNode::copyMaterials(irr::scene::IMesh *copyMesh)
			{				

				Materials.clear();

				if(copyMesh)
				{
					video::SMaterial mat;

					for (u32 i=0; i<copyMesh->getMeshBufferCount(); ++i)
					{
						IMeshBuffer* mb = copyMesh->getMeshBuffer(i);
						if (mb)
							mat = mb->getMaterial();

						Materials.push_back(mat);
					}					
				}
				else if (Mesh)
				{
					video::SMaterial mat;

					for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
					{
						IMeshBuffer* mb = Mesh->getMeshBuffer(i);
						if (mb)
							mat = mb->getMaterial();

						Materials.push_back(mat);
					}
				}
			}


			//! Writes attributes of the scene node.
			void CTiledPlaneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				
				//m_nTempType = (ESCENE_NODE_TYPE)CTiledPlaneNode::TypeID;

				//ReadyToSave();
				
				IMeshSceneNode::serializeAttributes(out, options);
				

				char szBuf[256];

#ifdef JZ3DPLUGINS_EXPORTS
				out->addString("Mesh", SceneManager->getMeshCache()->getMeshFilename(Mesh) );		

#else
				//out->addString("Mesh", SceneManager->getMeshCache()->getMeshFilename(Mesh));
				out->addString("Mesh", SceneManager->getMeshCache()->getMeshFilename(Mesh).c_str());
#endif
				out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);

				sprintf_s(szBuf,256,"%d,%d",m_TileCount.Width,m_TileCount.Height);
				out->addString("TileCount",szBuf);

				sprintf_s(szBuf,256,"%.2f,%.2f",m_TileSize.Width,m_TileSize.Height);
				out->addString("TileSize",szBuf);

				out->addBool("LightMap",m_bLightMapGen_Enable);

				//m_nTempType = ESNT_MESH;

				//out->addInt("TileCountWidth",m_TileCount.Width);
				//out->addInt("TileCountHeight",m_TileCount.Height);

				//out->addVector3d("TileSize",irr::core::vector3df(m_TileSize.Width,m_TileSize.Height,0));
				//out->addRect("box",irr::core::rect<s32>());

			}

			//! Reads attributes of the scene node.
			void CTiledPlaneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{
				core::stringc oldMeshStr = SceneManager->getMeshCache()->getMeshFilename(Mesh);

				core::stringc newMeshStr = in->getAttributeAsString("Mesh");

				ReadOnlyMaterials = in->getAttributeAsBool("ReadOnlyMaterials");

				irr::core::stringc strTemp;				
				
				//타일카운트
				strTemp = in->getAttributeAsString("TileCount").c_str();					
				sscanf_s(strTemp.c_str(),"%d,%d",&m_TileCount.Width,&m_TileCount.Height);

				//타일크기
				strTemp = in->getAttributeAsString("TileSize").c_str();					
				sscanf_s(strTemp.c_str(),"%f,%f",&m_TileSize.Width,&m_TileSize.Height);

				
				irr::core::stringc newMeshStr2;

				char szBuf[256];

				sprintf_s(szBuf,MeshNameFormat,
					m_TileCount.Width,m_TileCount.Height,
					m_TileSize.Width,m_TileSize.Height);

				newMeshStr2 = szBuf;

				//메쉬가 바꼈으면
				if(newMeshStr2 != newMeshStr || newMeshStr2 != oldMeshStr)
				{
					//copyMaterials(Mesh);

					IMesh* newMesh = 0;
					newMesh = setupMesh(szBuf); //기존 로드된메쉬를 검색해보고 없으면 다시만든다.				

					if (newMesh)
					{						
						if (Mesh)
							Mesh->drop();

						
						Mesh = newMesh;
						//copyMaterials(SceneManager->getMesh(newMeshStr.c_str()));

						if (Mesh)
							Mesh->grab();
						//setMesh(newMesh);
					}

				}
				else if (newMeshStr != "" && oldMeshStr != newMeshStr)//메쉬 직접입력 할경우
				{
					IMesh* newMesh = 0;
					IAnimatedMesh* newAnimatedMesh = SceneManager->getMesh(newMeshStr.c_str());

					if (newAnimatedMesh)
						newMesh = newAnimatedMesh->getMesh(0);

					if (newMesh)
						setMesh(newMesh);
				}

				m_bLightMapGen_Enable = in->getAttributeAsBool("LightMap");

				IMeshSceneNode::deserializeAttributes(in, options);
			}

			//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
			/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
			referencing this mesh to change too. */
			void CTiledPlaneNode::setReadOnlyMaterials(bool readonly)
			{
				ReadOnlyMaterials = readonly;
			}

			//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
			bool CTiledPlaneNode::isReadOnlyMaterials() const
			{
				return ReadOnlyMaterials;
			}


			//! Creates a clone of this scene node and its children.
			ISceneNode* CTiledPlaneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CTiledPlaneNode* nb = new CTiledPlaneNode(
					newParent, 
					newManager, 
					ID, 
					RelativeTranslation, RelativeRotation, RelativeScale);

				nb->cloneMembers(this, newManager);
				nb->ReadOnlyMaterials = ReadOnlyMaterials;
				nb->Materials = Materials;
				nb->m_TileCount = m_TileCount;
				nb->m_TileSize = m_TileSize;
				nb->m_bLightMapGen_Enable = m_bLightMapGen_Enable;

				{
#ifdef JZ3DPLUGINS_EXPORTS
					nb->setMesh(setupMesh((char *)SceneManager->getMeshCache()->getMeshFilename(Mesh) ));	
#else
					//nb->setMesh(setupMesh((char *)SceneManager->getMeshCache()->getMeshFilename(Mesh) ));	
					nb->setMesh(setupMesh((char *)SceneManager->getMeshCache()->getMeshFilename(Mesh).c_str() ));	
#endif
					
					//nb->copyMaterials(Mesh);
					nb->Materials = Materials;
				}

				nb->drop();
				return nb;
			}

		}
	}
}
