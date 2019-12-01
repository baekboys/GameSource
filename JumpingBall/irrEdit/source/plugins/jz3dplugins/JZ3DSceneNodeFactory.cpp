#include "jz3d.h"

//#include "CollusionMngPluginNode.h"
//#include "BulletPhysicsObjectNode.h"
#include "JZ3DSceneNodeFactory.h"
#include "ItemNode.h"
#include "PortalNode.h"
#include "BlockNode.h"
#include "PositionNode.h"
#include "EmptyBBoxNode.h"
#include "HillPlaneNode.h"

namespace irr 
{
namespace scene
{
namespace jz3d
{

	struct S_SCENENODE_DESC
	{
		const int m_nID;
		const char *m_strName;
	};

	static const S_SCENENODE_DESC g_SCN_desc[] =
	{				
		{CItemNode::TypeID, CItemNode::Name},
		{CPortalNode::TypeID, CPortalNode::Name},
		{CBlockNode::TypeID, CBlockNode::Name},
		{CPositionNode::TypeID, CPositionNode::Name},
		{CEmptyBBoxNode::TypeID, CEmptyBBoxNode::Name},
		{CHillPlaneNode::TypeID, CHillPlaneNode::Name}
	};

	CJZ3DSceneNodeFactory::CJZ3DSceneNodeFactory(irr::scene::ISceneManager *pSmgr) : Manager(pSmgr)
	{
	}

	CJZ3DSceneNodeFactory::~CJZ3DSceneNodeFactory(void)
	{
	}

	ISceneNode* CJZ3DSceneNodeFactory::addSceneNode(ESCENE_NODE_TYPE type, ISceneNode* parent)
	{
		if (!parent)
			parent = Manager->getRootSceneNode();				

		switch(type)
		{
		case CItemNode::TypeID:
			{
				CItemNode *node = new CItemNode(parent, Manager, -1);
				node->drop();
				return node;
			}
			break;

		case CPortalNode::TypeID:
			{
				CPortalNode *node = new CPortalNode(parent, Manager, -1);
				node->drop();
				return node;
			}
			break;

		case CBlockNode::TypeID:
			{
				CBlockNode *node = new CBlockNode(parent, Manager, -1);
				node->drop();
				return node;
			}
			break;

		case CPositionNode::TypeID:
			{
				CPositionNode *node = new CPositionNode(parent, Manager, -1);
				node->drop();
				return node;
			}
			break;

		case CEmptyBBoxNode::TypeID:
			{
				CEmptyBBoxNode *node = new CEmptyBBoxNode(parent, Manager, -1);
				node->drop();
				return node;
			}
			break;

		case CHillPlaneNode::TypeID:
			{
				CHillPlaneNode *node = new CHillPlaneNode(parent, Manager, -1);
				node->drop();
				return node;
			}
			break;
		}				

		return 0;
	}

	ISceneNode* CJZ3DSceneNodeFactory::addSceneNode(const c8* typeName, ISceneNode* parent)
	{
		return addSceneNode( getTypeFromName(typeName), parent );
	}

	u32 CJZ3DSceneNodeFactory::getCreatableSceneNodeTypeCount() const
	{
		return sizeof(g_SCN_desc)/sizeof(S_SCENENODE_DESC);
	}

	ESCENE_NODE_TYPE CJZ3DSceneNodeFactory::getCreateableSceneNodeType(u32 idx) const
	{
		if(idx <= getCreatableSceneNodeTypeCount())
			return (ESCENE_NODE_TYPE)g_SCN_desc[idx].m_nID;
		return ESNT_UNKNOWN;
	}

	const c8* CJZ3DSceneNodeFactory::getCreateableSceneNodeTypeName(u32 idx) const
	{
		if(idx <= getCreatableSceneNodeTypeCount())
			return g_SCN_desc[idx].m_strName;
		return 0;
	}

	const c8* CJZ3DSceneNodeFactory::getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
	{
		u32 i;
		for(i=0; i< getCreatableSceneNodeTypeCount();i++)
		{
			if(type == g_SCN_desc[i].m_nID)
				return g_SCN_desc[i].m_strName;
		}
		return 0;
	}

	ESCENE_NODE_TYPE CJZ3DSceneNodeFactory::getTypeFromName(const c8* name)
	{
		u32 i;
		for(i=0; i< getCreatableSceneNodeTypeCount();i++)
		{
			if(!strcmp(g_SCN_desc[i].m_strName,name))
				return (ESCENE_NODE_TYPE)g_SCN_desc[i].m_nID;
		}
		return ESNT_UNKNOWN;
	}

}
}
}