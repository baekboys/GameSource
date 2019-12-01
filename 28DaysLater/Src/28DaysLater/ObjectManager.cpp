#include "ObjectManager.h"

CObjectManager::CObjectManager(void) : m_makedHero(false)
{
	m_objectMgr.InitHashTable(257);
}

CObjectManager::~CObjectManager(void)
{
}

void CObjectManager::Update(irr::f32 fDelta)
{

	//-------------------
	// 오브젝트 업데이트
	//-------------------

	POSITION pos = m_objectMgr.GetStartPosition();
	CAtlMap<btCollisionObject*, SP_IFSMObject>::CPair* pPair = NULL;

	while(pos)
	{
		pPair = m_objectMgr.GetNext(pos);

		if (pPair->m_value->isDie() == true)
		{
			m_objectMgr.RemoveKey(pPair->m_key);
		}
		else
		{
			static bool start = false;
			pPair->m_value->Update(fDelta);

			if(!start)
			{
				m_pHeroObject->addWeapon(m_pWeaponObject);
				start = true;
			}
		}
	}



	//--------------------
	// 오브젝트 추가 삭제
	//--------------------

	// 맵데이터에 오브젝트 추가를 위한 큐 처리
	while (!m_objectAddQueue.empty())
	{
		objectMgrData data =  m_objectAddQueue.front();
		m_objectMgr[data.colObj] = data.pObj;
		m_objectAddQueue.pop();
	}

	// 맵데이터에 오브젝트 삭제를 위한 큐 처리
	while(!m_objectRemoveQueue.empty())
	{
		btCollisionObject* key =  m_objectRemoveQueue.front();
		m_objectMgr.RemoveKey(key);
		m_objectRemoveQueue.pop();
	}

}

SP_IFSMObject CObjectManager::createObject(CObjectManager::OBJECT_TYPE type, irr::core::vector3df position, irr::u32 option)
{
	SP_IFSMObject sp_obj;
	objectMgrData data;

	switch(type)
	{
	case CObjectManager::OBJ_HERO :
		{
			CHeroObject* pObj = m_pHeroObject = new CHeroObject();
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);

			m_makedHero = true;
		}
		break;

	case CObjectManager::OBJ_ZOMBIE :
		{
			CZombieObject* pObj = new CZombieObject();
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;

	case CObjectManager::OBJ_HL2ZOMBIE :
		{
			CHL2ZombieObject* pObj = new CHL2ZombieObject();
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;

	case CObjectManager::OBJ_PLASMA_RIFLE :
		{
			CPlasmaRifleObject* pObj = new CPlasmaRifleObject();
			m_pWeaponObject = pObj;
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;

	case CObjectManager::OBJ_CARTRIGE:
		{
			CCartrigeObject* pObj = new CCartrigeObject();
			sp_obj.reset(pObj);
			pObj->Init(position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;
	}

	addObjectToMap(data);

	return sp_obj;
}

SP_IFSMObject CObjectManager::createObject(CObjectManager::OBJECT_TYPE type, irr::core::vector3df position)
{
	SP_IFSMObject sp_obj;
	objectMgrData data;

	switch(type)
	{
	case CObjectManager::OBJ_HERO :
		{
			CHeroObject* pObj = m_pHeroObject = new CHeroObject();
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);

			m_makedHero = true;
		}
		break;

	case CObjectManager::OBJ_ZOMBIE :
		{
			CZombieObject* pObj = new CZombieObject();
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;

	case CObjectManager::OBJ_HL2ZOMBIE :
		{
			CHL2ZombieObject* pObj = new CHL2ZombieObject();
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;

	case CObjectManager::OBJ_PLASMA_RIFLE :
		{
			CPlasmaRifleObject* pObj = new CPlasmaRifleObject();
			m_pWeaponObject = pObj;
			sp_obj.reset(pObj);
			pObj->Init(m_pObjSceneNode[type]->clone(0,0), position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;

	case CObjectManager::OBJ_CARTRIGE:
		{
			CCartrigeObject* pObj = new CCartrigeObject();
			sp_obj.reset(pObj);
			pObj->Init(position);

			data.colObj = pObj->getRigidBody();
			data.pObj._Reset(sp_obj);
		}
		break;
	}

	addObjectToMap(data);

	return sp_obj;
}

SP_IFSMObject CObjectManager::createDynamicObject(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode)
{
	SP_IFSMObject sp_obj;
	objectMgrData data;

	CDynamicObject* pObj = new CDynamicObject();
	sp_obj.reset(pObj);
	pObj->Init(pNode);

	data.colObj = pObj->getRigidBody();
	data.pObj._Reset(sp_obj);
	
	addObjectToMap(data);

	return sp_obj;
}

SP_IFSMObject CObjectManager::createAnimationDynamicObject(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode)
{
	SP_IFSMObject sp_obj;
	objectMgrData data;

	CAnimationDynamicObject* pObj = new CAnimationDynamicObject();
	sp_obj.reset(pObj);
	pObj->Init(pNode);

	data.colObj = pObj->getRigidBody();
	data.pObj._Reset(sp_obj);

	addObjectToMap(data);

	return sp_obj;
}