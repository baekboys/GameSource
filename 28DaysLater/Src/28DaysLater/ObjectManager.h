#pragma once

#include "stdLibrary.h"
#include "ObjectList.h"

const int objectCount = 3;

class CObjectManager
{
public:
	CObjectManager(void);
	~CObjectManager(void);

	enum OBJECT_TYPE 
	{ 
		OBJ_HERO, 
		OBJ_ZOMBIE,
		OBJ_HL2ZOMBIE ,
		OBJ_PLASMA_RIFLE,
		OBJ_CARTRIGE,
		OJB_DYNAMIC,
		OBJ_TOTAL_COUNT };

	inline void addObjectToMap(objectMgrData data)
	{
		m_objectAddQueue.push(data);
	}
	inline void removeObjectFromMap(btCollisionObject* key)
	{
		m_objectRemoveQueue.push(key);
	}

	inline void addSceneNode(OBJECT_TYPE type, irr::scene::ISceneNode* pNode)
	{
		m_pObjSceneNode[type] = pNode;
		pNode->setVisible(false);
	}

	inline CAtlMap<btCollisionObject*, SP_IFSMObject>* getObjectData()
	{
		return &m_objectMgr;
	}

	inline bool makedHero()
	{
		return m_makedHero;
	}

	void Update(irr::f32 fDelta);

	SP_IFSMObject createObject(OBJECT_TYPE type, irr::core::vector3df position);
	SP_IFSMObject createObject(OBJECT_TYPE type, irr::core::vector3df position, irr::u32 option);
	SP_IFSMObject createDynamicObject(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode);
	SP_IFSMObject createAnimationDynamicObject(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode);

	// 추가
	inline CHeroObject* getHeroObject() { return m_pHeroObject; }
	inline IWeaponObject* getWeaponObject() { return m_pWeaponObject; }

private:
	bool m_makedHero;

	// 추가
	CHeroObject* m_pHeroObject;
	IWeaponObject* m_pWeaponObject;

	CAtlMap<btCollisionObject*, SP_IFSMObject> m_objectMgr;
	std::queue<objectMgrData> m_objectAddQueue;
	std::queue<btCollisionObject*> m_objectRemoveQueue;

	//--------
	// 신노드
	//--------
	irr::scene::ISceneNode* m_pObjSceneNode[OBJ_TOTAL_COUNT];
};
