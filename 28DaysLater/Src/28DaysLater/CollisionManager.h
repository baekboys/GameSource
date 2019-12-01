#pragma once

#include <algorithm>
#include "StdLibrary.h"
#include "BtEngine.h"
#include "FactoryHeader.h"

// 광선충돌 결과데이터
struct collisionData
{
	irr::core::vector3df hitDir;
	irr::core::vector3df hitPoint;
	irr::core::vector3df hitNormal;
	btCollisionShape* hitCollisionShape;
	std::string hitNodeName;
	irr::f32 farValue;
};

// collisionShape 충돌 결과데이터 (근접공격타입)
struct collisionMeleeData
{
	std::string hitNodeName;
	irr::core::vector3df hitPoint;
	irr::core::vector3df hitNormal;
	irr::f32 hitDistance;
};

class CCollisionManager
{
	// 최종 광선충돌 테스트 결과
	collisionData m_resultData;
	
	// 최종 근접충돌 테스트 결과
	collisionMeleeData m_resultMeleeData;
	
	// collisionMngNode 데이터 저장
	std::map<std::string, irr::scene::jz3d::CCollusionMngNode*> m_mapCollisionNode;

	// 히트하였는가 체크하는 플래그
	bool hitFlag;

public :
	CCollisionManager() : hitFlag (false)
	{
	}
	~CCollisionManager() {};

	// 맵에 데이터 추가
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