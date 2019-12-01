#pragma once

#include "App.h"

class CBulletCustomFilterCallback : public btOverlapFilterCallback
{
	std::map<btCollisionObject*, SP_IFSMObject>* m_objectMgr;

public:

	CBulletCustomFilterCallback(std::map<btCollisionObject*, SP_IFSMObject>* objectMgr)
	{
		m_objectMgr = objectMgr;
	}
	~CBulletCustomFilterCallback(void) { };

	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const;
};
