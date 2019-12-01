#pragma once
#include <irrlicht.h>

namespace irr {
	namespace scene
	{
		namespace jz3d
		{

			class CJZ3DSceneNodeFactory :
				public irr::scene::ISceneNodeFactory
			{
			public:
				CJZ3DSceneNodeFactory(irr::scene::ISceneManager *pSmgr);
				virtual ~CJZ3DSceneNodeFactory(void);

				virtual ISceneNode* addSceneNode(ESCENE_NODE_TYPE type, ISceneNode* parent=0);
				virtual ISceneNode* addSceneNode(const c8* typeName, ISceneNode* parent=0);
				virtual u32 getCreatableSceneNodeTypeCount() const;
				virtual const c8* getCreateableSceneNodeTypeName(u32 idx) const;
				virtual ESCENE_NODE_TYPE getCreateableSceneNodeType(u32 idx) const;
				virtual const c8* getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const;

			private:				

				ESCENE_NODE_TYPE getTypeFromName(const c8* name);
				ISceneManager* Manager;
			};
		}
	}
}
