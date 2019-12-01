#include "Plan1x1SceneNode.h"


namespace irr {
	namespace scene
	{
		namespace jz3d
		{

			CPlan1x1SceneNode::CPlan1x1SceneNode(
				ISceneNode* parent, 
				ISceneManager* mgr,
				s32 id, const core::vector3df& position,
				const core::vector3df& rotation, const core::vector3df& scale)
				: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
				Mesh(0), PassCount(0),
				ReadOnlyMaterials(false)
			{
#ifdef _DEBUG
				setDebugName("CPlan1x1SceneNode");
#endif
				//irr::scene::ISceneNode* pNode = 0;

				irr::scene::IAnimatedMesh* pMesh; 				

				if(SceneManager->getMeshCache()->isMeshLoaded("jz3d/scene/mesh/plane1x1") == false)
				{
					pMesh = SceneManager->addHillPlaneMesh("jz3d/scene/mesh/plane1x1",
						irr::core::dimension2d<irr::f32>(10,10),
						irr::core::dimension2d<irr::u32>(1,1));
				}
				else
				{
					pMesh = SceneManager->getMesh("jz3d/scene/mesh/plane1x1");
				}
				

				setMesh(pMesh);
			}

			CPlan1x1SceneNode::~CPlan1x1SceneNode(void)
			{
				if (Mesh)
					Mesh->drop();
			}

			//void CPlan1x1SceneNode::setSize()
			//{
			//	// we are creating the cube mesh here. 

			//	// nicer texture mapping sent in by Dr Andros C Bragianos 
			//	// .. and then improved by jox.

			//	

			//	//video::SColor clr(255,255,255,255);

			//	//SMeshBuffer* buf = (SMeshBuffer*)Mesh.getMeshBuffer(0);

			//	//buf->Vertices.reallocate(12);
			//	//// Start setting vertices from index 0 to deal with this method being called multiple times.
			//	//buf->Vertices.set_used(0);
			//	//buf->Vertices.push_back(video::S3DVertex(0,0,0, -1,-1,-1, clr, 0, 1));
			//	//buf->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 1, 1));
			//	//buf->Vertices.push_back(video::S3DVertex(1,1,0,  1, 1,-1, clr, 1, 0));
			//	//buf->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 0, 0));
			//	//buf->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 0, 1));
			//	//buf->Vertices.push_back(video::S3DVertex(1,1,1,  1, 1, 1, clr, 0, 0));
			//	//buf->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 1, 0));
			//	//buf->Vertices.push_back(video::S3DVertex(0,0,1, -1,-1, 1, clr, 1, 1));
			//	//buf->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 0, 1));
			//	//buf->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 1, 1));
			//	//buf->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 1, 0));
			//	//buf->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 0, 0));

			//	//buf->BoundingBox.reset(0,0,0); 

			//	//for (u32 i=0; i<12; ++i)
			//	//{
			//	//	buf->Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
			//	//	buf->Vertices[i].Pos *= Size;
			//	//	buf->BoundingBox.addInternalPoint(buf->Vertices[i].Pos);
			//	//}
			//}


			//! renders the node.
			void CPlan1x1SceneNode::render()
			{
				video::IVideoDriver* driver = SceneManager->getVideoDriver();
				//driver->setMaterial(Mesh->getMeshBuffer(0)->getMaterial());
				driver->setMaterial(getMaterial(0));
				driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
				driver->drawMeshBuffer(Mesh->getMeshBuffer(0));
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CPlan1x1SceneNode::getBoundingBox() const
			{
				return Mesh ? Mesh->getBoundingBox() : Box;
			}


			void CPlan1x1SceneNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);
				ISceneNode::OnRegisterSceneNode();
			}


			//! returns the material based on the zero based index i. To get the amount
			//! of materials used by this scene node, use getMaterialCount().
			//! This function is needed for inserting the node into the scene hirachy on a
			//! optimal position for minimizing renderstate changes, but can also be used
			//! to directly modify the material of a scene node.
			video::SMaterial& CPlan1x1SceneNode::getMaterial(u32 i)
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
			u32 CPlan1x1SceneNode::getMaterialCount() const
			{

				if (Mesh && ReadOnlyMaterials)
					return Mesh->getMeshBufferCount();

				return Materials.size();
			}


			//! Writes attributes of the scene node.
			void CPlan1x1SceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);


			}


			//! Reads attributes of the scene node.
			void CPlan1x1SceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{
				//Size =	in->getAttributeAsFloat("Size");
				//Size = core::max_(Size, 0.0001f);
				//setSize();

				ISceneNode::deserializeAttributes(in, options);
			}


			//! Creates a clone of this scene node and its children.
			ISceneNode* CPlan1x1SceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CPlan1x1SceneNode* nb = new CPlan1x1SceneNode(
					newParent, 
					newManager, 
					ID, 
					RelativeTranslation);

				nb->cloneMembers(this, newManager);
				nb->Materials = Materials;
				//nb->getMaterial(0) = getMaterial(0);

				nb->drop();
				return nb;
			}

			void CPlan1x1SceneNode::setMesh(IMesh* mesh)
			{
				if (!mesh)
					return; // won't set null mesh

				if (Mesh)
					Mesh->drop();

				Mesh = mesh;
				copyMaterials();

				if (Mesh)
					Mesh->grab();
			}


			void CPlan1x1SceneNode::copyMaterials()
			{
				Materials.clear();

				if (Mesh)
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
		}
	}
}
