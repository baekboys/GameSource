#pragma warning (disable:4819)

#include "CBulletAnimatorManager.h"
#include "BulletChracterAnimator.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

#include "CBulletPhysicsUtils.h"

namespace irr
{
namespace scene
{

CBulletChracterAnimator::CBulletChracterAnimator(void) :CBulletObjectAnimator()
  
{
	m_LocalPos = irr::core::vector3df(0,0,0);
	
	
//	m_bForword = false;
//	m_bBakword = false;
//	m_bLeftword = false;
//	m_bRightword = false;
//	m_bStop = false;
	//	m_uLastTick = 0;
//	m_pDynamicsWorld = NULL;
//	m_pCam = NULL;
	
}

CBulletChracterAnimator::~CBulletChracterAnimator(void)
{
	RemoveFromWorld();
	SAFE_DELETE(bulletMesh);
	SAFE_DELETE(collisionShape);
	SAFE_DELETE(motionState);
	SAFE_DELETE(rigidBody);
}

//------------------------------------------------------------------------------
//! CreateInstance
//! Creates CBulletChracterAnimator or returns NULL
//! CBulletObjectAnimator�� �޸� ��ȸ���� ����(���ִ»��¸� �����ϱ����ؼ��� �������� �����ϱ����ؼ�...)
CBulletChracterAnimator* CBulletChracterAnimator::createInstance(
  ISceneManager* pSceneManager,
  ISceneNode* pSceneNode,
  CBulletAnimatorManager* pBulletMgr,
  CBulletObjectAnimatorGeometry* pGeom,
  CBulletObjectAnimatorParams* pPhysicsParam) 
{
  // get node scaling
  core::vector3df scaling = pSceneNode->getScale();

  btStridingMeshInterface* triangleMesh = NULL;
  // prepare collision shape
  btCollisionShape* collisionShape = 
    CreateBulletCollisionShape(pSceneManager, pGeom, scaling, triangleMesh);
  if (collisionShape == NULL)
    return NULL;

  CBulletChracterAnimator* bulletAnimator = new CBulletChracterAnimator();
  bulletAnimator->geometry = *pGeom;
  bulletAnimator->physicsParams = *pPhysicsParam;
  bulletAnimator->bulletMesh = triangleMesh;
  bulletAnimator->collisionShape = collisionShape;

  bulletAnimator->sceneNode = pSceneNode;
  bulletAnimator->sceneManager = pSceneManager;
  bulletAnimator->bulletMgr = pBulletMgr;

  bulletAnimator->InitPhysics();

  ////////////////////////////////////////////////////
  //	���� �������� �����Ӽ� ��� ���� �Ķ����	//
  //	2010. 2. 2									//
  ////////////////////////////////////////////////////
  
  // ���� � ����
  btVector3 linearFactor;
  Irrlicht2Bullet(pPhysicsParam->linearFactor, linearFactor);
  bulletAnimator->getRigidBody()->setLinearFactor(linearFactor);
  
  // �� � ����
  btVector3 angularFactor;
  Irrlicht2Bullet(pPhysicsParam->angularFactor, angularFactor);
  bulletAnimator->getRigidBody()->setAngularFactor(angularFactor);


  //�߰� ���� �Ӽ�
  //������ ��� ���� ��ȸ�� ����
  /*bulletAnimator->getRigidBody()->setAngularFactor(0.0f);
  bulletAnimator->getRigidBody()->setSleepingThresholds (0.0, 0.0);*/  

  return bulletAnimator;
}

//------------------------------------------------------------------------------
//! animateNode
//! animates a scene node
void CBulletChracterAnimator::animateNode(ISceneNode* node, u32 timeMs)
{

	_IRR_DEBUG_BREAK_IF(node != sceneNode);	


	if (physicsParams.mass != 0.0f && rigidBody && rigidBody->getMotionState())
	{
		// set pos
		btVector3 p = rigidBody->getCenterOfMassPosition();

		sceneNode->setPosition(core::vector3df(p.getX(), p.getY(), p.getZ()) + m_LocalPos);

		// set rot
		core::vector3df rot;
		btQuaternion btq = rigidBody->getOrientation();
		QuaternionToEulerXYZ(btq, rot);
		rot *= (f32)BPU_180_PI;
		//rot.Y = 0-rot.Y; //�޼���ǥ��� ��ȯ(may bullet use right hande axies?)
		sceneNode->setRotation(rot);
	}
}

btScalar CBulletChracterAnimator::getBodyFrontAngle()
{
	btTransform xform;
	rigidBody->getMotionState()->getWorldTransform(xform);
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize();

	irr::core::vector3df dir(forwardDir.getX(),0,forwardDir.getZ());

	return 0 - dir.getHorizontalAngle().Y;

	/*
	btScalar Angle = forwardDir.angle(btVector3(0,0,1));
	if(forwardDir.getX() < 0)
	{
		Angle += irr::core::PI;
	}
	return Angle;
	*/
}


//���� �ൿ �����
void CBulletChracterAnimator::controlStep_Walker(btVector3 &WalkVelocity)
{
	controlStep(WalkVelocity,true,true);		
}

//!��ó�� ����(�浹 ó��)
void CBulletChracterAnimator::preStep()
{
	btTransform xform;	
	rigidBody->getMotionState()->getWorldTransform (xform);
	

	btVector3 down = -xform.getBasis()[1]; //Y�� ����
	btVector3 forward = xform.getBasis()[2]; //Z�� ����
	down.normalize ();

	forward.setX(-forward.getX()); //���� �����ʿ���.

	forward.normalize();

	m_raySource[0] = xform.getOrigin();
	m_raySource[1] = xform.getOrigin();

	m_rayTarget[0] = m_raySource[0] + down * (geometry.geometryUnion.Capsule.hight*.5f + geometry.geometryUnion.Capsule.radius) * btScalar(1.1); // / 2.f) * btScalar(1.1);
	m_rayTarget[1] = m_raySource[1] + forward * (geometry.geometryUnion.Capsule.radius ) * btScalar(1.1);

	/*ggf::irr_util::DebugOutputFmt(NULL,"%f %f %f | %f %f %f\n"
	,m_raySource[1].getX()
	,m_raySource[1].getY()
	,m_raySource[1].getZ()
	,m_rayTarget[1].getX()
	,m_rayTarget[1].getY()
	,m_rayTarget[1].getZ()			
	);*/

	class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
	{
	public:
		ClosestNotMe (btRigidBody* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
		{
			m_me = me;
		}

		virtual btScalar AddSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
		{
			if (rayResult.m_collisionObject == m_me)
				return 1.0;
			return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace
				);
		}
	protected:
		btRigidBody* m_me;
	};

	ClosestNotMe rayCallback(rigidBody);

	//btDynamicsWorld* dynamicsWorld = ->getWorld();
	//if(m_pDynamicsWorld)

	{
		btDynamicsWorld* dynamicsWorld = bulletMgr->getBulletWorldByID(bulletWorldID)->getWorld();

		int i = 0;
		for (i = 0; i < 2; i++)
		{			
			rayCallback.m_closestHitFraction = 1.0;
			dynamicsWorld->rayTest (m_raySource[i], m_rayTarget[i], rayCallback);
			if (rayCallback.hasHit())
			{
				m_rayLambda[i] = rayCallback.m_closestHitFraction; //�浹������
			} else {
				m_rayLambda[i] = 1.0; //�浹��������
			}
		}
	}		
}

//! Angle�� ����
void CBulletChracterAnimator::controlStep_Walker(btScalar Angle, btScalar Speed)
{
	/*
	btVector3 WalkVelocity(0,0,1);
	WalkVelocity.rotate(btVector3(0,1,0),Angle);
	WalkVelocity *= Speed;
	controlStep(WalkVelocity,true,true);
	*/

	////��ó�� ����(�浹 ó��)
	//{
	//	btTransform xform;	
	//	rigidBody->getMotionState()->getWorldTransform (xform);
	//
	//	btVector3 down = -xform.getBasis()[1]; //Y�� ����
	//	btVector3 forward = xform.getBasis()[2]; //Z�� ����
	//	down.normalize ();

	//	//forward.setX(-forward.getX()); //���� �����ʿ���.

	//	forward.normalize();
	//	forward.setX(-forward.getX());

	//	m_raySource[0] = xform.getOrigin();
	//	m_raySource[1] = xform.getOrigin();

	//	m_rayTarget[0] = m_raySource[0] + down * (geometry.geometryUnion.Capsule.hight*.5f + geometry.geometryUnion.Capsule.radius )* btScalar(1.1);///2.f) * btScalar(1.5f);
	//	m_rayTarget[1] = m_raySource[1] + forward * (geometry.geometryUnion.Capsule.radius ) * btScalar(1.1);		

	//	class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
	//	{
	//	public:
	//		ClosestNotMe (btRigidBody* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	//		{
	//			m_me = me;
	//		}

	//		virtual btScalar AddSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	//		{
	//			if (rayResult.m_collisionObject == m_me)
	//				return 1.0;
	//			
	//			return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace
	//				);
	//		}
	//	protected:
	//		btRigidBody* m_me;
	//	};

	//	ClosestNotMe rayCallback(rigidBody);		

	//	{
	//		btDynamicsWorld* dynamicsWorld = bulletMgr->getBulletWorldByID(bulletWorldID)->getWorld();

	//		int i = 0;
	//		for (i = 0; i < 2; i++)
	//		{			
	//			rayCallback.m_closestHitFraction = 1.0;
	//			dynamicsWorld->rayTest (m_raySource[i], m_rayTarget[i], rayCallback);
	//			if (rayCallback.hasHit())
	//			{
	//				m_rayLambda[i] = rayCallback.m_closestHitFraction; //�浹������
	//			} else {
	//				m_rayLambda[i] = 1.0; //�浹��������
	//			}
	//		}
	//	}		
	//}	

	


	//��ó��
	
	
	preStep();

	{
		btTransform xform;	
		rigidBody->getMotionState()->getWorldTransform (xform);
		
		// ȸ�� ���
		xform.setRotation (btQuaternion (btVector3(0.0, 1.0, 0.0), Angle * irr::core::DEGTORAD));			
		
		btScalar velocity_Y = rigidBody->getLinearVelocity().getY();

		if(m_rayLambda[0] < 1.0) //���鿡 ������ ����
		{
			if(velocity_Y < 0) //���� �����������ִ°�?
			{
				velocity_Y = 0; //���� �̹� ���������Ƿ� �� ������������.
			}			
		}

		btVector3 forwardDir = xform.getBasis()[2];
		forwardDir.normalize ();		
		forwardDir.setX(-forwardDir.getX());		
		
		btVector3 velocity =  (forwardDir * Speed);
		velocity.setY(velocity_Y);
		rigidBody->setLinearVelocity (velocity);

		// ȸ�� ����
		rigidBody->getMotionState()->setWorldTransform (xform);
		rigidBody->setCenterOfMassTransform (xform);		
	}

}

//���ߺξ繰ü �ൿ�����
void CBulletChracterAnimator::controlStep(btVector3 &WalkVelocity,bool bVelforce,bool bSyncMoveDirection)
{

	preStep();	

	btTransform xform;	
	rigidBody->getMotionState()->getWorldTransform (xform);	

	//��ó��
	{
			
		//��������
		btVector3 linearVelocity = rigidBody->getLinearVelocity();			
		if(!bVelforce)
		{			
			linearVelocity += WalkVelocity;

			if(m_rayLambda[0] < 1.0) //���鿡 ������ ����
			{
				linearVelocity.setY(0);
			}

			rigidBody->setLinearVelocity (linearVelocity);		
		}
		else//����������
		{
			if(m_rayLambda[0] < 1.0) //�̲�������
			{
				if(linearVelocity.getY() < 0)
					WalkVelocity.setY(0);
				else
					WalkVelocity.setY(linearVelocity.getY()); //���ϼӵ�������

			}
			else
			{
				//if(m_rayLambda[1] < 1.0) //��Ǫ�� ������⿡ �Ӱ��ɸ���
				//{
				//	WalkVelocity = linearVelocity;
				//}
				//else
				//{
				WalkVelocity.setY(linearVelocity.getY()); //���ϼӵ�������
				//}
				
			}
			linearVelocity = WalkVelocity;
		}
		rigidBody->setLinearVelocity (linearVelocity);		

		

		{
			irr::core::vector3df dir = irr::core::vector3df(
				linearVelocity.getX(),
				0,
				linearVelocity.getZ());

			//�̵� ������ �ٶ󺸱�
			if(dir.getLength() > 0.f && bSyncMoveDirection)
			{
				//dir.Y = 10;
				
				dir.normalize();
				btScalar tAngle = dir.getHorizontalAngle().Y;			
				
				xform.setRotation(btQuaternion(btVector3(0,1,0),tAngle *  irr::core::DEGTORAD) );			

				
				//forward.rotate(btVector3(0,1,0),tAngle * irr::core::DEGTORAD);
			}
		}

		//ggf::irr_util::DebugOutputFmt(NULL,"%f %f %f %f\n",m_rayLambda[0],m_rayLambda[1],getBodyFrontAngle(),linearVelocity.getY());
		
		/*if(WalkVelocity.length() > 0.0f)
		{
			irr::core::vector3df dir = irr::core::vector3df(WalkVelocity.getX(),
				WalkVelocity.getY(),
				WalkVelocity.getZ());
			btScalar tAngle = dir.getHorizontalAngle().Y;

				

			ggf::irr_util::DebugOutputFmt(NULL,"%f %f %f %f\n",tAngle,linearVelocity.length(),WalkVelocity.length(),m_rayLambda[1]);
		}*/
	}

	
	rigidBody->getMotionState()->setWorldTransform (xform);
	rigidBody->setCenterOfMassTransform (xform);

	/*
	irr::core::vector3df LookAtdir = m_pCam->getTarget() - m_pCam->getPosition();
	LookAtdir.Y=0;
	LookAtdir.normalize();

	//�������
	irr::core::vector3df SideDir = LookAtdir.crossProduct(m_pCam->getUpVector());
	SideDir.Y=0;
	SideDir.normalize();	
	btTransform xform;
	rigidBody->getMotionState()->getWorldTransform (xform);		

	btVector3 linearVelocity = rigidBody->getLinearVelocity();
	btScalar speed = rigidBody->getLinearVelocity().length();

	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize ();

	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);		

	btScalar walkSpeed = 20.0f * fElapsedTime;

	if (m_bForword)
	{			
		walkDirection += btVector3(LookAtdir.X,LookAtdir.Y,LookAtdir.Z);			
		//walkDirection += forwardDir;

	}		
	if (m_bBakword)
	{
		walkDirection -= btVector3(LookAtdir.X,LookAtdir.Y,LookAtdir.Z);			
		//walkDirection -= forwardDir;
	}

	if (m_bLeftword)
	{
		walkDirection += btVector3(SideDir.X,SideDir.Y,SideDir.Z);						
	}
	if (m_bRightword)
	{
		walkDirection -= btVector3(SideDir.X,SideDir.Y,SideDir.Z);						
	}

	//if (!forward && !backward && onGround())
	if((!m_bForword && !m_bBakword && !m_bRightword && !m_bLeftword &&	m_rayLambda[0] < 1.0f) || //�ܷ��� ������....
		m_bStop //������ ����
		)
	{			
		linearVelocity *= btScalar(0.2);
		rigidBody->setLinearVelocity (linearVelocity);
	} else {
		if (speed < 20.0f)
		{
			//ggf::irr_util::DebugOutputFmt(NULL,"%f \n",walkDirection.angle(btVector3(1.0, 0.0, 0.0)));
			btVector3 velocity = linearVelocity + walkDirection * walkSpeed;
			rigidBody->setLinearVelocity (velocity);						
		}
	}

	
	if(walkDirection.length() > 0.0f)
	{
		irr::core::vector3df dir = irr::core::vector3df(walkDirection.getX(),
			walkDirection.getY(),
			walkDirection.getZ());
		btScalar tAngle = dir.getHorizontalAngle().Y;
		//btScalar tAngle = walkDirection.angle(btVector3(0.0, 1.0, 0.0));
		ggf::irr_util::DebugOutputFmt(NULL,"%f \n",tAngle);
		//xform.setRotation (btQuaternion (btVector3(0.0, 1.0, 0.0), 
		//	tAngle));

	}

	rigidBody->getMotionState()->setWorldTransform (xform);
	rigidBody->setCenterOfMassTransform (xform);

	{
		//wchar_t wszOutput[256];
		//StringCchggf::irr_util::DebugOutputFmt(NULL, wszOutput, 1024, L"%f\n",rigidBody->getLinearVelocity().length());
		//OutputDebugString(wszOutput);
		
	}
	*/
}


}
}
