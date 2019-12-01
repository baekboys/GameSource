#include <irrlicht.h>
#include <irrEdit.h>

#include "jz3dSceneNodeFactory.h"

// this example shows how to add own scene nodes into irrEdit.
// You only need two things:
// - a scene node factory 
// - your scene node.
// you can add as many scene nodes with a plugin as you wish.


// entry point for the plugin, will be called by the editor. Return a
// plugin interface if your plugin should run as plugin inside the editor, otherwise
// just modify the engine (add factories etc) and return 0.
__declspec(dllexport) irr::irredit::IIrrEditPlugin* __stdcall createPlugin( irr::irredit::IrrEditServices* services )
{
	// we simple register a new scene node factory here, which is only able to
	// create our example scene node
	
	irr::scene::ISceneManager* mgr = services->getDevice()->getSceneManager();

	irr::scene::jz3d::CJZ3DSceneNodeFactory *factory = new irr::scene::jz3d::CJZ3DSceneNodeFactory(mgr);
	
	mgr->registerSceneNodeFactory(factory);
	factory->drop();

	return 0;
}
