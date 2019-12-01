#include "CollisionManager.h"

bool compareCollisionData(collisionData data1, collisionData data2)
{
	return data1.farValue < data2.farValue;
}

bool compareCollisionMeleeData(collisionMeleeData data1, collisionMeleeData data2)
{
	return data1.hitDistance < data2.hitDistance;
}

void CCollisionManager::hitTest(irr::core::vector3df rayFrom, irr::core::vector3df rayTo, irr::f32 raySize)
{
	// 테스트를 시작하므로 충돌결과 플래그를 false로 선언
	hitFlag = false;
	
	// 필요한 변수 선언.
	std::vector<collisionData> vData;
	btVoronoiSimplexSolver	simplexSolver;
	btScalar closestHitResults = 1.f;
	btConvexCast::CastResult rayResult;
	btSphereShape pointShape(raySize);

	// (1) 일리히트엔진의 벡터를 불릿엔진의 벡터로 변환
	btVector3 vStart;
	btVector3 vEnd;
	Irrlicht2Bullet(rayFrom, vStart);
	Irrlicht2Bullet(rayTo, vEnd);

	// (2) 불릿엔진 매트릭스 자료 선언.
	btTransform rayFromTrans;
	btTransform rayToTrans;

	// (3) 파라미터로 받아온 불릿벡터를 매트릭스형으로 설정.
	rayFromTrans.setIdentity();
	rayFromTrans.setOrigin(vStart);
	rayToTrans.setIdentity();
	rayToTrans.setOrigin(vEnd);

	/*objTranform.setIdentity();
	objTranform.setOrigin(btVector3(3,1,1)); 
	objTranform.setRotation(btQuaternion(btVector3(0,1,0),irr::core::PI * .25f));*/

	std::map<std::string, irr::scene::jz3d::CCollusionMngNode*>::iterator it = m_mapCollisionNode.begin();
	for (; it != m_mapCollisionNode.end(); ++it)
	{
		irr::scene::jz3d::CCollusionMngNode* pNode = (*it).second;
		pNode->OnAnimate(0);
		
		btConvexShape* targetShape = static_cast<btConvexShape*>(pNode->getCollisionShape());

		btVector3 shapePos; 
		btVector3 shapeRot;

		//irr::core::vector3df position;
		//irr::core::vector3df rotation;
		irr::core::vector3df position = pNode->getCollisionShapeTransform().getTranslation();
		irr::core::vector3df rotation = pNode->getCollisionShapeTransform().getRotationDegrees() * irr::core::DEGTORAD;
		
		Irrlicht2Bullet(position, shapePos);
		Irrlicht2Bullet(rotation, shapeRot);
		btQuaternion shapeQt(shapeRot.x(), shapeRot.y(), shapeRot.z());

		btTransform objTranform;
		objTranform.setIdentity();
		objTranform.setOrigin(shapePos); 
		objTranform.setRotation(shapeQt);

		btSubsimplexConvexCast convexCaster(&pointShape, targetShape,&simplexSolver);

		if ( convexCaster.calcTimeOfImpact(rayFromTrans, rayToTrans, objTranform, objTranform, rayResult))
		{
			hitFlag = true;

			collisionData data;

			data.hitDir = (rayTo - rayFrom).normalize();

			rayResult.m_hitTransformA.setIdentity();
			rayResult.m_hitTransformA.getOrigin().setInterpolate3(
				rayFromTrans.getOrigin(), rayToTrans.getOrigin(), rayResult.m_fraction);

			Bullet2Irrlicht(rayResult.m_hitTransformA.getOrigin(), data.hitPoint);
			Bullet2Irrlicht(rayResult.m_normal, data.hitNormal);

			//m_hitPoint = rayResult.m_hitPoint'
			//m_hitNormal = rayResult.m_normal;

			data.hitNodeName = pNode->getName();
			data.hitCollisionShape = pNode->getCollisionShape();

			data.farValue = rayFrom.getDistanceFrom(data.hitPoint);

			vData.push_back(data);
		}
	}

	// 충돌이 여러개이면 가장 가까운 충돌지점으로 정렬함.
	if ( vData.size() > 1 )
		sort(vData.begin(), vData.end(), compareCollisionData);

	// 충돌이 있어났으면 정렬한 데이터중 [0]번째, 즉 가장 가까운 결과물로 설정.
	if ( hitFlag )
		m_resultData = vData[0];
}

void CCollisionManager::hitTestMelee(irr::scene::jz3d::CCollusionMngNode* pNode)
{
	// 테스트를 시작하므로 충돌결과 플래그를 false로 선언
	hitFlag = false;
	
	// 필요한 변수 선언.
	std::vector<collisionMeleeData> vData;

	// 최단점 찾기
	btVoronoiSimplexSolver sGjkSimplexSolver;
	// 파고들어간정도 구하기
	btGjkEpaPenetrationDepthSolver epaSolver;
	
	// 결과 변수
	btPointCollector gjkOutput; 


	// 타격을 하는 노드의 collisionShape 구하기
	btCollisionShape *shape1 = pNode->getCollisionShape();
	btTransform shape_trans1;
	shape_trans1.setFromOpenGLMatrix(pNode->getCollisionShapeTransform().pointer());

	// 맵데이터에서 CollisionMngNode를 순회하며 충돌 테스트 하기
	std::map<std::string, irr::scene::jz3d::CCollusionMngNode*>::iterator it = m_mapCollisionNode.begin();
	for (; it != m_mapCollisionNode.end(); ++it)
	{
		// 타겟노드
		irr::scene::jz3d::CCollusionMngNode* pTargetNode = (*it).second;

		// 타겟노드의 collisionShape
		btCollisionShape *shape2 = pTargetNode->getCollisionShape();
		btTransform shape_trans2;
		shape_trans2.setFromOpenGLMatrix(pTargetNode->getCollisionShapeTransform().pointer());
		

		//최단거리구하기
		btGjkPairDetector convexConvex(
			static_cast<const btConvexShape *>(shape1),
			static_cast<const btConvexShape *>(shape2),						
			&sGjkSimplexSolver,
			&epaSolver); 

		btGjkPairDetector::ClosestPointInput input; 
		input.m_transformA = shape_trans1;
		input.m_transformB = shape_trans2;

		convexConvex.getClosestPoints(input, gjkOutput, 0);					

		// 거리가 0이하라면, 즉 충돌하였다면
		if(gjkOutput.m_distance < 0.f)
		{
			hitFlag = true;

			// 결과데이터
			collisionMeleeData data;

			irr::core::vector3df hitPoint, hitNormal;

			Bullet2Irrlicht(gjkOutput.m_pointInWorld, hitPoint);
			Bullet2Irrlicht(gjkOutput.m_normalOnBInWorld, hitNormal);

			// 충돌지점
			data.hitPoint = hitPoint;
			
			// 법선벡터
			data.hitNormal = hitNormal;
			
			// 파고들어간 거리
			data.hitDistance = gjkOutput.m_distance;
			
			// 충돌된 노드명
			data.hitNodeName = (*it).first;

			// 충돌데이터 저장
			vData.push_back(data);
		}
	}

	// 충돌이 여러개이면 가장 가까운 충돌지점으로 정렬함.
	if ( vData.size() > 1 )
		sort(vData.begin(), vData.end(), compareCollisionMeleeData);

	// 충돌이 있어났으면 정렬한 데이터중 [0]번째, 즉 가장 가까운 결과물로 설정.
	if ( hitFlag )
		m_resultMeleeData = vData[0];
}