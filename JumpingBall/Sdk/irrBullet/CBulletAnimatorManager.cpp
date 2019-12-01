// coded by zet
// extend by gbox3d 2009.8.30

#include "CBulletAnimatorManager.h"

#include "ISceneNodeAnimatorCollisionResponse.h"
#include "ISceneManager.h"


#include "CBulletWorldAnimator.h"
#include "CBulletObjectAnimator.h"


extern bool gDisableDeactivation;

namespace irr
{
namespace scene
{

//! Bullet animator types count
	const s32 SceneNodeAnimatorTypeCount = ESNAT_BULLET_FOR_COUNT - ESNAT_BULLET_WORLD;
	//! Names for scene node types
	struct CSceneNodeAnimatorTypeName { ESCENE_NODE_BULLET_ANIMATOR_TYPE type; const c8* name; };
	CSceneNodeAnimatorTypeName const SceneNodeAnimatorTypeNames[] =
	{
		{ ESNAT_BULLET_WORLD, "bulletWorld" },
		{ ESNAT_BULLET_OBJECT, "bulletObject" },
		{ ESNAT_BULLET_OBJECT_FPS_CTRL, "bulletObjectFpsCtrl" },
		{ ESNAT_BULLET_OBJECT_CHACTER_CTRL, "bulletObjectCharacterCtrl" },
		{ ESNAT_BULLET_UNKNOWN,"ESNAT_BULLET_UNKNOWN" }
	};


CBulletAnimatorManager::CBulletAnimatorManager(ISceneManager* mgr)
: sceneManager(mgr)
{
	// don't grab the scene manager here to prevent cyclic references

  // register factory
  sceneManager->registerSceneNodeAnimatorFactory(this);

  gDisableDeactivation = false;
}


CBulletAnimatorManager::~CBulletAnimatorManager()
{
}



//------------------------------------------------------------------------------
//! createBulletWorldAnimator
//! Create CBulletWorldAnimatorAnimator
CBulletWorldAnimator* CBulletAnimatorManager::createBulletWorldAnimator(
  ISceneManager* sceneManager,
  ISceneNode* sceneNode, 
  CBulletWorldAnimator::Params* params)
{
  CBulletWorldAnimator* bulletWorld = 
    //CBulletWorldAnimator::createInstance(sceneManager, sceneNode, this, params);
	CBulletWorldAnimator::createInstance(sceneManager, this, params);

  if (bulletWorld != NULL)
    bulletWorldAnimators.push_back(bulletWorld);

  return bulletWorld;
}

//------------------------------------------------------------------------------
//! createEmptyBulletWorldAnimator
//! Create CBulletWorldAnimatorAnimator
CBulletWorldAnimator* CBulletAnimatorManager::createEmptyBulletWorldAnimator(
  ISceneManager* sceneManager, 
  ISceneNode* sceneNode)
{
  CBulletWorldAnimator* bulletWorld = 
    CBulletWorldAnimator::createEmptyInstance(sceneManager, sceneNode, this);

  if (bulletWorld != NULL)
    bulletWorldAnimators.push_back(bulletWorld);

  return bulletWorld;
}

//------------------------------------------------------------------------------
//! createBulletPhysicsAnimator by world id
CBulletObjectAnimator* CBulletAnimatorManager::createBulletObjectAnimator(
  ISceneManager* sceneManager, 
  ISceneNode* sceneNode, 
  s32 bulletWorldID,
  CBulletObjectAnimatorGeometry* geometry,
  CBulletObjectAnimatorParams* physicsParams)
{
  CBulletObjectAnimator* bulletObject = 
    CBulletObjectAnimator::createInstance(
      sceneManager, 
      sceneNode,
      this,
      geometry,
      physicsParams
    );

  bulletObject->AddToWorld(bulletWorldID);

  return bulletObject;
}

//------------------------------------------------------------------------------
//! createBulletPhysicsAnimator by world name

CBulletObjectAnimator* CBulletAnimatorManager::createBulletObjectAnimator(
  ISceneManager* sceneManager, 
  ISceneNode* sceneNode, 
  core::stringc strWorldName,
  CBulletObjectAnimatorGeometry* geometry,
  CBulletObjectAnimatorParams* physicsParams)
{
	

	CBulletObjectAnimator* bulletObject = 
		CBulletObjectAnimator::createInstance(
		sceneManager, 
		sceneNode,
		this,
		geometry,
		physicsParams
		);

	irr::scene::ISceneNode *pWorldNode = sceneManager->getSceneNodeFromName(strWorldName.c_str());	

	if(!pWorldNode) return NULL;

	s32 bulletWorldID;
	bulletWorldID = getBulletWorldBySceneNode(pWorldNode)->getID();
	bulletObject->AddToWorld(bulletWorldID);

	return bulletObject;
}

//------------------------------------------------------------------------------
//! createBulletPhysicsAnimator
//! Create CBulletCharaterAnimator

CBulletChracterAnimator* CBulletAnimatorManager::createBulletCharcterAnimator(
	ISceneManager* scenemanager, 
	ISceneNode* sceneNode, 
	s32 bulletWorldID,
	CBulletObjectAnimatorGeometry* geometry,
	CBulletObjectAnimatorParams* physicsParams
	)
{
	CBulletChracterAnimator* bulletObject = 
		(CBulletChracterAnimator*)CBulletChracterAnimator::createInstance(
		sceneManager, 
		sceneNode,
		this,
		geometry,
		physicsParams
		);

	bulletObject->AddToWorld(bulletWorldID);	

	return (CBulletChracterAnimator*)bulletObject;

}

//------------------------------------------------------------------------------
//! createBulletPhysicsAnimator
//! Create CBulletCharaterAnimator

CBulletFpsCamAnimator* CBulletAnimatorManager::createBulletFpsCamAnimator(
	ISceneManager* scenemanager, 
	ISceneNode* sceneNode, 
	 gui::ICursorControl *CursorControl,
	s32 bulletWorldID,
	CBulletObjectAnimatorGeometry* geometry,
	CBulletObjectAnimatorParams* physicsParams
	)
{
	CBulletFpsCamAnimator* bulletObject = 
		CBulletFpsCamAnimator::createInstance(
		sceneManager, 
		sceneNode,
		CursorControl,
		this,
		geometry,
		physicsParams
		);

	bulletObject->AddToWorld(bulletWorldID);	
	

	return (CBulletFpsCamAnimator*)bulletObject;

}

//------------------------------------------------------------------------------
//! createBulletPhysicsAnimator
//! Create CBulletWorldAnimatorAnimator
CBulletObjectAnimator* CBulletAnimatorManager::createEmptyBulletObjectAnimator(
  ISceneManager* sceneManager, 
  ISceneNode* sceneNode)
{
  CBulletObjectAnimator* bulletObject = 
    CBulletObjectAnimator::createEmptyInstance(
      sceneManager, 
      sceneNode,
      this
    );

  return bulletObject;
}

//------------------------------------------------------------------------------
//! removeWorld
//! Remove world from the list
void CBulletAnimatorManager::removeWorld(CBulletWorldAnimator* world)
{
  core::list<CBulletWorldAnimator*>::Iterator it = bulletWorldAnimators.begin();
  for (; it != bulletWorldAnimators.end(); ++it)
    if ((*it) == world)
    {
      bulletWorldAnimators.erase(it);
      break;
    }	
}

//------------------------------------------------------------------------------
//! getBulletWorldByID
//! Get Bullet world by identifier
CBulletWorldAnimator* CBulletAnimatorManager::getBulletWorldByID(s32 id)
{
	core::list<CBulletWorldAnimator*>::Iterator it = bulletWorldAnimators.begin();
	for (; it != bulletWorldAnimators.end(); ++it)
		if ((*it)->getID() == id)
			return (*it);

	return NULL;
}

//------------------------------------------------------------------------------
//! getBulletWorldBySceneNode
//! Get Bullet world by Scenenode ptr
CBulletWorldAnimator* CBulletAnimatorManager::getBulletWorldBySceneNode(irr::scene::ISceneNode *pNode)
{
	irr::core::list<ISceneNodeAnimator *>::ConstIterator it = pNode->getAnimators().begin();

	if ( (*it)->getType() == ESNAT_BULLET_WORLD )
	{
		return (CBulletWorldAnimator *)(*it);//pWorldAnim;
	}

	return NULL;
}

//------------------------------------------------------------------------------
//! createBulletPhysicsAnimator
//! Create CBulletWorldAnimatorAnimator
void CBulletAnimatorManager::OnUpdate(u32 timeMs)
{
  core::list<CBulletWorldAnimator*>::Iterator it = bulletWorldAnimators.begin();
  for (; it != bulletWorldAnimators.end(); ++it)
  {
    (*it)->OnUpdate(timeMs);
  }
}


//! creates a scene node animator based on its type id
ISceneNodeAnimator* CBulletAnimatorManager::createSceneNodeAnimator(ESCENE_NODE_ANIMATOR_TYPE type, ISceneNode* target)
{
  ESCENE_NODE_BULLET_ANIMATOR_TYPE animType = (ESCENE_NODE_BULLET_ANIMATOR_TYPE)type;

  ISceneNodeAnimator* anim = NULL;
  switch (animType)
  {
    case ESNAT_BULLET_WORLD:
    {
      // create bullet world animator
      //if (BulletWorldAnimator != NULL) { BulletWorldAnimator->drop(); BulletWorldAnimator = NULL; } 
      anim = createEmptyBulletWorldAnimator(sceneManager, target);
    } break;

    case ESNAT_BULLET_OBJECT:
    {
      // create object
      anim = createEmptyBulletObjectAnimator(sceneManager, target);
    } break;

	//나중에 확장을...
	case ESNAT_BULLET_OBJECT_FPS_CTRL:
    {
      // create object		
		//anim = new CBulletFpsCamAnimator();
	
		//anim = createBulletFpsCamAnimator(sceneManager, target);
    } break;
	case ESNAT_BULLET_OBJECT_CHACTER_CTRL:
    {
      // create object
		anim = new CBulletChracterAnimator();
		target->addAnimator(anim);
		anim->drop();
		//anim = createBulletCharcterAnimator(sceneManager, target,0,);
    } break;
  }

  if (anim && target)
  {
    target->addAnimator(anim);
  }

  return anim;
}

//! creates a scene node animator based on its type name
ISceneNodeAnimator* CBulletAnimatorManager::createSceneNodeAnimator(const c8* typeName, ISceneNode* target)
{
  ESCENE_NODE_BULLET_ANIMATOR_TYPE animType = ESNAT_BULLET_UNKNOWN;
  s32 i;
  for (i=0; i < SceneNodeAnimatorTypeCount; ++i)
  {
    if (!strcmp(typeName, SceneNodeAnimatorTypeNames[i].name) )
    {
      animType = SceneNodeAnimatorTypeNames[i].type;
      break;
    }
  }

  return createSceneNodeAnimator((ESCENE_NODE_ANIMATOR_TYPE)animType, target);
}


//! returns amount of scene node animator types this factory is able to create
#ifdef  IRR_OLD_13X
s32 CBulletAnimatorManager::getCreatableSceneNodeAnimatorTypeCount()
#else
u32 CBulletAnimatorManager::getCreatableSceneNodeAnimatorTypeCount() const
#endif
//s32 CBulletAnimatorManager::getCreatableSceneNodeAnimatorTypeCount()
{
	return SceneNodeAnimatorTypeCount;
}


//! returns type of a createable scene node animator type
#ifdef  IRR_OLD_13X
ESCENE_NODE_ANIMATOR_TYPE CBulletAnimatorManager::getCreateableSceneNodeAnimatorType(s32 idx)
#else
ESCENE_NODE_ANIMATOR_TYPE CBulletAnimatorManager::getCreateableSceneNodeAnimatorType(u32 idx) const
#endif
//ESCENE_NODE_ANIMATOR_TYPE CBulletAnimatorManager::getCreateableSceneNodeAnimatorType(s32 idx)
{
  if (idx>=0 && idx<SceneNodeAnimatorTypeCount)
    return (ESCENE_NODE_ANIMATOR_TYPE)SceneNodeAnimatorTypeNames[idx].type;

	return ESNAT_UNKNOWN;
}


//! returns type name of a createable scene node animator type 
#ifdef  IRR_OLD_13X
     const c8* CBulletAnimatorManager::getCreateableSceneNodeAnimatorTypeName(s32 idx)
#else
	 const c8* CBulletAnimatorManager::getCreateableSceneNodeAnimatorTypeName(u32 idx) const
#endif	
//const c8* CBulletAnimatorManager::getCreateableSceneNodeAnimatorTypeName(s32 idx)
{
	if (idx>=0 && idx<SceneNodeAnimatorTypeCount)
		return SceneNodeAnimatorTypeNames[idx].name;

	return 0;
}

//! returns type name of a createable scene node animator type 
#ifdef  IRR_OLD_13X
     const c8* CBulletAnimatorManager::getCreateableSceneNodeAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type)
#else
	 const c8* CBulletAnimatorManager::getCreateableSceneNodeAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type) const
#endif	
//const c8* CBulletAnimatorManager::getCreateableSceneNodeAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type)
{
  s32 i;

  for (i = 0; i < SceneNodeAnimatorTypeCount; i++)
  {
    if ((ESCENE_NODE_BULLET_ANIMATOR_TYPE)type == SceneNodeAnimatorTypeNames[i].type)
      return SceneNodeAnimatorTypeNames[i].name;
  }

	return 0;
}

} // end namespace scene
} // end namespace irr

