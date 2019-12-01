#include "BulletCustomFilterCallback.h"

bool CBulletCustomFilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const
{
	btCollisionObject* obj1 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
	std::map<btCollisionObject*, SP_IFSMObject>::iterator obj1_it;
	obj1_it = m_objectMgr->find(obj1);

	btCollisionObject* obj2 = static_cast<btCollisionObject*>(proxy1->m_clientObject);
	std::map<btCollisionObject*, SP_IFSMObject>::iterator obj2_it;
	obj2_it = m_objectMgr->find(obj2);

	std::string obj1_typeName, obj2_typeName;

	static int count;

	if ( obj1_it != m_objectMgr->end() )
	{
		obj1_typeName = (*obj1_it).second.get()->m_strTypeName;
	}

	if ( obj2_it != m_objectMgr->end() )
	{
		obj2_typeName = (*obj2_it).second.get()->m_strTypeName;
	}

	if ( obj1_typeName == "Shell" || obj2_typeName == "Shell")
	{
		//return false;
		if ( obj1_typeName == "Enemy" || obj2_typeName == "Enemy")
		{
			++count;
			std::cout << "filter!!" << count << std::endl;
			return false;
		}
	}

	return true;
}