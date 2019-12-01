#include "TunnelSceneNode.h"

namespace irr
{
namespace scene
{
namespace jz3d
{
	const char *CTunnelSceneNode::Name = "TunnelSceneNode";

	CTunnelSceneNode::CTunnelSceneNode(ISceneNode* parent, ISceneManager* mgr,s32 id,	const core::vector3df& position,const core::vector3df& rotation, const core::vector3df& scale)
		: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
		m_TileCount(irr::core::dimension2d<irr::u32>(1,1)),
		m_TileSize(irr::core::dimension2d<irr::f32>(10,10))
	{	
	}

	CTunnelSceneNode::~CTunnelSceneNode(void)
	{
	}


}
}
}