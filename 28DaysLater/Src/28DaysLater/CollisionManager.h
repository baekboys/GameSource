#pragma once

#include <algorithm>
#include "StdLibrary.h"
#include "BtEngine.h"
#include "FactoryHeader.h"

// �����浹 ���������
struct collisionData
{
	irr::core::vector3df hitDir;
	irr::core::vector3df hitPoint;
	irr::core::vector3df hitNormal;
	btCollisionShape* hitCollisionShape;
	std::string hitNodeName;
	irr::f32 farValue;
};

// collisionShape �浹 ��������� (��������Ÿ��)
struct collisionMeleeData
{
	std::string hitNodeName;
	irr::core::vector3df hitPoint;
	irr::core::vector3df hitNormal;
	irr::f32 hitDistance;
};

class CCollisionManager
{
	// ���� �����浹 �׽�Ʈ ���
	collisionData m_resultData;
	
	// ���� �����浹 �׽�Ʈ ���
	collisionMeleeData m_resultMeleeData;
	
	// collisionMngNode ������ ����
	std::map<std::string, irr::scene::jz3d::CCollusionMngNode*> m_mapCollisionNode;

	// ��Ʈ�Ͽ��°� üũ�ϴ� �÷���
	bool hitFlag;

public :
	CCollisionManager() : hitFlag (false)
	{
	}
	~CCollisionManager() {};

	// �ʿ� ������ �߰�
	inline void add(std::string nodeName, irr::scene::jz3d::CCollusionMngNode* pNode)
	{
		m_mapCollisionNode[nodeName] = pNode;
	}

	inline bool hasHit()
	{
		return hitFlag;
	}

	inline collisionData getCollisionData()
	{
		return m_resultData;
	}

	inline collisionMeleeData getCollisionMeleeData()
	{
		return m_resultMeleeData;
	}
		
	void hitTest(irr::core::vector3df rayFrom, irr::core::vector3df rayTo, irr::f32 raySize);
	void hitTestMelee(irr::scene::jz3d::CCollusionMngNode* pNode);
};