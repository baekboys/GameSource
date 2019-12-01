
#include "jz3d.h"

#include "Plan1x1SceneNode.h"
#include "TiledPlaneNode.h"
#include "FormatedAnimationNode.h"
#include "EmptyPluginNode.h"
#include "CollusionMngPluginNode.h"
#include "BulletPhysicsObjectNode.h"
#include "JZ3DSceneNodeFactory.h"
#include "CylinderSceneNode.h"
#include "TiledPlaneNormalMapNode.h"
#include "CABVSceneNode.h"
//#include "BulletPhysicsObjPluginNode.h"
//#include "BulletPhysicsWorldPluginNode.h"

#include <string>
#include <map>

namespace irr {
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
				{CPlan1x1SceneNode::TypeID,"Plane1x1"},
				{CTiledPlaneNode::TypeID,CTiledPlaneNode::Name},
				{CFormatedAnimationNode::TypeID,CFormatedAnimationNode::Name},
				{CEmptyPluginNode::TypeID,CEmptyPluginNode::Name},
				{CCollusionMngNode::TypeID,CCollusionMngNode::Name},
				{CBulletPhysicsObjectNode::TypeID,CBulletPhysicsObjectNode::Name},
				{CylinderSceneNode::SN_ID,CylinderSceneNode::Name},
				{CTiledPlaneNormalMapNode::TypeID,CTiledPlaneNormalMapNode::Name},
				{CABVSceneNode::TypeID,CABVSceneNode::Name}
				//{CBulletPhysicsWorldPluginNode::TypeID, CBulletPhysicsWorldPluginNode::Name},
				//{CBulletPhysicsObjPluginNode::TypeID, CBulletPhysicsObjPluginNode::Name}
			};



			CJZ3DSceneNodeFactory::CJZ3DSceneNodeFactory(irr::scene::ISceneManager *pSmgr)
				:Manager(pSmgr)
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
				case CPlan1x1SceneNode::TypeID:
					{
						CPlan1x1SceneNode *node = new CPlan1x1SceneNode(parent, Manager, -1);					
						node->drop();
						return node; 
					}
					break;
				case CTiledPlaneNode::TypeID:
					{
						CTiledPlaneNode *node = new CTiledPlaneNode(parent, Manager, -1);
						node->drop();
						return node; 
					}
					break;
				case CFormatedAnimationNode::TypeID:
					{
						CFormatedAnimationNode *node = new CFormatedAnimationNode(parent, Manager, -1);					
						node->drop();
						return node; 
					}
					break;
				case CCollusionMngNode::TypeID:
					{
						CCollusionMngNode *node = new CCollusionMngNode(parent, Manager, -1);
						node->drop();
						return node; 						
					}
					break;
				case CEmptyPluginNode::TypeID:				
					{
						CEmptyPluginNode *node = new CEmptyPluginNode(parent, Manager, -1);					
						node->drop();
						return node; 						
					}
					break;
				case CBulletPhysicsObjectNode::TypeID:					
					{
						CBulletPhysicsObjectNode *node = new CBulletPhysicsObjectNode(parent, Manager, -1);					
						node->drop();
						return node; 						
					}
					break;

				case CylinderSceneNode::SN_ID:
					{
						CylinderSceneNode *pNode = new CylinderSceneNode(parent,Manager,-1);
						pNode->drop();
						return pNode;
					}
					break;

				case CTiledPlaneNormalMapNode::TypeID:
					{
						CTiledPlaneNormalMapNode *node = new CTiledPlaneNormalMapNode(parent, Manager, -1);					
						node->drop();
						return node; 
					}
					break;

				case CABVSceneNode::TypeID:
					{
						CABVSceneNode *node = new CABVSceneNode(parent, Manager, -1);
						node->drop();
						return node;
					}
					break;

				/*case CBulletPhysicsWorldPluginNode::TypeID:
					{
						CBulletPhysicsWorldPluginNode *node = new CBulletPhysicsWorldPluginNode(parent, Manager, -1);
						node->drop();
						return node;
					}
					break;

				case CBulletPhysicsObjPluginNode::TypeID:
					{
						CBulletPhysicsObjPluginNode *node = new CBulletPhysicsObjPluginNode(parent, Manager, -1);
						node->drop();
						return node;
					}
					break;*/
				}				

				return 0;
			}


		//! adds a scene node to the scene graph based on its type name
		ISceneNode* CJZ3DSceneNodeFactory::addSceneNode(const c8* typeName, ISceneNode* parent)
		{
			return addSceneNode( getTypeFromName(typeName), parent );
		}


		//! returns amount of scene node types this factory is able to create
		u32 CJZ3DSceneNodeFactory::getCreatableSceneNodeTypeCount() const
		{
			//return 1;
			return sizeof(g_SCN_desc)/sizeof(S_SCENENODE_DESC);
		}


		//! returns type of a createable scene node type
		ESCENE_NODE_TYPE CJZ3DSceneNodeFactory::getCreateableSceneNodeType(u32 idx) const
		{
			//if (idx==0)
			//return (ESCENE_NODE_TYPE)EXAMPLE_SCENE_NODE_ID;
			if(idx <= getCreatableSceneNodeTypeCount())
				return (ESCENE_NODE_TYPE)g_SCN_desc[idx].m_nID;

			return ESNT_UNKNOWN;
		}


		//! returns type name of a createable scene node type 
		const c8* CJZ3DSceneNodeFactory::getCreateableSceneNodeTypeName(u32 idx) const
		{
			if(idx <= getCreatableSceneNodeTypeCount())
				return g_SCN_desc[idx].m_strName;

			//if (idx==0)
			//	return exampleSceneNodeTypeName;

			return 0;
		}


		//! returns type name of a createable scene node type 
		const c8* CJZ3DSceneNodeFactory::getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
		{
			//if (type == EXAMPLE_SCENE_NODE_ID)
			//	return exampleSceneNodeTypeName;

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
			//if (!strcmp(name, exampleSceneNodeTypeName))
			//	return (ESCENE_NODE_TYPE)EXAMPLE_SCENE_NODE_ID;
			//return ESNT_UNKNOWN;

			u32 i;
			for(i=0; i< getCreatableSceneNodeTypeCount();i++)
			{
				//if(type == g_SCN_desc[i].m_nID)
				if(!strcmp(g_SCN_desc[i].m_strName,name))
					return (ESCENE_NODE_TYPE)g_SCN_desc[i].m_nID;
			}

			return ESNT_UNKNOWN;
		}

	}
}

}