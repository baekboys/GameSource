#include "BlockNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	int CBlockNode::blockCount = 0;
	const char *CBlockNode::Name  = "CBlockNode";

	const wchar_t* CBlockNode::typeNames[] =
			{
				L"0",
				L"1",
				L"2",
				L"3",
				L"4",
				0
			};

	const wchar_t* CBlockNode::propertyNames[] =
	{
		L"NORMAL",
		L"DESTROY",
		L"MOVE",
		L"ROTATION",
		L"SPRING",
		L"ILLUSION",
		0
	};

	const wchar_t* CBlockNode::springBlockDirectionNames[] =
	{
		L"UP",
		L"DOWN",
		L"LEFT",
		L"RIGHT",
		0
	};

	const wchar_t* CBlockNode::rotationBlockDirectionNames[] =
	{
		L"LEFT",
		L"RIGHT",
		0
	};

	/*const wchar_t* CBlockNode::typeNames[] =			
	{
		L"wood",
		L"ice",
		L"concrete",
		L"brick",
		L"space",
		0
	};*/

	CBlockNode::CBlockNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position,
		const core::vector3df& rotation,
		const core::vector3df& scale,
		irr::f32 blockSize, irr::u32 countX, irr::u32 countZ)
		: IMeshSceneNode(parent,mgr,id, position, rotation, scale), 
		m_fBlockSize (blockSize), m_uCountX (countX), m_uCountZ (countZ), Mesh (NULL),
		m_fFriction (0.5f), m_fRestitution (0), m_uBlockType(BLOCK_WOOD), m_uBlockProperty(BLOCK_PROPERTY_NORMAL), m_pCollisionShape(NULL)
	{
#ifdef _DEBUG
		setDebugName(CBlockNode::Name);
#endif
		setAutomaticCulling(scene::EAC_OFF);

		// 블럭 속성 값 초기화
		{
			irr::u32 i;
			for(i = 0 ; i < 3 ; ++i)
			{
				m_fBlockPropertyValue[i] = 0;
			}
		}
		
		// 아이템의 카운트 1개 증가 및 노드이름 정하기
		{
			blockCount++;
			irr::core::stringc temp("Block/");
			temp += blockCount;
			this->setName(temp.c_str());
		}


		// 매쉬 만들기 및 collisionShape 만들기
		{
			createBlock();
		}

		// 매트리얼 설정
		{
			// 매트리얼 설정
			irr::video::SMaterial m;
			m.Lighting = false;
			m.BackfaceCulling = false;

			irr::video::SMaterial mt;

			// 0번 매트리얼은 m, 나머지는 mt로 초기화
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
		}

		// 기본 타입 설정
		setType(BLOCK_WOOD);
		
		m_fFriction = 0.5f;
		m_fRestitution = 0.1f;
	}

	CBlockNode::~CBlockNode()
	{
		blockCount--;

		// 생성된 매쉬가 있다면 드롭
		if(Mesh)
			Mesh->drop();
		
		if(m_pCollisionShape)
			delete m_pCollisionShape;
	}


	void CBlockNode::OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}


	void CBlockNode::render()
	{
		
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		driver->setMaterial(getMaterial(0));
		driver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
		
		// 매쉬버퍼로 그리기
		driver->drawMeshBuffer(Mesh->getMeshBuffer(0));

		if(DebugDataVisible)
		{
			// 물리적인 영역을 표시하기위한 바운딩 박스
			driver->draw3DBox(Mesh->getBoundingBox(), irr::video::SColor(255,255,0,0));
		}
	}


	const core::aabbox3d<f32>& CBlockNode::getBoundingBox() const
	{
		return Mesh->getBoundingBox();
	}


	ISceneNode* CBlockNode::clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if (!newParent) newParent = Parent;
		if (!newManager) newManager = SceneManager;

		CBlockNode* nb = new CBlockNode(newParent, 
			newManager, ID);

		nb->cloneMembers(this, newManager);

		// 신노드의 이름 다시 설정하기.
		// 그 이유는 위의 cloneMembers함수를 호출하여 신노드의 이름까지 복사되었다.
		irr::core::stringc temp("Block/");
		temp += blockCount;
		nb->setName(temp.c_str());

		// 매쉬관련 복사
		nb->m_fBlockSize = m_fBlockSize;
		nb->m_uCountX = m_uCountX;
		nb->m_uCountZ = m_uCountZ;

		// 매쉬의 이름은 복사하지 않는다.
		// 기본생성자로 생성하였기 때문에 다시 매쉬를 만들어야한다.
		// 이 과정을 생략하려면 생성자에서 설정하면 된다.
		// 생성자에서 해결하려면 생성자함수가 길어져서 복잡해 보이기 때문에 매쉬를 새로 만들기로 하였다.
		// 뭐... 편한 스타일로 하면된다...

		// 재질관련 복사
		nb->m_Matrial = m_Matrial;

		// 타입 복사
		nb->setType(m_uBlockType);

		// 물리속성 관련 복사
		nb->m_fFriction = m_fFriction;
		nb->m_fRestitution = m_fRestitution;

		// 블럭 속성 복사
		nb->m_uBlockProperty = m_uBlockProperty;

		// 블럭 속성 값 복사
		irr::u32 i;
		for(i = 0 ; i < 3 ; ++i)
		{
			nb->m_fBlockPropertyValue[i] = m_fBlockPropertyValue[i];
		}

		// 블럭 시작위치 값 복사
		nb->m_startPosition = m_startPosition;

		// 블럭 회전 값 복사
		nb->m_startRotation = m_startRotation;

		// 매쉬 다시 만드는 함수 호출
		nb->createBlock();

		nb->drop();
		return nb;
	}		


	void CBlockNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		ISceneNode::serializeAttributes(out, options);		

		//데이터 쓰기 코드추가
		//...

		out->addString("Mesh Name", m_meshName.c_str());
		out->addFloat("Block size" , m_fBlockSize);
		out->addInt("BlockCount X", m_uCountX);
		out->addInt("BlockCount Z", m_uCountZ);
		out->addFloat("friction", m_fFriction);
		out->addFloat("restitution", m_fRestitution);
		out->addInt("Block type", m_uBlockType);
		out->addInt("Block property", m_uBlockProperty);
		out->addFloat("value0", m_fBlockPropertyValue[0]);
		out->addFloat("value1", m_fBlockPropertyValue[1]);
		out->addFloat("value2", m_fBlockPropertyValue[2]);
		out->addVector3d("start position", m_startPosition);
		out->addVector3d("start rotation", m_startRotation);
	}

	//! Reads attributes of the scene node.
	void CBlockNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{	
		//데이터 읽기 코드추가
		//...

		// 사이즈 설정
		setSize( in->getAttributeAsFloat("Block size") );

		// 블럭의 갯수 설정
		setBlockCount( in->getAttributeAsInt("BlockCount X"), in->getAttributeAsInt("BlockCount Z") );

		// 마찰력
		m_fFriction = in->getAttributeAsFloat("friction");

		// 탄성력
		m_fRestitution = in->getAttributeAsFloat("restitution");

		// 블럭 타입
		m_uBlockType = in->getAttributeAsInt("Block type");

		// 블럭 특성
		setProperty(in->getAttributeAsInt("Block property"));

		// 블럭 값
		m_fBlockPropertyValue[0] = in->getAttributeAsFloat("value0");
		m_fBlockPropertyValue[1] = in->getAttributeAsFloat("value1");
		m_fBlockPropertyValue[2] = in->getAttributeAsFloat("value2");

		// 블럭 시작 위치
		m_startPosition = in->getAttributeAsVector3d("start position");

		// 블럭 회전값 
		m_startRotation = in->getAttributeAsVector3d("start rotation");

		ISceneNode::deserializeAttributes(in, options);				
	}

	void CBlockNode::setBlockCount(irr::u32 countX, irr::u32 countZ)
	{
		irr::u32 X = countX;
		irr::u32 Z = countZ;

		// 갯수가 0이 되는것을 방지.
		if(countX == 0)
			X = 1;
		if(countZ == 0)
			Z = 1;

		m_uCountX = X;
		m_uCountZ = Z;

		// 블럭의 갯수가 바뀌었을수 있으므로 매쉬를 바꿀 필요가 있으면 바꾼다.
		createBlock();
	}

	irr::core::stringc CBlockNode::getMeshName()
	{
		//-----------------------------------------------------------------------------------------------------
		// 매쉬이름의 규칙은 아래와 같다.
		// "블럭하나당기본사이즈_X축갯수_Z축갯수"
		// 예를 들면 사이즈가 20, X축으로 5개, Z축으로 2개이면 매쉬의 이름은 20.000000_5_2 이런식으로 할당된다.
		//-----------------------------------------------------------------------------------------------------

		irr::core::stringc name;

		name += m_fBlockSize;
		name += '_';
		name += m_uCountX;
		name += '_';
		name += m_uCountZ;

		return name;
	}

	irr::scene::IMesh* CBlockNode::createBlockMesh()
	{
		//////////////////////
		//					//
		//	매쉬 버퍼 생성	//
		//					//
		//////////////////////
		irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();


		//////////////////////
		//					//
		//	버텍스 추가하기	//
		//					//
		//////////////////////
		{
			//-----------------------------------------------------------
			// (1)육면체를 만들기 위한 좌표 12개, 버텍스 12개, 컬러 선언
			//-----------------------------------------------------------
			irr::core::vector3df point[12];
			irr::video::S3DVertex vertex[12];
			irr::video::SColor clr(255,255,255,255);

			//----------------------------------------------------------------------------------------------------
			// (2) 정점의 위치 계산하기. X와 Z축만을 이용할 것이므로, 2차원적으로 생각하면 좌측 상단부터 시작한다.
			//----------------------------------------------------------------------------------------------------

			// (2-1) 좌표의 기본적인 모양 잡기
			point[0].set(0,0,0);
			point[1].set(1,0,0);
			point[2].set(1,1,0);
			point[3].set(0,1,0);
			point[4].set(1,0,1);
			point[5].set(1,1,1);
			point[6].set(0,1,1);
			point[7].set(0,0,1);
			point[8].set(0,1,1);
			point[9].set(0,1,0);
			point[10].set(1,0,1);
			point[11].set(1,0,0);

			// (2-2) 블럭의 중앙을 맞추기 위한 거리
			irr::core::vector3df length(m_uCountX * m_fBlockSize / 2.f, m_fBlockSize / 2.f, m_uCountZ * m_fBlockSize / 2.f);

			// (2-3) 좌표의 위치 설정하여 버텍스에 넣기
			for(int i = 0 ; i < 12 ; ++i)
			{
				// 스케일을 기본크기1에서 블럭사이즈 만큼 키운다.
				point[i] *= m_fBlockSize;
				// 중앙에 맞추기 위하여 계산된 거리만큼 이동시킨다.
				point[i] -= length;

				// 버텍스의 위치는 위에서 계산된 point값이다.
				vertex[i].Pos = point[i];
				vertex[i].Color = clr;
			}

			//-------------------------------------------------
			// (3) 12개의 버텍스의 법선벡터와 텍스쳐좌표 설정.
			//-------------------------------------------------

			// 0번 버텍스
			vertex[0].Normal = irr::core::vector3df(-1, -1, -1);
			vertex[0].TCoords = irr::core::vector2df(0, 1);

			// 1번 버텍스
			vertex[1].Normal = irr::core::vector3df(1, -1, -1);
			vertex[1].TCoords = irr::core::vector2df(1, 1);

			// 2번 버텍스
			vertex[2].Normal = irr::core::vector3df(1, 1, -1);
			vertex[2].TCoords = irr::core::vector2df(1, 0);

			// 3번 버텍스
			vertex[3].Normal = irr::core::vector3df(-1, 1, -1);
			vertex[3].TCoords = irr::core::vector2df(0, 0);

			// 4번 버텍스
			vertex[4].Normal = irr::core::vector3df(1, -1, 1);
			vertex[4].TCoords = irr::core::vector2df(0, 1);

			// 5번 버텍스
			vertex[5].Normal = irr::core::vector3df(1, 1, 1);
			vertex[5].TCoords = irr::core::vector2df(0, 0);

			// 6번 버텍스
			vertex[6].Normal = irr::core::vector3df(-1, 1, 1);
			vertex[6].TCoords = irr::core::vector2df(1, 0);

			// 7번 버텍스
			vertex[7].Normal = irr::core::vector3df(-1, -1, 1);
			vertex[7].TCoords = irr::core::vector2df(1, 1);

			// 8번 버텍스
			vertex[8].Normal = irr::core::vector3df(-1, 1, 1);
			vertex[8].TCoords = irr::core::vector2df(0, 1);

			// 9번 버텍스
			vertex[9].Normal = irr::core::vector3df(-1, 1, -1);
			vertex[9].TCoords = irr::core::vector2df(1, 1);

			// 10번 버텍스
			vertex[10].Normal = irr::core::vector3df(1, -1, 1);
			vertex[10].TCoords = irr::core::vector2df(1, 0);

			// 11번 버텍스
			vertex[11].Normal = irr::core::vector3df(1, -1, -1);
			vertex[11].TCoords = irr::core::vector2df(0, 0);


			//-------------------------------------------------------------
			// (4) 매쉬버퍼에 m_uCountX, m_ucountZ 갯수만큼 버텍스 추가하기
			//-------------------------------------------------------------
			irr::u32 j, k, l;
			for(j = 0 ; j < m_uCountZ ; ++j) // Z축으로 증가하는 코드
			{
				for(k = 0 ; k < m_uCountX ; ++k) // X축으로 증가하는 코드
				{
					for(l = 0 ; l < 12 ; ++l) // 버텍스가 12개 이므로
					{
						// 위에서 만든 버텍스 정보를 가져옴.
						irr::video::S3DVertex ver = vertex[l];

						// 기본적인 버텍스에서 x축으로, z축으로 계속 위치를 변화시킴
						irr::core::vector3df addPos(m_fBlockSize * k, 0, m_fBlockSize * j);
						ver.Pos += addPos;

						// 매쉬버퍼에 버텍스 추가
						buffer->Vertices.push_back(ver);
					}
				}
			}
		}


		//////////////////////
		//					//
		//	인덱스 추가하기	//
		//					//
		//////////////////////
		{
			//-------------------------
			// (1) 인덱스 리스트 만들기
			//-------------------------

			// 블럭의 육면을 다 그릴때
			irr::u16 single_list[36] = {	0,2,1,	0,3,2,	1,5,4,	1,2,5,	4,6,7,	4,5,6,	7,3,0,	7,6,3,	9,5,2,	9,8,5,	0,11,10,	0,10,7 };
			
			// 블럭이 X축으로만 늘어날때(즉 Z축으로는 블럭이 1개일때) X축으로 늘어나므로 왼쪽면과 오른쪽 면은 그릴필요가 없다.
			irr::u16 only_X_list[24] = {	0,2,1,	0,3,2,	4,6,7,	4,5,6,	9,5,2,	9,8,5,	0,11,10,	0,10,7	};
			
			// 블럭이 Z축으로만 늘어날때(즉 X축으로는 블럭이 1개일때) Z축으로 늘어나므로 앞면과 뒷면은 그릴필요가 없다.
			irr::u16 only_Z_list[24] = {	1,5,4,	1,2,5,	7,3,0,	7,6,3,	9,5,2,	9,8,5,	0,11,10,	0,10,7	};
			
			// 블럭이 X축과 Z축으로 동시에 늘어날때 윗면과 아랫면만 그리면 된다.
			irr::u16 dual_XZ_list[12] = {	9,5,2,	9,8,5,	0,11,10,	0,10,7	};


			//---------------------
			// (2) 인덱스 추가하기
			//---------------------

			irr::u32 i, j, count; // 반복에 사용할 변수 선언.
			count = 0; //초기화
			for(i = 0 ; i < m_uCountZ ; ++i) // Z축의 갯수만큼 반복.
			{
				for(j = 0 ; j < m_uCountX ; ++j) // X축의 갯수만큼 반복. 즉 X축부터 먼저 쌓아간다.
				{
					// 상황에 따른 인덱스리스트를 순회할 변수
					irr::u32 k;


					//--------------------------------------------------------------
					// (3-1) 블럭의 위치가 각 코너에 있을 경우 육면체를 모두 그린다.
					//--------------------------------------------------------------
					if( (i==0 && j==0) || (i==0 && j==m_uCountX-1) || (i==m_uCountZ-1 && j==0) || (i==m_uCountZ-1 && j==m_uCountX-1) )
					{
						for(k = 0 ; k < 36 ; ++k)
						{
							// 블럭 1개당 12개의 버텍스가 있고, 그만큼 늘어나므로 인덱스리스트에 (현재카운트 * 버텍스의 갯수12)를 더해줘야 한다.
							// 인덱스리스트는 육면체를 모두 그리는 것을 사용한다.
							irr::u16 indexNumber = single_list[k] + (12 * count);

							// 매쉬버퍼에 인덱스 리스트 추가
							buffer->Indices.push_back(indexNumber);
						}
					}
					
					//-------------------------------
					// (3-2) 블럭이 X축으로 늘어날때
					//-------------------------------
					else if( (i==0 && j<m_uCountX) || (i==m_uCountZ-1 && j<m_uCountX) )
					{
						for(k = 0 ; k < 24 ; ++k)
						{
							// 블럭 1개당 12개의 버텍스가 있고, 그만큼 늘어나므로 인덱스리스트에 (현재카운트 * 버텍스의 갯수12)를 더해줘야 한다.
							// 인덱스리스트는 좌우면을 제외하고 그리는 것을 사용한다.
							irr::u16 indexNumber = only_X_list[k] + (12 * count);

							// 매쉬버퍼에 인덱스 리스트 추가
							buffer->Indices.push_back(indexNumber);
						}
					}

					//-------------------------------
					// (3-3) 블럭이 Z축으로 늘어날때
					//-------------------------------
					else if( (j==0 && i<m_uCountZ) || (j==m_uCountX-1 && i<m_uCountZ) )
					{
						for(k = 0 ; k < 24 ; ++k)
						{
							// 블럭 1개당 12개의 버텍스가 있고, 그만큼 늘어나므로 인덱스리스트에 (현재카운트 * 버텍스의 갯수12)를 더해줘야 한다.
							// 인덱스리스트는 좌우면을 제외하고 그리는 것을 사용한다.
							irr::u16 indexNumber = only_Z_list[k] + (12 * count);

							// 매쉬버퍼에 인덱스 리스트 추가
							buffer->Indices.push_back(indexNumber);
						}
					}

					//---------------------------------------------------------------------
					// (3-4) 블럭이 안쪽에 있을때, 즉 주위에 다른 블럭으로 둘러쌓여 있을때
					//---------------------------------------------------------------------
					else
					{
						for(k = 0 ; k < 12 ; ++k)
						{
							// 블럭 1개당 12개의 버텍스가 있고, 그만큼 늘어나므로 인덱스리스트에 (현재카운트 * 버텍스의 갯수12)를 더해줘야 한다.
							// 인덱스리스트는 좌우면을 제외하고 그리는 것을 사용한다.
							irr::u16 indexNumber = dual_XZ_list[k] + (12 * count);

							// 매쉬버퍼에 인덱스 리스트 추가
							buffer->Indices.push_back(indexNumber);
						}
					}

					// 버텍스번호를 위하여 카운트를 증가시킨다.
					count++;
				}
			}
		}


		//////////////////////
		//					//
		//	매쉬에 추가하기	//
		//					//
		//////////////////////
		buffer->recalculateBoundingBox();
		irr::scene::SMesh* mesh = new irr::scene::SMesh();
		mesh->addMeshBuffer(buffer);
		mesh->recalculateBoundingBox();
		buffer->drop();

		return mesh;
	}

	void CBlockNode::createBlock()
	{
		//---------------------------------------------------------------------------------------------
		// 매쉬의 이름이 기존의 매쉬이름과 동일하다면 다시 만들 필요가 없다.
		// 이름이 다르다면 블럭의 사이즈나 갯수가 변한것이므로 기존의 매쉬를 없애고 다시 만들어야 한다.
		//---------------------------------------------------------------------------------------------

		// 매쉬의 이름 얻어오기
		irr::core::stringc meshName = getMeshName();

		// 현재 만들매쉬의 이름이 이미 만들어진 이름과 다르다면 새로 매쉬를 생성한다.
		if(m_meshName != meshName)
		{
			// 만들어진 매쉬가 있다면 드롭한다.
			if(Mesh)
				Mesh->drop();

			// 새로 매쉬를 생성한다.
			Mesh = createBlockMesh();

			// collisionShape을 새로 생성한다.
			createCollisionShape();

			m_meshName = meshName;			
		}
	}

	void CBlockNode::createCollisionShape()
	{
		irr::core::vector3df size = Mesh->getBoundingBox().getExtent();

		btVector3 boxSize( size.X/2.f, size.Y/2.f, size.Z/2.f);

		if(m_pCollisionShape)
		{
			delete m_pCollisionShape;
			m_pCollisionShape = NULL;
		}

		m_pCollisionShape = new btBoxShape( boxSize ) ;
	}

	void CBlockNode::setType(irr::u32 type)
	{
		m_uBlockType = type;

		// 텍스쳐 설정
		irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();
		irr::video::ITexture *texture;

		switch(m_uBlockType)
		{
		case BLOCK_ICE :
			{
				texture = driver->getTexture("texture/block_ice.png");
				//m_fFriction = 0.f;
				//m_fRestitution = 0.f;
			}
			break;

		case BLOCK_CONCRETE :
			{
				texture = driver->getTexture("texture/block_concrete.png");
				//m_fFriction = 0.5f;
				//m_fRestitution = 0.f;
			}
			break;

		case BLOCK_BRICK :
			{
				texture = driver->getTexture("texture/block_brick.png");
				//m_fFriction = 0.6f;
				//m_fRestitution = 0.f;
			}
			break;

		case BLOCK_WOOD :
			{
				texture = driver->getTexture("texture/block_wood.png");
				//m_fFriction = 0.7f;
				//m_fRestitution = 0.5f;
			}
			break;

		case BLOCK_SPACE :
			{
				texture = driver->getTexture("texture/block_space.png");
				//m_fFriction = 0.8f;
				//m_fRestitution = 1.f;
			}
			break;
		}

		this->getMaterial(0).setTexture(0, texture);
	}

	void CBlockNode::startAnimation()
	{
		if(this->getAnimators().size() != 0)
			deleteAnimation();

		irr::scene::ISceneNodeAnimator* pAnimator = NULL;;

		switch(m_uBlockProperty)
		{
		case BLOCK_PROPERTY_MOVE:
			{
				// 시작위치
				irr::core::array<irr::core::vector3df> follow;
				//irr::core::vector3df startPosition = this->getAbsolutePosition();
				irr::core::vector3df startPosition = m_startPosition;
				follow.push_back(startPosition);
				
				// 종료위치
				irr::core::vector3df endPosition( startPosition.X + m_fBlockPropertyValue[0], 0,  startPosition.Z + m_fBlockPropertyValue[1]);
				follow.push_back(endPosition);

				// 애니메이터 생성
				pAnimator = SceneManager->createFollowSplineAnimator(
					0,
					follow,
					m_fBlockPropertyValue[2]
					);
			}
			break;

		case BLOCK_PROPERTY_ROTATION:
			{
				// 회전 방향 결정
				irr::f32 direction;
				switch( static_cast<irr::u32>(m_fBlockPropertyValue[0]) )
				{
				case BLOCK_PROPERTY_ROTATING_DIRECTION_LEFT:
					direction = 1;
					break;

				case BLOCK_PROPERTY_ROTATING_DIRECTION_RIGHT:
					direction = -1;
					break;
				}

				// 회전 애니메이터 생성
				pAnimator = SceneManager->createRotationAnimator(
					irr::core::vector3df(0,1,0) * direction * m_fBlockPropertyValue[1]
					);
			}
			break;
		}

		
		if(pAnimator)
		{
			// 애니메이터 추가
			this->addAnimator(pAnimator);
			// 애니메이터 드롭
			pAnimator->drop();
		}
	}

	void CBlockNode::deleteAnimation()
	{
		this->removeAnimators()	;
		this->setPosition(m_startPosition);
		this->setRotation(m_startRotation);
	}

	void CBlockNode::deleteBlockPropertyValues()
	{
		irr::u32 i;
		for(i = 0 ; i < 3 ; ++i)
			m_fBlockPropertyValue[i] = 0;
	}

	CBlockNode* CBlockNode::CreateBlockNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df start, irr::core::vector3df end, irr::s32 type)
	{
		if (!parent)
			parent = mgr->getRootSceneNode();

		CBlockNode* pNode = new CBlockNode(parent, mgr, id);

		irr::u32 countX = (irr::u32)(abs(end.X - start.X) / pNode->m_fBlockSize);
		irr::u32 countZ = (irr::u32)(abs(end.Z - start.Z) / pNode->m_fBlockSize);

		irr::f32 positionX = (end.X + start.X) / 2.f;
		irr::f32 positionY = 0.f;
		irr::f32 positionZ = (end.Z + start.Z) / 2.f;

		irr::core::vector3df startPosition(positionX, positionY, positionZ);
		pNode->setPosition(startPosition);
		pNode->m_startPosition = startPosition;
		pNode->setBlockCount(countX, countZ);
		pNode->setType(type);
		
		pNode->drop();

		return pNode;
	}
}
}
}