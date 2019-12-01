//저작권:
//물리 fps 애니메이터 
//수정 2008.8.12
//미구현 사항 : 점푸구현
//
//수정 2009.8.6

#pragma warning (disable:4819)
#include "CBulletAnimatorManager.h"

#include "BulletFpsCamAnimator.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "Keycodes.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"


#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

namespace irr
{
namespace scene
{

//! constructor
CBulletFpsCamAnimator::CBulletFpsCamAnimator(gui::ICursorControl* cursorControl,
		f32 rotateSpeed, f32 moveSpeed, f32 jumpSpeed,
		SKeyMap* keyMapArray, u32 keyMapSize, bool noVerticalMovement)
: CursorControl(cursorControl), MaxVerticalAngle(88.0f),
	MoveSpeed(moveSpeed/1000.0f), RotateSpeed(rotateSpeed), JumpSpeed(jumpSpeed),
	LastAnimationTime(0), firstUpdate(true), NoVerticalMovement(noVerticalMovement),
	FPSCursorControl(true)
{
	#ifdef _DEBUG
	setDebugName("CCameraSceneNodeAnimatorFPS");
	#endif

	if (CursorControl)
		CursorControl->grab();

	allKeysUp();

	// create key map
	if (!keyMapArray || !keyMapSize)
	{
		// create default key map
		KeyMap.push_back(SCamKeyMap(0, irr::KEY_KEY_W));
		KeyMap.push_back(SCamKeyMap(1, irr::KEY_KEY_S));
		KeyMap.push_back(SCamKeyMap(2, irr::KEY_KEY_A));
		KeyMap.push_back(SCamKeyMap(3, irr::KEY_KEY_D));
		KeyMap.push_back(SCamKeyMap(4, irr::KEY_SPACE ));

		/*KeyMap.push_back(SCamKeyMap(0, irr::KEY_UP));
		KeyMap.push_back(SCamKeyMap(1, irr::KEY_DOWN));
		KeyMap.push_back(SCamKeyMap(2, irr::KEY_LEFT));
		KeyMap.push_back(SCamKeyMap(3, irr::KEY_RIGHT));
		KeyMap.push_back(SCamKeyMap(4, irr::KEY_KEY_J));*/
	}
	else
	{
		// create custom key map
		setKeyMap(keyMapArray, keyMapSize);
	}

	m_LocalPos = irr::core::vector3df(0,0,0);
}


//! destructor
CBulletFpsCamAnimator::~CBulletFpsCamAnimator()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
//! 아래의이벤트핸들러가 호출되는 이유는 현재활성화된 카메라노드에 자식으로 붙어있는 애니매이터는
//! 무조건 이밴트핸들러가 호출이된다.
bool CBulletFpsCamAnimator::OnEvent(const SEvent& evt)
{
	switch(evt.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		for (u32 i=0; i<KeyMap.size(); ++i)
		{
			if (KeyMap[i].keycode == evt.KeyInput.Key)
			{
				CursorKeys[KeyMap[i].action] = evt.KeyInput.PressedDown;
				return true;
			}
		}
		break;

	case EET_MOUSE_INPUT_EVENT:
		if (evt.MouseInput.Event == EMIE_MOUSE_MOVED)
		{
			CursorPos = CursorControl->getRelativePosition();
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}


//------------------------------------------------------------------------------
//! CreateInstance
//! Creates CBulletChracterAnimator or returns NULL
//! CBulletObjectAnimator와 달리 각회전을 제한(서있는상태를 유지하기위해서는 쓰러짐을 제어하기위해서...)
CBulletFpsCamAnimator* CBulletFpsCamAnimator::createInstance(
  ISceneManager* pSceneManager,
  ISceneNode* pSceneNode,
  gui::ICursorControl *CursorControl,
  CBulletAnimatorManager* pBulletMgr,
  CBulletObjectAnimatorGeometry* pGeom,
  CBulletObjectAnimatorParams* pPhysicsParam) 
{
	//CursorControl = CursorControl;
  // get node scaling
  core::vector3df scaling = pSceneNode->getScale();

  btStridingMeshInterface* triangleMesh = NULL;
  // prepare collision shape
  btCollisionShape* collisionShape = 
    CreateBulletCollisionShape(pSceneManager, pGeom, scaling, triangleMesh);
  if (collisionShape == NULL)
    return NULL;

  CBulletFpsCamAnimator* bulletAnimator = new CBulletFpsCamAnimator(CursorControl,100.f,50.f);
  bulletAnimator->geometry = *pGeom;
  bulletAnimator->physicsParams = *pPhysicsParam;
  bulletAnimator->bulletMesh = triangleMesh;
  bulletAnimator->collisionShape = collisionShape;

  bulletAnimator->sceneNode = pSceneNode;
  bulletAnimator->sceneManager = pSceneManager;
  bulletAnimator->bulletMgr = pBulletMgr;
  bulletAnimator->CursorControl = CursorControl;

  bulletAnimator->InitPhysics();

  //추가 물리 속성
  //쓰러짐 제어를 위해 각회전 제한
  bulletAnimator->getRigidBody()->setAngularFactor(0.0f);
  bulletAnimator->getRigidBody()->setSleepingThresholds (0.0, 0.0);  

  return bulletAnimator;
}


void CBulletFpsCamAnimator::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node->getType() != ESNT_CAMERA)
		return;

	ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

	if (firstUpdate)
	{
		if (CursorControl && camera)
		{
			CursorControl->setPosition(0.5f, 0.5f);
			CursorPos = CenterCursor = CursorControl->getRelativePosition();
		}

		LastAnimationTime = timeMs;

		firstUpdate = false;
	}

	// get time
	f32 timeDiff = (f32) ( timeMs - LastAnimationTime );
	LastAnimationTime = timeMs;

	// update position
	core::vector3df pos = camera->getPosition();

	// Update rotation
	core::vector3df target = (camera->getTarget() - camera->getAbsolutePosition());
	core::vector3df relativeRotation = target.getHorizontalAngle();

	if (CursorControl && FPSCursorControl)
	{
		if (CursorPos != CenterCursor)
		{
			relativeRotation.Y -= (0.5f - CursorPos.X) * RotateSpeed;
			relativeRotation.X -= (0.5f - CursorPos.Y) * RotateSpeed;

			if (relativeRotation.X > MaxVerticalAngle*2 &&
				relativeRotation.X < 360.0f-MaxVerticalAngle)
			{
				relativeRotation.X = 360.0f-MaxVerticalAngle;
			}
			else
			if (relativeRotation.X > MaxVerticalAngle &&
				relativeRotation.X < 360.0f-MaxVerticalAngle)
			{
				relativeRotation.X = MaxVerticalAngle;
			}

			// reset cursor position
			CursorControl->setPosition(0.5f, 0.5f);
			CenterCursor = CursorControl->getRelativePosition();
		}
	}

	// set target

	target.set(0,0,100);
	core::vector3df movedir = target;

	core::matrix4 mat;
	mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
	mat.transformVect(target);

	if (NoVerticalMovement)
	{
		mat.setRotationDegrees(core::vector3df(0, relativeRotation.Y, 0));
		mat.transformVect(movedir);
	}
	else
	{
		movedir = target;
	}

	movedir.normalize();

	/////////////////////////gbox/////////////////////////
	//gbox patch 08.07.27 rotation bug fix
	camera->setRotation(mat.getRotationDegrees());
	/////////////////////////gbox/////////////////////////

	TargetVector = target;
	
	irr::f32 Speed = 0;// = timeDiff * MoveSpeed;
	irr::f32 Strife_Speed = 0;
	irr::f32 Angle = relativeRotation.Y;
	
	if (CursorKeys[0])
	{
		Speed = MoveSpeed;
	}
	if (CursorKeys[1])
	{
		Speed = -MoveSpeed;
	}
	if (CursorKeys[2])
	{
		Strife_Speed = MoveSpeed;
	}
	if (CursorKeys[3])
	{
		Strife_Speed = -MoveSpeed;
	}

	//전처리 스탭(충돌 처리)
	{
		btTransform xform;	
		rigidBody->getMotionState()->getWorldTransform (xform);
	
		btVector3 down = -xform.getBasis()[1]; //Y축 정보
		btVector3 forward = xform.getBasis()[2]; //Z축 정보
		down.normalize ();	

		forward.normalize();
		forward.setX(-forward.getX()); //오른손좌표계롤 왼손좌표계로...

		m_raySource[0] = xform.getOrigin();
		m_raySource[1] = xform.getOrigin();

		m_rayTarget[0] = m_raySource[0] + down * ((geometry.Capsule.hight * 0.5f) + geometry.Capsule.radius ) * btScalar(1.1);
		m_rayTarget[1] = m_raySource[1] + forward * (geometry.Capsule.radius ) * btScalar(1.1);		

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

		{
			btDynamicsWorld* dynamicsWorld = bulletMgr->getBulletWorldByID(bulletWorldID)->getWorld();

			int i = 0;
			for (i = 0; i < 2; i++)
			{			
				rayCallback.m_closestHitFraction = 1.0;
				dynamicsWorld->rayTest (m_raySource[i], m_rayTarget[i], rayCallback);
				if (rayCallback.hasHit())
				{
					m_rayLambda[i] = rayCallback.m_closestHitFraction; //충돌비율값
				} else {
					m_rayLambda[i] = 1.0; //충돌하지않음
				}
			}
		}		
	}	


	//후처리
	{
		btTransform xform;	
		rigidBody->getMotionState()->getWorldTransform (xform);

		xform.setRotation (btQuaternion (btVector3(0.0, 1.0, 0.0), Angle * irr::core::DEGTORAD));			

		btVector3 linearVelocity = rigidBody->getLinearVelocity();		

		if(m_rayLambda[0] < 1.0)
		{
			linearVelocity.setY(0);			
		}

		btVector3 forwardDir = xform.getBasis()[2];
		btVector3 SideDir = xform.getBasis()[0];

		//축변환 및 정규화
		forwardDir.normalize ();		
		forwardDir.setX(-forwardDir.getX());
		SideDir.normalize();
		SideDir.setX(-SideDir.getX());

		btVector3 velocity =  (forwardDir * Speed);

		velocity +=  (SideDir * Strife_Speed);

		irr::core::vector3df fowardvec = irr::core::vector3df(forwardDir.getX(),forwardDir.getY(),forwardDir.getZ());
		irr::core::vector3df sidevec = irr::core::vector3df(-SideDir.getX(),SideDir.getY(),SideDir.getZ());

		velocity.setY(linearVelocity.getY());

		rigidBody->setLinearVelocity (velocity);

		rigidBody->getMotionState()->setWorldTransform (xform);
		rigidBody->setCenterOfMassTransform (xform);
	}


	//물리엔진 변환적용
	if (physicsParams.mass != 0.0f && rigidBody && rigidBody->getMotionState())
	{

		btTransform xform;	
		rigidBody->getMotionState()->getWorldTransform (xform);			

		btVector3 forwardDir = xform.getBasis()[2];
		forwardDir.normalize ();

		// set pos
		btVector3 p = rigidBody->getCenterOfMassPosition();
		
		irr::core::vector3df vforward = irr::core::vector3df(-forwardDir.getX(),forwardDir.getY(),forwardDir.getZ());
		irr::core::vector3df eye_pos = core::vector3df(p.getX(), p.getY(), p.getZ()) + m_LocalPos;

		sceneNode->setPosition(eye_pos);
		camera->setTarget(eye_pos + TargetVector);	
	}
}

void CBulletFpsCamAnimator::addRotation(irr::f32 vertical, irr::f32 horizontal)
{
	ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(sceneNode);
	irr::core::vector3df rotation =  camera->getAbsoluteTransformation().getRotationDegrees();

	rotation.X -= vertical;
	rotation.Y -= horizontal;

	irr::core::vector3df vDir(0,0,1);
	irr::core::matrix4 mat;
	mat.setRotationDegrees(rotation);
	mat.transformVect(vDir);

	//camera->setRotation(rotation);


	btVector3 p = rigidBody->getCenterOfMassPosition();
	irr::core::vector3df eye_pos = core::vector3df(p.getX(), p.getY(), p.getZ()) + m_LocalPos;

	camera->setTarget(eye_pos + vDir);
}


void CBulletFpsCamAnimator::allKeysUp()
{
	for (u32 i=0; i<6; ++i)
		CursorKeys[i] = false;
}


//! Sets the rotation speed
void CBulletFpsCamAnimator::setRotateSpeed(f32 speed)
{
	RotateSpeed = speed;
}


//! Sets the movement speed
void CBulletFpsCamAnimator::setMoveSpeed(f32 speed)
{
	MoveSpeed = speed;
}


//! Gets the rotation speed
f32 CBulletFpsCamAnimator::getRotateSpeed() const
{
	return RotateSpeed;
}


// Gets the movement speed
f32 CBulletFpsCamAnimator::getMoveSpeed() const
{
	return MoveSpeed;
}


//! Sets the keyboard mapping for this animator
void CBulletFpsCamAnimator::setKeyMap(SKeyMap *map, u32 count)
{
	// clear the keymap
	KeyMap.clear();

	// add actions
	for (u32 i=0; i<count; ++i)
	{
		switch(map[i].Action)
		{
		case EKA_MOVE_FORWARD: KeyMap.push_back(SCamKeyMap(0, map[i].KeyCode));
			break;
		case EKA_MOVE_BACKWARD: KeyMap.push_back(SCamKeyMap(1, map[i].KeyCode));
			break;
		case EKA_STRAFE_LEFT: KeyMap.push_back(SCamKeyMap(2, map[i].KeyCode));
			break;
		case EKA_STRAFE_RIGHT: KeyMap.push_back(SCamKeyMap(3, map[i].KeyCode));
			break;
		case EKA_JUMP_UP: KeyMap.push_back(SCamKeyMap(4, map[i].KeyCode));
			break;
		default:
			break;
		}
	}
}

//! Sets whether vertical movement should be allowed.
void CBulletFpsCamAnimator::setVerticalMovement(bool allow)
{
	NoVerticalMovement = !allow;
}


} // namespace scene
} // namespace irr

