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

		// �� �Ӽ� �� �ʱ�ȭ
		{
			irr::u32 i;
			for(i = 0 ; i < 3 ; ++i)
			{
				m_fBlockPropertyValue[i] = 0;
			}
		}
		
		// �������� ī��Ʈ 1�� ���� �� ����̸� ���ϱ�
		{
			blockCount++;
			irr::core::stringc temp("Block/");
			temp += blockCount;
			this->setName(temp.c_str());
		}


		// �Ž� ����� �� collisionShape �����
		{
			createBlock();
		}

		// ��Ʈ���� ����
		{
			// ��Ʈ���� ����
			irr::video::SMaterial m;
			m.Lighting = false;
			m.BackfaceCulling = false;

			irr::video::SMaterial mt;

			// 0�� ��Ʈ������ m, �������� mt�� �ʱ�ȭ
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
		}

		// �⺻ Ÿ�� ����
		setType(BLOCK_WOOD);
		
		m_fFriction = 0.5f;
		m_fRestitution = 0.1f;
	}

	CBlockNode::~CBlockNode()
	{
		blockCount--;

		// ������ �Ž��� �ִٸ� ���
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
		
		// �Ž����۷� �׸���
		driver->drawMeshBuffer(Mesh->getMeshBuffer(0));

		if(DebugDataVisible)
		{
			// �������� ������ ǥ���ϱ����� �ٿ�� �ڽ�
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

		// �ų���� �̸� �ٽ� �����ϱ�.
		// �� ������ ���� cloneMembers�Լ��� ȣ���Ͽ� �ų���� �̸����� ����Ǿ���.
		irr::core::stringc temp("Block/");
		temp += blockCount;
		nb->setName(temp.c_str());

		// �Ž����� ����
		nb->m_fBlockSize = m_fBlockSize;
		nb->m_uCountX = m_uCountX;
		nb->m_uCountZ = m_uCountZ;

		// �Ž��� �̸��� �������� �ʴ´�.
		// �⺻�����ڷ� �����Ͽ��� ������ �ٽ� �Ž��� �������Ѵ�.
		// �� ������ �����Ϸ��� �����ڿ��� �����ϸ� �ȴ�.
		// �����ڿ��� �ذ��Ϸ��� �������Լ��� ������� ������ ���̱� ������ �Ž��� ���� ������ �Ͽ���.
		// ��... ���� ��Ÿ�Ϸ� �ϸ�ȴ�...

		// �������� ����
		nb->m_Matrial = m_Matrial;

		// Ÿ�� ����
		nb->setType(m_uBlockType);

		// �����Ӽ� ���� ����
		nb->m_fFriction = m_fFriction;
		nb->m_fRestitution = m_fRestitution;

		// �� �Ӽ� ����
		nb->m_uBlockProperty = m_uBlockProperty;

		// �� �Ӽ� �� ����
		irr::u32 i;
		for(i = 0 ; i < 3 ; ++i)
		{
			nb->m_fBlockPropertyValue[i] = m_fBlockPropertyValue[i];
		}

		// �� ������ġ �� ����
		nb->m_startPosition = m_startPosition;

		// �� ȸ�� �� ����
		nb->m_startRotation = m_startRotation;

		// �Ž� �ٽ� ����� �Լ� ȣ��
		nb->createBlock();

		nb->drop();
		return nb;
	}		


	void CBlockNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		ISceneNode::serializeAttributes(out, options);		

		//������ ���� �ڵ��߰�
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
		//������ �б� �ڵ��߰�
		//...

		// ������ ����
		setSize( in->getAttributeAsFloat("Block size") );

		// ���� ���� ����
		setBlockCount( in->getAttributeAsInt("BlockCount X"), in->getAttributeAsInt("BlockCount Z") );

		// ������
		m_fFriction = in->getAttributeAsFloat("friction");

		// ź����
		m_fRestitution = in->getAttributeAsFloat("restitution");

		// �� Ÿ��
		m_uBlockType = in->getAttributeAsInt("Block type");

		// �� Ư��
		setProperty(in->getAttributeAsInt("Block property"));

		// �� ��
		m_fBlockPropertyValue[0] = in->getAttributeAsFloat("value0");
		m_fBlockPropertyValue[1] = in->getAttributeAsFloat("value1");
		m_fBlockPropertyValue[2] = in->getAttributeAsFloat("value2");

		// �� ���� ��ġ
		m_startPosition = in->getAttributeAsVector3d("start position");

		// �� ȸ���� 
		m_startRotation = in->getAttributeAsVector3d("start rotation");

		ISceneNode::deserializeAttributes(in, options);				
	}

	void CBlockNode::setBlockCount(irr::u32 countX, irr::u32 countZ)
	{
		irr::u32 X = countX;
		irr::u32 Z = countZ;

		// ������ 0�� �Ǵ°��� ����.
		if(countX == 0)
			X = 1;
		if(countZ == 0)
			Z = 1;

		m_uCountX = X;
		m_uCountZ = Z;

		// ���� ������ �ٲ������ �����Ƿ� �Ž��� �ٲ� �ʿ䰡 ������ �ٲ۴�.
		createBlock();
	}

	irr::core::stringc CBlockNode::getMeshName()
	{
		//-----------------------------------------------------------------------------------------------------
		// �Ž��̸��� ��Ģ�� �Ʒ��� ����.
		// "���ϳ���⺻������_X�హ��_Z�హ��"
		// ���� ��� ����� 20, X������ 5��, Z������ 2���̸� �Ž��� �̸��� 20.000000_5_2 �̷������� �Ҵ�ȴ�.
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
		//	�Ž� ���� ����	//
		//					//
		//////////////////////
		irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();


		//////////////////////
		//					//
		//	���ؽ� �߰��ϱ�	//
		//					//
		//////////////////////
		{
			//-----------------------------------------------------------
			// (1)����ü�� ����� ���� ��ǥ 12��, ���ؽ� 12��, �÷� ����
			//-----------------------------------------------------------
			irr::core::vector3df point[12];
			irr::video::S3DVertex vertex[12];
			irr::video::SColor clr(255,255,255,255);

			//----------------------------------------------------------------------------------------------------
			// (2) ������ ��ġ ����ϱ�. X�� Z�ุ�� �̿��� ���̹Ƿ�, 2���������� �����ϸ� ���� ��ܺ��� �����Ѵ�.
			//----------------------------------------------------------------------------------------------------

			// (2-1) ��ǥ�� �⺻���� ��� ���
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

			// (2-2) ���� �߾��� ���߱� ���� �Ÿ�
			irr::core::vector3df length(m_uCountX * m_fBlockSize / 2.f, m_fBlockSize / 2.f, m_uCountZ * m_fBlockSize / 2.f);

			// (2-3) ��ǥ�� ��ġ �����Ͽ� ���ؽ��� �ֱ�
			for(int i = 0 ; i < 12 ; ++i)
			{
				// �������� �⺻ũ��1���� �������� ��ŭ Ű���.
				point[i] *= m_fBlockSize;
				// �߾ӿ� ���߱� ���Ͽ� ���� �Ÿ���ŭ �̵���Ų��.
				point[i] -= length;

				// ���ؽ��� ��ġ�� ������ ���� point���̴�.
				vertex[i].Pos = point[i];
				vertex[i].Color = clr;
			}

			//-------------------------------------------------
			// (3) 12���� ���ؽ��� �������Ϳ� �ؽ�����ǥ ����.
			//-------------------------------------------------

			// 0�� ���ؽ�
			vertex[0].Normal = irr::core::vector3df(-1, -1, -1);
			vertex[0].TCoords = irr::core::vector2df(0, 1);

			// 1�� ���ؽ�
			vertex[1].Normal = irr::core::vector3df(1, -1, -1);
			vertex[1].TCoords = irr::core::vector2df(1, 1);

			// 2�� ���ؽ�
			vertex[2].Normal = irr::core::vector3df(1, 1, -1);
			vertex[2].TCoords = irr::core::vector2df(1, 0);

			// 3�� ���ؽ�
			vertex[3].Normal = irr::core::vector3df(-1, 1, -1);
			vertex[3].TCoords = irr::core::vector2df(0, 0);

			// 4�� ���ؽ�
			vertex[4].Normal = irr::core::vector3df(1, -1, 1);
			vertex[4].TCoords = irr::core::vector2df(0, 1);

			// 5�� ���ؽ�
			vertex[5].Normal = irr::core::vector3df(1, 1, 1);
			vertex[5].TCoords = irr::core::vector2df(0, 0);

			// 6�� ���ؽ�
			vertex[6].Normal = irr::core::vector3df(-1, 1, 1);
			vertex[6].TCoords = irr::core::vector2df(1, 0);

			// 7�� ���ؽ�
			vertex[7].Normal = irr::core::vector3df(-1, -1, 1);
			vertex[7].TCoords = irr::core::vector2df(1, 1);

			// 8�� ���ؽ�
			vertex[8].Normal = irr::core::vector3df(-1, 1, 1);
			vertex[8].TCoords = irr::core::vector2df(0, 1);

			// 9�� ���ؽ�
			vertex[9].Normal = irr::core::vector3df(-1, 1, -1);
			vertex[9].TCoords = irr::core::vector2df(1, 1);

			// 10�� ���ؽ�
			vertex[10].Normal = irr::core::vector3df(1, -1, 1);
			vertex[10].TCoords = irr::core::vector2df(1, 0);

			// 11�� ���ؽ�
			vertex[11].Normal = irr::core::vector3df(1, -1, -1);
			vertex[11].TCoords = irr::core::vector2df(0, 0);


			//-------------------------------------------------------------
			// (4) �Ž����ۿ� m_uCountX, m_ucountZ ������ŭ ���ؽ� �߰��ϱ�
			//-------------------------------------------------------------
			irr::u32 j, k, l;
			for(j = 0 ; j < m_uCountZ ; ++j) // Z������ �����ϴ� �ڵ�
			{
				for(k = 0 ; k < m_uCountX ; ++k) // X������ �����ϴ� �ڵ�
				{
					for(l = 0 ; l < 12 ; ++l) // ���ؽ��� 12�� �̹Ƿ�
					{
						// ������ ���� ���ؽ� ������ ������.
						irr::video::S3DVertex ver = vertex[l];

						// �⺻���� ���ؽ����� x������, z������ ��� ��ġ�� ��ȭ��Ŵ
						irr::core::vector3df addPos(m_fBlockSize * k, 0, m_fBlockSize * j);
						ver.Pos += addPos;

						// �Ž����ۿ� ���ؽ� �߰�
						buffer->Vertices.push_back(ver);
					}
				}
			}
		}


		//////////////////////
		//					//
		//	�ε��� �߰��ϱ�	//
		//					//
		//////////////////////
		{
			//-------------------------
			// (1) �ε��� ����Ʈ �����
			//-------------------------

			// ���� ������ �� �׸���
			irr::u16 single_list[36] = {	0,2,1,	0,3,2,	1,5,4,	1,2,5,	4,6,7,	4,5,6,	7,3,0,	7,6,3,	9,5,2,	9,8,5,	0,11,10,	0,10,7 };
			
			// ���� X�����θ� �þ��(�� Z�����δ� ���� 1���϶�) X������ �þ�Ƿ� ���ʸ�� ������ ���� �׸��ʿ䰡 ����.
			irr::u16 only_X_list[24] = {	0,2,1,	0,3,2,	4,6,7,	4,5,6,	9,5,2,	9,8,5,	0,11,10,	0,10,7	};
			
			// ���� Z�����θ� �þ��(�� X�����δ� ���� 1���϶�) Z������ �þ�Ƿ� �ո�� �޸��� �׸��ʿ䰡 ����.
			irr::u16 only_Z_list[24] = {	1,5,4,	1,2,5,	7,3,0,	7,6,3,	9,5,2,	9,8,5,	0,11,10,	0,10,7	};
			
			// ���� X��� Z������ ���ÿ� �þ�� ����� �Ʒ��鸸 �׸��� �ȴ�.
			irr::u16 dual_XZ_list[12] = {	9,5,2,	9,8,5,	0,11,10,	0,10,7	};


			//---------------------
			// (2) �ε��� �߰��ϱ�
			//---------------------

			irr::u32 i, j, count; // �ݺ��� ����� ���� ����.
			count = 0; //�ʱ�ȭ
			for(i = 0 ; i < m_uCountZ ; ++i) // Z���� ������ŭ �ݺ�.
			{
				for(j = 0 ; j < m_uCountX ; ++j) // X���� ������ŭ �ݺ�. �� X����� ���� �׾ư���.
				{
					// ��Ȳ�� ���� �ε�������Ʈ�� ��ȸ�� ����
					irr::u32 k;


					//--------------------------------------------------------------
					// (3-1) ���� ��ġ�� �� �ڳʿ� ���� ��� ����ü�� ��� �׸���.
					//--------------------------------------------------------------
					if( (i==0 && j==0) || (i==0 && j==m_uCountX-1) || (i==m_uCountZ-1 && j==0) || (i==m_uCountZ-1 && j==m_uCountX-1) )
					{
						for(k = 0 ; k < 36 ; ++k)
						{
							// �� 1���� 12���� ���ؽ��� �ְ�, �׸�ŭ �þ�Ƿ� �ε�������Ʈ�� (����ī��Ʈ * ���ؽ��� ����12)�� ������� �Ѵ�.
							// �ε�������Ʈ�� ����ü�� ��� �׸��� ���� ����Ѵ�.
							irr::u16 indexNumber = single_list[k] + (12 * count);

							// �Ž����ۿ� �ε��� ����Ʈ �߰�
							buffer->Indices.push_back(indexNumber);
						}
					}
					
					//-------------------------------
					// (3-2) ���� X������ �þ��
					//-------------------------------
					else if( (i==0 && j<m_uCountX) || (i==m_uCountZ-1 && j<m_uCountX) )
					{
						for(k = 0 ; k < 24 ; ++k)
						{
							// �� 1���� 12���� ���ؽ��� �ְ�, �׸�ŭ �þ�Ƿ� �ε�������Ʈ�� (����ī��Ʈ * ���ؽ��� ����12)�� ������� �Ѵ�.
							// �ε�������Ʈ�� �¿���� �����ϰ� �׸��� ���� ����Ѵ�.
							irr::u16 indexNumber = only_X_list[k] + (12 * count);

							// �Ž����ۿ� �ε��� ����Ʈ �߰�
							buffer->Indices.push_back(indexNumber);
						}
					}

					//-------------------------------
					// (3-3) ���� Z������ �þ��
					//-------------------------------
					else if( (j==0 && i<m_uCountZ) || (j==m_uCountX-1 && i<m_uCountZ) )
					{
						for(k = 0 ; k < 24 ; ++k)
						{
							// �� 1���� 12���� ���ؽ��� �ְ�, �׸�ŭ �þ�Ƿ� �ε�������Ʈ�� (����ī��Ʈ * ���ؽ��� ����12)�� ������� �Ѵ�.
							// �ε�������Ʈ�� �¿���� �����ϰ� �׸��� ���� ����Ѵ�.
							irr::u16 indexNumber = only_Z_list[k] + (12 * count);

							// �Ž����ۿ� �ε��� ����Ʈ �߰�
							buffer->Indices.push_back(indexNumber);
						}
					}

					//---------------------------------------------------------------------
					// (3-4) ���� ���ʿ� ������, �� ������ �ٸ� ������ �ѷ��׿� ������
					//---------------------------------------------------------------------
					else
					{
						for(k = 0 ; k < 12 ; ++k)
						{
							// �� 1���� 12���� ���ؽ��� �ְ�, �׸�ŭ �þ�Ƿ� �ε�������Ʈ�� (����ī��Ʈ * ���ؽ��� ����12)�� ������� �Ѵ�.
							// �ε�������Ʈ�� �¿���� �����ϰ� �׸��� ���� ����Ѵ�.
							irr::u16 indexNumber = dual_XZ_list[k] + (12 * count);

							// �Ž����ۿ� �ε��� ����Ʈ �߰�
							buffer->Indices.push_back(indexNumber);
						}
					}

					// ���ؽ���ȣ�� ���Ͽ� ī��Ʈ�� ������Ų��.
					count++;
				}
			}
		}


		//////////////////////
		//					//
		//	�Ž��� �߰��ϱ�	//
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
		// �Ž��� �̸��� ������ �Ž��̸��� �����ϴٸ� �ٽ� ���� �ʿ䰡 ����.
		// �̸��� �ٸ��ٸ� ���� ����� ������ ���Ѱ��̹Ƿ� ������ �Ž��� ���ְ� �ٽ� ������ �Ѵ�.
		//---------------------------------------------------------------------------------------------

		// �Ž��� �̸� ������
		irr::core::stringc meshName = getMeshName();

		// ���� ����Ž��� �̸��� �̹� ������� �̸��� �ٸ��ٸ� ���� �Ž��� �����Ѵ�.
		if(m_meshName != meshName)
		{
			// ������� �Ž��� �ִٸ� ����Ѵ�.
			if(Mesh)
				Mesh->drop();

			// ���� �Ž��� �����Ѵ�.
			Mesh = createBlockMesh();

			// collisionShape�� ���� �����Ѵ�.
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

		// �ؽ��� ����
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
				// ������ġ
				irr::core::array<irr::core::vector3df> follow;
				//irr::core::vector3df startPosition = this->getAbsolutePosition();
				irr::core::vector3df startPosition = m_startPosition;
				follow.push_back(startPosition);
				
				// ������ġ
				irr::core::vector3df endPosition( startPosition.X + m_fBlockPropertyValue[0], 0,  startPosition.Z + m_fBlockPropertyValue[1]);
				follow.push_back(endPosition);

				// �ִϸ����� ����
				pAnimator = SceneManager->createFollowSplineAnimator(
					0,
					follow,
					m_fBlockPropertyValue[2]
					);
			}
			break;

		case BLOCK_PROPERTY_ROTATION:
			{
				// ȸ�� ���� ����
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

				// ȸ�� �ִϸ����� ����
				pAnimator = SceneManager->createRotationAnimator(
					irr::core::vector3df(0,1,0) * direction * m_fBlockPropertyValue[1]
					);
			}
			break;
		}

		
		if(pAnimator)
		{
			// �ִϸ����� �߰�
			this->addAnimator(pAnimator);
			// �ִϸ����� ���
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