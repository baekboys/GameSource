#ifndef BLOCK_NODE_H
#define BLOCK_NODE_H

#include "irrlicht.h"
#include "jz3d.h"

namespace irr 
{
namespace scene
{
namespace jz3d
{
	// ���� Ÿ�� : �ؽ���, ������, ź���� ���� �����ϰ� �����ϱ� ����
	enum BLOCK_TYPE
	{
		BLOCK_WOOD,
		BLOCK_ICE,
		BLOCK_CONCRETE,
		BLOCK_BRICK,
		BLOCK_SPACE,

		BLOCK_TOTAL_COUNT
	};

	// ���� �Ӽ�
	enum BLOCK_PROPERTY
	{
		BLOCK_PROPERTY_NORMAL,			// �⺻ �� �Ӽ�
		BLOCK_PROPERTY_DESTROY,			// ������ ��
		BLOCK_PROPERTY_MOVE,				// �����̴� ��
		BLOCK_PROPERTY_ROTATION,		// ȸ�� ��
		BLOCK_PROPERTY_SPRING,			// ���� ƨ���ִ� ��
		BLOCK_PROPERTY_ILLUSION,			// �������� ���� ���� ������ ��

		BLOCK_PROPERTY_TOTAL_COUNT
	};

	// ���������� ���� ƨ��� ����
	enum BLOCK_PROPERTY_SPRING_DIRECTION
	{
		BLOCK_PROPERTY_SPRING_DIRECTION_UP,
		BLOCK_PROPERTY_SPRING_DIRECTION_DOWN,
		BLOCK_PROPERTY_SPRING_DIRECTION_LEFT,
		BLOCK_PROPERTY_SPRING_DIRECTION_RIGHT,

		BLOCK_PROPERTY_SPRING_DIRECTION_TOTAL_COUNT
	};

	// ȸ������ ȸ�� ����
	enum BLOCK_PROPERTY_ROTATING_DIRECTION
	{
		BLOCK_PROPERTY_ROTATING_DIRECTION_LEFT,
		BLOCK_PROPERTY_ROTATING_DIRECTION_RIGHT,

		BLOCK_PROPERTY_ROTATING_DIRECTION_TOTAL_COUNT
	};

	class CBlockNode : public IMeshSceneNode
	{
		static int blockCount;
		irr::scene::IMesh* Mesh;
		irr::core::array<irr::video::SMaterial> m_Matrial;

		btCollisionShape* m_pCollisionShape;

		irr::core::stringc m_meshName;

		irr::f32 m_fBlockSize;
		irr::u32 m_uCountX;
		irr::u32 m_uCountZ;

		irr::u32 m_uBlockType;
		irr::u32 m_uBlockProperty;
		irr::f32 m_fBlockPropertyValue[3];

		irr::core::vector3df m_startPosition;
		irr::core::vector3df m_startRotation;

		// ������� ���� ������ ���Ͽ� �Ž��� �̸� ������
		irr::core::stringc getMeshName();

		// ���Ž� �����ϱ�
		irr::scene::IMesh* createBlockMesh();

		// �ñ����� ��ǥ�� �� �����ϱ�
		void createBlock();

		// �ٿ���ڽ��� �̿��Ͽ� �Ž������ŭ collisionShape�� ����
		void createCollisionShape();

	public:

		irr::f32 m_fFriction;
		irr::f32 m_fRestitution;

		static const int TypeID = MAKE_IRR_ID('@','B','L','K');
		static const char *Name;

		static const wchar_t* typeNames[];
		static const wchar_t* propertyNames[];
		static const wchar_t* springBlockDirectionNames[];
		static const wchar_t* rotationBlockDirectionNames[];
		
		CBlockNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const irr::core::vector3df& position = core::vector3df(0,0,0),
			const irr::core::vector3df& rotation = core::vector3df(0,0,0),
			const irr::core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			irr::f32 blockSize = 20, irr::u32 countX = 1, irr::u32 countZ = 1);
		virtual ~CBlockNode();

		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		virtual void OnRegisterSceneNode();

		virtual void render();				

		virtual ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)TypeID; }

		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		inline virtual video::SMaterial& getMaterial(u32 num)
		{
			return m_Matrial[num];
		}

		inline virtual u32 getMaterialCount() const
		{
			return m_Matrial.size();
		}

		inline btCollisionShape* getCollisionShape()
		{
			return m_pCollisionShape;
		}

		// ����� �����ϴ� �Լ�
		inline void setSize(irr::f32 size)
		{
			if(m_fBlockSize != size)
			{
				m_fBlockSize = size;
				createBlock();
			}
		}

		// ����� ������ �Լ�
		inline irr::f32 getBlockSize()
		{
			return m_fBlockSize;
		}


		// ���� ������ �����ϴ� �Լ�
		void setBlockCount(irr::u32 countX, irr::u32 countZ);

		// IMeshSceneNode ���������Լ� ����
		virtual void setMesh(IMesh* mesh) {};
		virtual IMesh* getMesh(void) { return Mesh; }
		virtual void setReadOnlyMaterials(bool readonly) {}
		virtual bool isReadOnlyMaterials() const { return false; }

		void setType(irr::u32 type);
		
		inline irr::u32 getBlockType()
		{
			return m_uBlockType;
		}

		void setProperty(irr::u32 property) { m_uBlockProperty = property; }
		
		inline irr::u32 getBlockProperty()
		{
			return m_uBlockProperty;
		}

		void setPropertyValue(irr::u32 index, irr::f32 value) { m_fBlockPropertyValue[index] = value; }

		irr::f32 getPropertyValue(irr::u32 index) { return m_fBlockPropertyValue[index]; }

		void startAnimation();
		void deleteAnimation();
		void deleteBlockPropertyValues();
		void setStartPosition(irr::core::vector3df& position) { m_startPosition = position; }
		irr::core::vector3df getStartPosition() { return m_startPosition; }
		void setStartRotation(irr::core::vector3df& rotation) { m_startRotation = rotation; }
		irr::core::vector3df getStartRotation() { return m_startRotation; }
		
		static irr::scene::jz3d::CBlockNode* CreateBlockNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df start, irr::core::vector3df end, irr::s32 type);
	};

}
}
}

#endif