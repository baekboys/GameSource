#ifndef ITEM_NODE_H
#define ITEM_NODE_H

#include "irrlicht.h"

namespace irr 
{
namespace scene
{
namespace jz3d 
{
	enum ITEM_TYPE
	{
		CHANGE_TYPE,
		JUMP,
		SIZE,
		/*DIRECTION,*/
		TIME,
		SPEED,
		ZERO_GRAVITY,

		ITEM_TOTAL_COUNT
	};

	class CItemNode : public IMeshSceneNode
	{
		static int itemCount;

		irr::scene::IMesh* Mesh;
		irr::core::array<irr::video::SMaterial> m_Matrial;

		irr::f32 m_fCubeSize;

		// 큐브매쉬 생성하기
		irr::scene::IMesh* createCubeMesh();

		// 바운딩박스
		core::aabbox3d<f32> m_Box;

	public:

		static const wchar_t* typeNames[];

		//irr::core::stringw m_itemType;
		irr::s32 m_sItemType;
		irr::f32 m_fValue1;
		irr::f32 m_fDurationTime;

		static const int TypeID = MAKE_IRR_ID('@','I','T','M');
		static const char *Name;

		CItemNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const irr::core::vector3df& position = core::vector3df(0,0,0),
			const irr::core::vector3df& rotation = core::vector3df(0,0,0),
			const irr::core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			irr::f32 cubeSize = 9.f
			);

		virtual ~CItemNode();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		inline virtual video::SMaterial& getMaterial(u32 num)
		{
			return m_Matrial[num];
		}

		inline virtual u32 getMaterialCount() const
		{
			return m_Matrial.size();
		}

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

		// IMeshSceneNode 순수가상함수 구현
		virtual void setMesh(IMesh* mesh) {};
		virtual IMesh* getMesh(void) { return Mesh; }
		virtual void setReadOnlyMaterials(bool readonly) {}
		virtual bool isReadOnlyMaterials() const { return false; }

		void setType(irr::s32 type);

		void setSize(irr::f32 size);

		inline irr::s32 getItemType()
		{
			return m_sItemType;
		}

		static irr::scene::jz3d::CItemNode* CreateItemNode(ISceneNode* parent, ISceneManager* mgr, s32 id, irr::core::vector3df position);
	};

}
}
}

#endif