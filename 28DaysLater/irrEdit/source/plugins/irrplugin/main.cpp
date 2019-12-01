#include <irrlicht.h>
#include "../../irrEditLib/include/irrEdit.h"

using namespace irr;
using namespace scene;

class CSampleTestSceneNode : public ISceneNode
{
	irr::core::aabbox3df m_box;
	irr::core::array<irr::video::SMaterial> m;
public :
	static char* name;
	static const int sn_id1 = MAKE_IRR_ID('s', 'm', 'p', 't');

	CSampleTestSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id) : ISceneNode(parent, mgr ,id)
	{
		m_box.MaxEdge = irr::core::vector3df(5, 5, 5);
		m_box.MinEdge = irr::core::vector3df(-5, -5, -5);

		irr::video::SMaterial material;
		m.push_back( material );
		m.push_back( material );
	}

	inline void OnRegisterSceneNode()
	{
		if(IsVisible)
		{
			SceneManager->registerNodeForRendering(this);
		}
		ISceneNode::OnRegisterSceneNode();
	}
	inline void render()
	{
		irr::video::SMaterial sm;
		sm.Lighting = false;
		irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(sm);
		driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
		driver->draw3DBox(m_box, irr::video::SColor(255,255,0,0));

		irr::video::S3DVertex vertex[3];
		vertex[0] = irr::video::S3DVertex(10, 0, 0, 0, 0, 0, video::SColor(255,255,255,255), 1,1);
		vertex[1] = irr::video::S3DVertex(0, 10, 0, 0, 0, 0, video::SColor(255,255,255,255), 0.5, 0);
		vertex[2] = irr::video::S3DVertex(-10,0, 0, 0, 0, 0, video::SColor(255,255,255,255), 0,1);

		driver->setMaterial( m[0] );

		u16 indices[] = { 2,1,0 };
		driver->drawIndexedTriangleList(vertex, 3, indices, 1);

		irr::core::matrix4 mat;
		mat.setTranslation( irr::core::vector3df(10,0,0) );
		mat = AbsoluteTransformation * mat;
		driver->setTransform( irr::video::ETS_WORLD, mat);
		driver->setMaterial( m[1] );
		driver->drawIndexedTriangleList(vertex, 3, indices, 1);
	}
	const core::aabbox3df& getBoundingBox() const { return m_box; };
	ESCENE_NODE_TYPE getType() const { return (ESCENE_NODE_TYPE)sn_id1; };
	inline void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
	{
		ISceneNode::serializeAttributes(out, options);
		out->addVector3d("box", m_box.getExtent());
	}

	inline void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		irr::core::vector3df box_ext = in->getAttributeAsVector3d("box");
		m_box = irr::core::aabbox3df(-box_ext/2.f, box_ext/2.f);

		ISceneNode::deserializeAttributes(in, options);
	}

	inline ISceneNode* clone(ISceneNode* newParent, ISceneManager* newManager)
	{
		if( !newParent )
			newParent = Parent;
		if( !newManager )
			newManager = SceneManager;
		CSampleTestSceneNode* cloneNode = new CSampleTestSceneNode(newParent, newManager, -1);
		cloneNode->cloneMembers(this, newManager);
		cloneNode->m_box = m_box;
		cloneNode->drop();
		return cloneNode;
	}

	irr::video::SMaterial& getMaterial(u32 num)
	{
		return m[num];
	}

	irr::u32 getMaterialCount() const
	{
		return m.size();
	}
};
char* CSampleTestSceneNode::name = "sampleTest";
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class TestSceneFactory : public irr::scene::ISceneNodeFactory
{
	irr::scene::ISceneManager* mgr;
public:
	TestSceneFactory(irr::scene::ISceneManager* manager) : mgr(manager)
	{}
	
	virtual irr::scene::ISceneNode* addSceneNode(irr::scene::ESCENE_NODE_TYPE type, ISceneNode* parent)
	{ return 0; }
	
	virtual ISceneNode* addSceneNode(const c8* typeName, ISceneNode* parent=0)
	{
		if( parent == 0)
		{
			parent = mgr->getRootSceneNode();
		}
		if( !strcmp(typeName, CSampleTestSceneNode::name) )
		{
			CSampleTestSceneNode* pNode = new CSampleTestSceneNode(parent, mgr, -1);
			pNode->drop();
		}
		return 0;
	}

	virtual u32 getCreatableSceneNodeTypeCount() const
	{ return 1;}
	
	virtual ESCENE_NODE_TYPE getCreateableSceneNodeType(u32 idx) const
	{ 
		return (ESCENE_NODE_TYPE)CSampleTestSceneNode::sn_id1;
	}

	virtual const c8* getCreateableSceneNodeTypeName(u32 idx) const
	{
		static char* name = "sampleTest";
		return name;
	}

	virtual const c8* getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
	{
		if( type == CSampleTestSceneNode::sn_id1 )
		{
			static char* name = "sampleTest";
			return name;
		}
		return 0;
	}
};

__declspec(dllexport) irr::irredit::IIrrEditPlugin* __stdcall createPlugin( irr::irredit::IrrEditServices* service )
{
	irr::scene::ISceneManager* mgr = service->getDevice()->getSceneManager();
	service->getDevice()->getLogger()->log("hello\n");

	TestSceneFactory* factory = new TestSceneFactory(mgr);
	mgr->registerSceneNodeFactory(factory);
	factory->drop();
	return 0;
}