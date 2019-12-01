#pragma once
#include <irrlicht.h>

namespace irr 
{
namespace scene
{
namespace jz3d
{

	class CTiledPlaneNormalMapNode : public irr::scene::IMeshSceneNode
	{
	public:
		CTiledPlaneNormalMapNode(
			ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
			);
		virtual ~CTiledPlaneNormalMapNode(void);

		static const int TypeID = MAKE_IRR_ID('!','T','P','N');
		static const char *Name;
		static const char *MeshNameFormat;
		static const char *MeshNameFormatTG;

		static const c8* materialTypeList[];

		virtual void OnRegisterSceneNode();

		virtual void render();

		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		virtual video::SMaterial& getMaterial(u32 i);

		virtual u32 getMaterialCount() const;

		inline ESCENE_NODE_TYPE getType() const 
		{
			return (ESCENE_NODE_TYPE)TypeID;
		}

		static inline CTiledPlaneNormalMapNode* upcast(irr::scene::ISceneNode *pNode)
		{
			if(pNode->getType() == TypeID)
			{
				return static_cast<CTiledPlaneNormalMapNode *>(pNode);
			}

			return NULL;
		}

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

		virtual void setMesh(IMesh* mesh);

		irr::scene::IMesh *setupMesh(const char *MeshName);

		virtual IMesh* getMesh(void) { return Mesh; }

		virtual void setReadOnlyMaterials(bool readonly);

		virtual bool isReadOnlyMaterials() const;

		bool makeNormalMapping();

	private:				

		IMesh *Mesh;
		core::array<video::SMaterial> Materials;				
		core::aabbox3d<f32> Box;
		video::SMaterial tmpReadOnlyMaterial;	

		s32 PassCount;
		bool ReadOnlyMaterials;				

		void copyMaterials(irr::scene::IMesh *copyMesh = NULL);

		const char *getMeshName();

		irr::core::dimension2d<irr::u32> m_TileCount;
		irr::core::dimension2d<irr::f32> m_TileSize;

		bool m_normalmappingFlag;
		bool m_canRender;

		static bool m_hasNormalMappingTexture;

		irr::f32 m_amplitude;
		irr::f32 m_parallaxeffect;
		irr::core::stringc m_materialType;
		irr::core::stringc m_msg;
	};

}
}
}
