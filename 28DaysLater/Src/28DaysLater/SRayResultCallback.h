#pragma once

struct	RayMultiResultCallback : public btCollisionWorld::RayResultCallback
{
	RayMultiResultCallback() : m_rayFromWorld (btVector3(0,0,0)), m_rayToWorld (btVector3(0,0,0))
	{
	}

	RayMultiResultCallback(const btVector3&	rayFromWorld, const btVector3& rayToWorld)
		: m_rayFromWorld(m_rayFromWorld), m_rayToWorld (m_rayFromWorld)
	{
	}

	btVector3	m_rayFromWorld;
	btVector3	m_rayToWorld;

	btAlignedObjectArray<btVector3>	m_hitPointWorlds;
	btAlignedObjectArray<btCollisionWorld::LocalRayResult> m_RayResults;

	virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		m_closestHitFraction = 1.0f;

		btVector3 hitPointWorld;
		
		//��ŷ��ġ ���ϱ�
		hitPointWorld.setInterpolate3(m_rayFromWorld,m_rayToWorld,rayResult.m_hitFraction);
		//��ŷ��ġ ������ ����
		m_hitPointWorlds.push_back(hitPointWorld);

		//��������� ����
		m_RayResults.push_back(rayResult);

		m_collisionObject = rayResult.m_collisionObject;
		
		return 1.0f;
	}

	inline void setRayVector(btVector3& rayFromWorld, btVector3& rayToWorld)
	{
		m_rayFromWorld = rayFromWorld;
		m_rayToWorld = rayToWorld;
	}

	inline void reset()
	{
		m_rayFromWorld = btVector3(0,0,0);
		m_rayToWorld = btVector3(0,0,0);
		
		m_hitPointWorlds.clear();
		m_RayResults.clear();
	}
};