#include "TiledPlaneNormalMapNode.h"

//확장 타일드 플래인 씬노드(탄젠트 메쉬 변환기능 )
//작성 : 2009.9.2 gbox3d

// 수정 : 2009.11.12 명선
// normalmap과 parallax 가능.
// 타일의 갯수를 수정할때 자동으로 다시 normalmapping 실시
// parallax의 파라미터 추가 가능.

namespace irr 
{
namespace scene
{
namespace jz3d
{
	const char *CTiledPlaneNormalMapNode::Name = "TiledPlaneNormalMapNode";			
	const char *CTiledPlaneNormalMapNode::MeshNameFormat = "../res/usr/mesh/TNE/c%d_%d_s%d_%d";
	const char *CTiledPlaneNormalMapNode::MeshNameFormatTG = "../res/usr/mesh/TNE/TG/c%d_%d_s%d_%d";
	
	bool CTiledPlaneNormalMapNode::m_hasNormalMappingTexture = false;

	const c8* CTiledPlaneNormalMapNode::materialTypeList[] = 			
	{
		"EMT_PARALLAX_MAP_SOLID",
		"EMT_NORMAL_MAP_SOLID",
		"EMT_SOLID",
		0
	};

	CTiledPlaneNormalMapNode::CTiledPlaneNormalMapNode(ISceneNode* parent, ISceneManager* mgr,s32 id,	const core::vector3df& position,const core::vector3df& rotation, const core::vector3df& scale)
		: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
		Mesh(0), 
		PassCount(0),
		ReadOnlyMaterials(false),
		m_TileCount(irr::core::dimension2d<irr::u32>(1,1)),
		m_TileSize(irr::core::dimension2d<irr::f32>(10,10)),
		m_amplitude(9.0f), m_parallaxeffect (0.02f),
		m_normalmappingFlag(false), m_canRender(false)
	{	
		setMesh(setupMesh(getMeshName()));

		setMaterialType(irr::video::EMT_SOLID);
		m_materialType = "EMT_SOLID";
		m_msg = "set up a texture and heightMap-tex";
	}

	CTiledPlaneNormalMapNode::~CTiledPlaneNormalMapNode(void)
	{
	}			

	void CTiledPlaneNormalMapNode::OnRegisterSceneNode()
	{
		if (IsVisible)
		{
			video::IVideoDriver* driver = SceneManager->getVideoDriver();

			PassCount = 0;
			int transparentCount = 0;
			int solidCount = 0;

			if (ReadOnlyMaterials && Mesh)
			{
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

			if (solidCount)
				SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);

			if (transparentCount)
				SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);

			ISceneNode::OnRegisterSceneNode();
		}
	}

	void CTiledPlaneNormalMapNode::render()
	{
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		if (!Mesh || !driver || !m_canRender)
			return;

		bool isTransparentPass = 
			SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

		++PassCount;

		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		Box = Mesh->getBoundingBox();

		bool renderMeshes = true;
		video::SMaterial mat;
		if (DebugDataVisible && PassCount==1)
		{
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

					if (transparent == isTransparentPass) 
					{
						driver->setMaterial(material);
						driver->drawMeshBuffer(mb);
					}
				}
			}
		}

		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

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

				

				core::matrix4 m2;

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


#ifdef IRR_VER_16SVN
						quatRot.getMatrix(m2,irr::core::vector3df(0,0,0));
						//quatRot.getMatrix(m2);
#else
						//quatRot.getMatrix(m2);
						quatRot.getMatrix(m2,irr::core::vector3df(0,0,0));
#endif

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
	const core::aabbox3d<f32>& CTiledPlaneNormalMapNode::getBoundingBox() const
	{
		//return Box;
		return Mesh ? Mesh->getBoundingBox() : Box;
	}

	video::SMaterial& CTiledPlaneNormalMapNode::getMaterial(u32 i)
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
	u32 CTiledPlaneNormalMapNode::getMaterialCount() const
	{
		if (Mesh && ReadOnlyMaterials)
			return Mesh->getMeshBufferCount();

		return Materials.size();
	}


