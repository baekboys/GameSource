#pragma once
#include <irrlicht.h>

namespace irr
{
namespace scene
{
namespace jz3d
{

	class CTunnelSceneNode : public irr::scene::IMeshSceneNode
	{
	public:

		static const int TypeID = MAKE_IRR_ID('@','T','S','N');
		static const char *Name;

		CTunnelSceneNode(
			ISceneNode* parent, ISceneManager* mgr, s32 id,
					const core::vector3df& position = core::vector3df(0,0,0),
					const core::vector3df& rotation = core::vector3df(0,0,0),
					const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
			);
		virtual ~CTunnelSceneNode(void);

		virtual void OnRegisterSceneNode();

		virtual void render();

		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		virtual video::SMaterial& getMaterial(u32 i);

		virtual u32 getMaterialCount() const;

		inline ESCENE_NODE_TYPE getType() const 
		{
			return (ESCENE_NODE_TYPE)TypeID;
		}

		static inline CTunnelSceneNode* upcast(irr::scene::ISceneNode *pNode)
		{
			if(pNode->getType() == TypeID)
			{
				return static_cast<CTunnelSceneNode *>(pNode);
			}

			return NULL;
		}

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

		virtual void setMesh(IMesh* mesh);

		virtual IMesh* getMesh(void) { return Mesh; }

	private:				

		IMesh *Mesh;
		core::array<video::SMaterial> Materials;				
		core::aabbox3d<f32> Box;
		
		irr::core::dimension2d<irr::u32> m_TileCount;
		irr::core::dimension2d<irr::f32> m_TileSize;
	};

}
}
}