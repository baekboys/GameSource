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
	// �׽�Ʈ�� �����ϹǷ� �浹��� �÷��׸� false�� ����
	hitFlag = false;
	
	// �ʿ��� ���� ����.
	std::vector<collisionData> vData;
	btVoronoiSimplexSolver	simplexSolver;
	btScalar closestHitResults = 1.f;
	btConvexCast::CastResult rayResult;
	btSphereShape pointShape(raySize);

	// (1) �ϸ���Ʈ������ ���͸� �Ҹ������� ���ͷ� ��ȯ
	btVector3 vStart;
	btVector3 vEnd;
	Irrlicht2Bullet(rayFrom, vStart);
	Irrlicht2Bullet(rayTo, vEnd);

	// (2) �Ҹ����� ��Ʈ���� �ڷ� ����.
	btTransform rayFromTrans;
	btTransform rayToTrans;

	// (3) �Ķ���ͷ� �޾ƿ� �Ҹ����͸� ��Ʈ���������� ����.
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

	// �浹�� �������̸� ���� ����� �浹�������� ������.
	if ( vData.size() > 1 )
		sort(vData.begin(), vData.end(), compareCollisionData);

	// �浹�� �־���� ������ �������� [0]��°, �� ���� ����� ������� ����.
	if ( hitFlag )
		m_resultData = vData[0];
}

void CCollisionManager::hitTestMelee(irr::scene::jz3d::CCollusionMngNode* pNode)
{
	// �׽�Ʈ�� �����ϹǷ� �浹��� �÷��׸� false�� ����
	hitFlag = false;
	
	// �ʿ��� ���� ����.
	std::vector<collisionMeleeData> vData;

	// �ִ��� ã��
	btVoronoiSimplexSolver sGjkSimplexSolver;
	// �İ������ ���ϱ�
	btGjkEpaPenetrationDepthSolver epaSolver;
	
	// ��� ����
	btPointCollector gjkOutput; 


	// Ÿ���� �ϴ� ����� collisionShape ���ϱ�
	btCollisionShape *shape1 = pNode->getCollisionShape();
	btTransform shape_trans1;
	shape_trans1.setFromOpenGLMatrix(pNode->getCollisionShapeTransform().pointer());

	// �ʵ����Ϳ��� CollisionMngNode�� ��ȸ�ϸ� �浹 �׽�Ʈ �ϱ�
	std::map<std::string, irr::scene::jz3d::CCollusionMngNode*>::iterator it = m_mapCollisionNode.begin();
	for (; it != m_mapCollisionNode.end(); ++it)
	{
		// Ÿ�ٳ��
		irr::scene::jz3d::CCollusionMngNode* pTargetNode = (*it).second;

		// Ÿ�ٳ���� collisionShape
		btCollisionShape *shape2 = pTargetNode->getCollisionShape();
		btTransform shape_trans2;
		shape_trans2.setFromOpenGLMatrix(pTargetNode->getCollisionShapeTransform().pointer());
		

		//�ִܰŸ����ϱ�
		btGjkPairDetector convexConvex(
			static_cast<const btConvexShape *>(shape1),
			static_cast<const btConvexShape *>(shape2),						
			&sGjkSimplexSolver,
			&epaSolver); 

		btGjkPairDetector::ClosestPointInput input; 
		input.m_transformA = shape_trans1;
		input.m_transformB = shape_trans2;

		convexConvex.getClosestPoints(input, gjkOutput, 0);					

		// �Ÿ��� 0���϶��, �� �浹�Ͽ��ٸ�
		if(gjkOutput.m_distance < 0.f)
		{
			hitFlag = true;

			// ���������
			collisionMeleeData data;

			irr::core::vector3df hitPoint, hitNormal;

			Bullet2Irrlicht(gjkOutput.m_pointInWorld, hitPoint);
			Bullet2Irrlicht(gjkOutput.m_normalOnBInWorld, hitNormal);

			// �浹����
			data.hitPoint = hitPoint;
			
			// ��������
			data.hitNormal = hitNormal;
			
			// �İ�� �Ÿ�
			data.hitDistance = gjkOutput.m_distance;
			
			// �浹�� ����
			data.hitNodeName = (*it).first;

			// �浹������ ����
			vData.push_back(data);
		}
	}

	// �浹�� �������̸� ���� ����� �浹�������� ������.
	if ( vData.size() > 1 )
		sort(vData.begin(), vData.end(), compareCollisionMeleeData);

	// �浹�� �־���� ������ �������� [0]��°, �� ���� ����� ������� ����.
	if ( hitFlag )
		m_resultMeleeData = vData[0];
}