	//! Setup a new mesh
	irr::scene::IMesh *CTiledPlaneNormalMapNode::setupMesh(const char *MeshName)
	{
		irr::scene::IAnimatedMesh* pMesh;				

		if(!strcmp(MeshName,""))
		{
			return NULL;
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
	void CTiledPlaneNormalMapNode::setMesh(IMesh* mesh)
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


	void CTiledPlaneNormalMapNode::copyMaterials(irr::scene::IMesh *copyMesh)
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
	void CTiledPlaneNormalMapNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{

		IMeshSceneNode::serializeAttributes(out, options);		

		char szBuf[256];

		out->addString("Mesh",  irr::core::stringw(SceneManager->getMeshCache()->getMeshFilename(Mesh)).c_str() );
		out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);

		sprintf_s(szBuf,256,"%d,%d",m_TileCount.Width,m_TileCount.Height);
		out->addString("TileCount",szBuf);

		sprintf_s(szBuf,256,"%.2f,%.2f",m_TileSize.Width,m_TileSize.Height);
		out->addString("TileSize",szBuf);

		out->addEnum("material-Type", m_materialType.c_str(), (char **)&materialTypeList[0]);
		out->addFloat("narmalMap amplitude", m_amplitude);
		out->addFloat("height-param m_parallaxeffect", m_parallaxeffect);
		out->addString("debug Msg", m_msg.c_str());
	}

	//!getMesh name

	const char *CTiledPlaneNormalMapNode::getMeshName()
	{
		static char szBuf[256];

		if(m_normalmappingFlag)
		{
			sprintf_s(szBuf,MeshNameFormatTG,
				m_TileCount.Width,m_TileCount.Height,
				(int)(m_TileSize.Width*1000.f),(int)(m_TileSize.Height*1000.f)
				);
		}
		else
		{
			sprintf_s(szBuf,MeshNameFormat,
				m_TileCount.Width,m_TileCount.Height,
				(int)(m_TileSize.Width*1000.f),(int)(m_TileSize.Height*1000.f)
				);
		}
		return szBuf;
	}

	//! Reads attributes of the scene node.
	void CTiledPlaneNormalMapNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		// 매쉬의 이름 가져오기
		core::stringc oldMeshStr = SceneManager->getMeshCache()->getMeshFilename(Mesh);
		core::stringc newMeshStr = in->getAttributeAsString("Mesh");
		ReadOnlyMaterials = in->getAttributeAsBool("ReadOnlyMaterials");

		irr::core::stringc strTemp;				

		// 타일카운트
		strTemp = in->getAttributeAsString("TileCount").c_str();					
		sscanf_s(strTemp.c_str(),"%d,%d",&m_TileCount.Width,&m_TileCount.Height);

		// 타일크기
		strTemp = in->getAttributeAsString("TileSize").c_str();					
		sscanf_s(strTemp.c_str(),"%f,%f",&m_TileSize.Width,&m_TileSize.Height);

		// 매트리얼 타입
		m_materialType = in->getAttributeAsEnumeration("material-Type");
		if(m_materialType == "EMT_PARALLAX_MAP_SOLID")
		{
			setMaterialType(irr::video::EMT_PARALLAX_MAP_SOLID);
		}
		else
		if(m_materialType == "EMT_NORMAL_MAP_SOLID")
		{
			setMaterialType(irr::video::EMT_NORMAL_MAP_SOLID);
		}
		else
		if(m_materialType == "EMT_SOLID")
		{
			setMaterialType(irr::video::EMT_SOLID);
		}

		// 높이맵의 증폭값
		m_amplitude = in->getAttributeAsFloat("narmalMap amplitude");

		// EMT_PARALLAX_MAP_SOLID타입일때 사용되는 높이값
		m_parallaxeffect = in->getAttributeAsFloat("height-param m_parallaxeffect");

		//메쉬 변환 처리
		{
			irr::core::stringc newMeshStr2;					

			newMeshStr2 = getMeshName();

			//메쉬가 바꼈으면
			if(newMeshStr2 != newMeshStr || newMeshStr2 != oldMeshStr)
			{
				IMesh* newMesh = 0;
				newMesh = setupMesh(newMeshStr2.c_str()); //기존 로드된메쉬를 검색해보고 없으면 다시만든다.				

				if (newMesh)
				{						
					if (Mesh)
						Mesh->drop();

					Mesh = newMesh;					

					if (Mesh)
						Mesh->grab();						
				}

				// 메쉬가 바뀌었으므로 노멀맵핑을 다시 한다!!
				m_normalmappingFlag = false;
			}					
		}

		// 노멀맵핑이 되어있지않으면 노멀맵 만들기 실시
		if(!m_normalmappingFlag)
			makeNormalMapping();

		IMeshSceneNode::deserializeAttributes(in, options);
	}

	//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
	/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
	referencing this mesh to change too. */
	void CTiledPlaneNormalMapNode::setReadOnlyMaterials(bool readonly)
	{
		ReadOnlyMaterials = readonly;
	}

	//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
	bool CTiledPlaneNormalMapNode::isReadOnlyMaterials() const
	{
		return ReadOnlyMaterials;
	}


	//! Creates a clone of this scene node and its children.
	ISceneNode* CTiledPlaneNormalMapNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent) newParent = Parent;
		if (!newManager) newManager = SceneManager;

		CTiledPlaneNormalMapNode* nb = new CTiledPlaneNormalMapNode(
			newParent, 
			newManager, 
			ID, 
			RelativeTranslation, RelativeRotation, RelativeScale);

		nb->cloneMembers(this, newManager);
		nb->ReadOnlyMaterials = ReadOnlyMaterials;
		nb->Materials = Materials;
		nb->m_TileCount = m_TileCount;
		nb->m_TileSize = m_TileSize;

		nb->m_normalmappingFlag = m_normalmappingFlag;
		nb->m_canRender = m_canRender;
		nb->m_amplitude = m_amplitude;
		nb->m_parallaxeffect = m_parallaxeffect;
		nb->m_materialType = m_materialType;
		nb->m_msg = m_msg;
		

		{
			nb->setMesh( setupMesh( irr::core::stringc(SceneManager->getMeshCache()->getMeshFilename(Mesh)).c_str() ) );						
			nb->Materials = Materials;
		}

		nb->drop();
		return nb;
	}

	//////////////////////////////
	//							//
	//		노멀맵 만들기		//
	//							//
	//////////////////////////////
	bool CTiledPlaneNormalMapNode::makeNormalMapping()
	{
		//-----------------------------
		// (1) 텍스쳐와 높이맵 불러오기
		//-----------------------------
		irr::video::ITexture *pOriTexture = getMaterial(0).getTexture(0);
		irr::video::ITexture *pHeightMapTexture = getMaterial(0).getTexture(1);


		//---------------------------------------------------------------------------
		// (2) 텍스쳐나 높이맵 텍스쳐가 설정되어 있지 않다면 노멀맵핑을 하지 않는다.
		//---------------------------------------------------------------------------
		if(pOriTexture == NULL || pHeightMapTexture == NULL)
		{
			m_msg = "Err : setup Ori-texture & heightmap-texture!!";
			setMaterialType(irr::video::EMT_SOLID);
			m_materialType = "EMT_SOLID";
			return false;
		}
		else
		{
			//----------------------------------------------------------------------------------------------
			// (3) 노멀맵 생성한다. 만약에 이전에 높이맵텍스쳐로 노멀맵을 생성하였다면 다시 할 필요가 없다.
			//     만약 다시 노멀맵핑을 한다면 높이맵이아닌 노멀맵텍스쳐를 노멀맵핑하므로 중복되어 원하지 
			//     않는 결과가 나오게 된다.
			//----------------------------------------------------------------------------------------------
			if(!m_hasNormalMappingTexture)
			{
				SceneManager->getVideoDriver()->makeNormalMapTexture(pHeightMapTexture, m_amplitude);
				m_hasNormalMappingTexture = true;
			}

			//-----------------------------
			// (4) 탄젠트 메쉬를 생성한다.
			//-----------------------------
			if(Mesh->getMeshBuffer(0)->getVertexType() != irr::video::EVT_TANGENTS)
			{
				// 버텍스는 EVT_STANDARD, EVT_2TCOORDS, EVT_TANGENTS 타입이 있다.
				// 이전의 버텍스는 탄젠트벡터 정보를 가진 버텍스 타입이 아니므로 탄젠트타입의 버텍스로 구성된 메쉬를 생성한다.
				irr::scene::IMesh* pTangentMesh = SceneManager->getMeshManipulator()->createMeshWithTangents(Mesh);
				Mesh->drop();
				Mesh = pTangentMesh;
				Mesh->grab();						

				// 탄젠트 메쉬등록
				irr::scene::SAnimatedMesh *pAniMesh = new irr::scene::SAnimatedMesh(pTangentMesh);
				pTangentMesh->drop();				
				pAniMesh->recalculateBoundingBox();

				// 노말맵핑이 끝났으므로 노말맵핑 완료 플래그를 바꿔준다.
				m_normalmappingFlag = true;
				m_canRender = true;

				// 메쉬가 바뀌었으므로 이름을 다시 정해준다.
				irr::core::stringc strMeshName = getMeshName();
				SceneManager->getMeshCache()->addMesh(strMeshName.c_str(),pAniMesh); //씬메니져에 등록
				pAniMesh->drop();
			}

			irr::core::stringc msg("Making a ");
			msg	+=m_materialType;
			msg += " success !!";
			m_msg = msg;
		}

		return true;
	}

}
}
}