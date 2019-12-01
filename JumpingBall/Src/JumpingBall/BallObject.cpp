#include "BallObject.h"
#include "irrFactory.h"
#include "App.h"
#include "bulletEngine.h"
#include "AppDataStructure.h"
#include "IrrXMLReader.h"

// �߷¼����� �ΰ��������� ������ �ּҰ�
const irr::f32 accel_minimum_value = 0.02f;

// �߷¼����� ���� ��
const irr::f32 accel_jump_value = 1.2f;

// ���� �����Ŀ� �ٽ� ������ �����ϱ� ���� Ÿ�̸�
const irr::f32 jumpMinimumTimer = 0.3f;

// ������ ���ؼ� ���� �ٴڿ� ��� �־�� �Ѵ�. ������������ üũ�ϱ� ���� �����˻縦 �ϴµ�
// �����˻簡 ���� ������ ���� ���� �� ���� ���� ���� ������ ���Ƶ� ������ �Ҽ� �ִ�.
const irr::f32 jumpRayTestValue = 1.3f;

CBallObject::CBallObject(void)
	: m_fBallSize (10.f), m_fJumpForce(30), m_fMaxLinearVelocity(100), m_fMaxAngularVelocity(20),
	m_bHasItemEffect(false), m_fItemEffectDurationTime(0), m_fItemEffectTimer(0), m_fJumpTimer(jumpMinimumTimer),
	m_pMainNode(NULL), m_pSphereNode(NULL), m_pItemEffectNode(NULL), m_pAnimator(NULL)
{
	SetStatus(BALL_OBJ_READY);
}


CBallObject::~CBallObject(void)
{

}


irr::core::vector3df CBallObject::getPosition()
{
	return m_pMainNode->getAbsolutePosition();
}

bool CBallObject::jump() 
{
	// ���� ���� ���� ������ �ǽ��Ѵ�.
	//if(GetStatus() == BALL_OBJ_COLLISION)
	if(CheckPossibleJump() && GetStatus() != BALL_OBJ_ZERO_GRAVITY)
	{
		// ���� ���� ���
		{
			CApp::Get().NativePlaySound(m_sBallParam[m_eBallType].jumpSoundFileName.c_str());
			CApp::Get().NativePlayVibration();
		}

		// �׵����� ���� ���ش�.
		m_pAnimator->getRigidBody()->clearForces();

		// ������ ���Ͽ� �߷��� �ݴ� �������� ���� �ش�.
		m_pAnimator->applyImpulse(irr::core::vector3df(0,0,1) * m_fJumpForce, irr::core::vector3df(0,5,0));


		// �������·� ��ȯ
		//SetStatus(BALL_OBJ_JUMP);

		// ���� Ÿ�̸� ����
		m_fJumpTimer = 0;

		return true;
	}

	return false;
}

bool CBallObject::CheckPossibleJump()
{
	// ���� Ÿ�̸Ӱ� �����Ƿ� ���� �Ұ���
	if(m_fJumpTimer < jumpMinimumTimer)
	{
		return false;
	}

	btTransform xform;	
	m_pRigidBody->getMotionState()->getWorldTransform (xform);
	btVector3 raySource = xform.getOrigin();
	btVector3 rayTarget[3];
	
	// �غκ� �浹����
	btVector3 down(0,0,-1);
	// ���ʹغκ�(���� 45��)
	btVector3 down_left(-1,0,-1);
	// �����ʹغκ�(������ 45��)
	btVector3 down_right(1,0,-1);

	// 3�������� ���� �˻��ϱ�
	rayTarget[0]= xform.getOrigin() + down.normalize() * m_fBallSize * jumpRayTestValue;
	rayTarget[1]= xform.getOrigin() + down_left.normalize() * m_fBallSize * jumpRayTestValue;
	rayTarget[2]= xform.getOrigin() + down_right.normalize() * m_fBallSize * jumpRayTestValue;

	// ���� �ݹ�
	btCollisionWorld::ClosestRayResultCallback rayCallback_down(raySource, rayTarget[0]);
	btCollisionWorld::ClosestRayResultCallback rayCallback_down_left(raySource, rayTarget[1]);
	btCollisionWorld::ClosestRayResultCallback rayCallback_down_right(raySource, rayTarget[2]);
	
	// ���� ���
	btDynamicsWorld* dynamicsWorld = m_pWorldAnimator->getWorld();

	// �ٴ� ���� �˻�
	dynamicsWorld->rayTest(raySource, rayTarget[0], rayCallback_down);
	if (rayCallback_down.hasHit())
	{
		return true;
	}
	// �밢�� �˻�
	else
	{
		// ���� �˻�
		dynamicsWorld->rayTest(raySource, rayTarget[1], rayCallback_down_left);
		if(rayCallback_down_left.hasHit())
		{
			return true;
		}
		else
		{
			dynamicsWorld->rayTest(raySource, rayTarget[2], rayCallback_down_right);
			if(rayCallback_down_right.hasHit()) 
			{
				return true;
			}
		}
	}

	return false;
}

bool CBallObject::isDie()
{
	if(GetStatus() == BALL_OBJ_DIE)
		return true;
	return false;
}


void CBallObject::Signal(irr::core::stringc strSignal,void *pParam)
{
		
	if(strSignal == "orientationEvent")
	{
		// ����ȯ
		irr::core::vector3df* senser = static_cast<irr::core::vector3df*> ( pParam );
				
		// ����ũ����� �ϴ� ���, �� �������� �Ϲ������� ���� ������ ����쿡�� ������.
		if(senser->getLength() > accel_jump_value)
		{
			if(jump())
				m_pGameApp->Signal("zoomOutJump", NULL);
		}
		else
		{
			setMoveVelocity(senser);
		}

		// ���ӵ� ���� ����
		m_AccelValue = *senser;
	}
	
	// ����
	else if(strSignal == "jump")
	{
		jump();
	}

	else if(strSignal == "force")
	{
		m_pAnimator->applyImpulse( ( *(static_cast<irr::core::vector3df*>(pParam))) );
	}

	// ��Ż �̵�
	else if(strSignal == "portal")
	{
		CApp::Get().NativePlaySound("res/sound/portal.mp3");
		m_pAnimator->setPosition( *(static_cast<irr::core::vector3df*>(pParam)) );
		m_pGameApp->Signal("portalEffect", NULL);
	}

	else if(strSignal == "clear")
	{
		SSignalData* data =  static_cast<SSignalData *>(pParam);

		// �� �Ⱥ��̱�
		m_pMainNode->setVisible(false);

		// �������� Ŭ���� �ñ׳��� GameApp���� ������
		m_pGameApp->Signal("clearStage", data);
	}

	else if(strSignal == "item")
	{
		CApp::Get().NativePlaySound("res/sound/itemeffect.mp3");
		
		// ������ȿ�� ����
		ResetItemEffect();

		// ������ȿ�� ����ǽ�
		m_bHasItemEffect = true;

		SSignalData* data = static_cast<SSignalData *>(pParam);

		// ������ Ÿ�� ����
		m_uItemType = data->type;

		switch(data->type)
		{
		case irr::scene::jz3d::CHANGE_TYPE :
			{
				irr::u32 type = static_cast<irr::u32>(data->fValue[0]);

				if(type >= BALL_TYPE_COUNT)
				{
					type = BALL_TYPE_COUNT - 1;
				}

				BALL_TYPE ball_type = static_cast<BALL_TYPE>(type);

				CreateBallObjFromSBallParam(ball_type, NULL);

				switch(type)
				{
				case BALL_TYPE_BASIC :
					{
						m_sItemEffect = L"[choco candy]";
					}
					break;
				case BALL_TYPE_beach :
					{
						m_sItemEffect = L"[bubble gum]";
					}
					break;

				case BALL_TYPE_IRON :
					{
						m_sItemEffect = L"[bowling ball]";
					}
					break;

				case BALL_TYPE_TENNIS :
					{
						m_sItemEffect = L"[tennis ball]";
					}
					break;
				case BALL_TYPE_WOOD :
					{
						m_sItemEffect = L"[wood ball]";
					}
					break;
				}
				m_fItemEffectDurationTime = data->fValue[1];
			}
			break;

		case irr::scene::jz3d::JUMP :
			{
				m_fJumpForce *= data->fValue[0];

				if(data->fValue[0] > 1.f)
					m_sItemEffect = L"Jumping Up+";
				else
					m_sItemEffect = L"Jumping Down-";

				m_fItemEffectDurationTime = data->fValue[1];
			}
			break;

		case irr::scene::jz3d::SIZE :
			{
				CreateBallObjFromSBallParam(m_eBallType, m_fBallSize * data->fValue[0]);

				if(data->fValue[0] > 1.f)
					m_sItemEffect = L"Size Up+";
				else
					m_sItemEffect = L"Size Down-";

				m_fItemEffectDurationTime = data->fValue[1];
			}
			break;

		/*case irr::scene::jz3d::DIRECTION :
			{
				m_pGameApp->Signal("directionObject on", NULL);

				m_sItemEffect = L"Direction On";

				m_fItemEffectDurationTime = data->fValue[1];
				if(m_fItemEffectDurationTime <= 0)
				{
					m_fItemEffectDurationTime = 10.f;
				}
			}
			break;*/

		case irr::scene::jz3d::TIME :
			{
				m_pGameApp->Signal("time", &data->fValue[0]);

				if(data->fValue[0] > 0.f)
				{
					m_sItemEffect = L"Time +";
					m_sItemEffect += static_cast<irr::u32>(data->fValue[0]);
				}
				else
				{
					m_sItemEffect = L"Time -";
					m_sItemEffect += static_cast<irr::u32>(data->fValue[0]);
				}

				m_fItemEffectDurationTime  = 5.f;
			}
			break;

		case irr::scene::jz3d::SPEED :
			{
				m_fMaxLinearVelocity *= data->fValue[0];

				if(data->fValue[0] > 1.f)
				{
					m_sItemEffect = L"Speed Up+";
				}
				else
				{
					m_sItemEffect = L"Speed Down-";
				}

				m_fItemEffectDurationTime = data->fValue[1];
			}
			break;

		case irr::scene::jz3d::ZERO_GRAVITY :
			{
				m_fAccel_Z_Value = m_AccelValue.Z;
				SetStatus(BALL_OBJ_ZERO_GRAVITY);
				m_sItemEffect = L"Zero gravity";
				m_pGameApp->Signal("gravity off", NULL);
				m_fItemEffectDurationTime = data->fValue[1];
			}
			break;
		}

		m_fItemEffectTimer = m_fItemEffectDurationTime;
		CreateParticle(static_cast<irr::u32>(m_fItemEffectDurationTime * 1000.f));
	}
	
	else if(strSignal == "preLoadSound")
	{
		preLoadSound();
	}
}

void CBallObject::setMoveVelocity(irr::core::vector3df* accelValue)
{
	if ( accelValue->getLength() < accel_minimum_value ) 
	{
		m_pRigidBody->clearForces();
	}
	else
	{
		btVector3 linearVelocity = m_pRigidBody->getLinearVelocity();

		switch (GetStatus())
		{
		/*case BALL_OBJ_COLLISION:
			{
				linearVelocity.setX(accelValue->Y * -m_fMaxLinearVelocity);
			}
			break;

		case BALL_OBJ_AIR:
			{
				linearVelocity.setX( (accelValue->Y * -m_fMaxLinearVelocity) / 1.5f);
			}
			break;*/

		case BALL_OBJ_ZERO_GRAVITY:
			{
				linearVelocity.setX(accelValue->Y * -m_fMaxLinearVelocity);
				linearVelocity.setZ( (accelValue->Z - m_fAccel_Z_Value) * -m_fMaxLinearVelocity);
			}
			break;

		default :
			{
				linearVelocity.setX(accelValue->Y * -m_fMaxLinearVelocity);
			}
			break;
		}

		m_pRigidBody->setLinearVelocity(linearVelocity);
	}
}

void CBallObject::Update(irr::f32 fTick)
{
	// ���������̸� ��Ƽ����·� ��ȯ
	if(m_pRigidBody->getActivationState() != 1)
	{
		m_pRigidBody->activate(true);
	}

	//switch(GetStatus())
	//{
	//case BALL_OBJ_READY:
	//	{
	//		SetStatus(BALL_OBJ_AIR);
	//	}
	//	break;

	//case BALL_OBJ_AIR:
	//	{
	//		if(CollisionCheck())
	//		{
	//			// �浹 ���� ó�� �� ����
	//			//CApp::Get().NativePlaySound(m_sBallParam[m_eBallType].boundSoundFileName.c_str());
	//			printf("vibration effect!!\n");

	//			SetStatus(BALL_OBJ_COLLISION);
	//		}
	//	}
	//	break;

	//case BALL_OBJ_COLLISION:
	//	{
	//		if(!CollisionCheck())
	//		{
	//			SetStatus(BALL_OBJ_AIR);
	//		}
	//	}
	//	break;

	//case BALL_OBJ_JUMP:
	//	{
	//		if(CollisionCheck())
	//			SetStatus(BALL_OBJ_COLLISION);
	//		else
	//			SetStatus(BALL_OBJ_AIR);
	//	}
	//	break;
	//}

	// ������ ȿ�� ������Ʈ
	if(m_bHasItemEffect)
	{
		if(m_fItemEffectTimer <= 0)
		{
			CApp::Get().NativePlaySound("res/sound/itemuneffect.mp3");
			ResetItemEffect();
		}
		else
		{
			m_fItemEffectTimer -= fTick;
			
			if(m_uItemType == irr::scene::jz3d::TIME)
			{
				// ���� ���ѽð� ȿ���� �ܿ��ð��� ǥ���� �ʿ� ����.
				m_pGameApp->DrawBallStatus(m_sItemEffect.c_str());
			}
			else
			{
				// �ܿ��ð��� �߰��Ͽ� ������.
				irr::core::stringw temp(m_sItemEffect);
				temp += " : ";
				temp += static_cast<irr::u32>(m_fItemEffectTimer);
				m_pGameApp->DrawBallStatus(temp.c_str());
			}
		}
	}
	else
	{
		// ������ ȿ���� �����Ƿ� ����ִ� ���ڿ�������.
		m_pGameApp->DrawBallStatus(L"");
	}

	// ���� Ÿ�̸� ����
	m_fJumpTimer += fTick;
	if(m_fJumpTimer > jumpMinimumTimer)
	{
		m_fJumpTimer = jumpMinimumTimer;
	}
}

btRigidBody* CBallObject::getRigidBody()
{
	return m_pRigidBody;
}


irr::scene::ISceneNode* CBallObject::getSceneNode()
{
	return m_pSphereNode;
}


irr::scene::CBulletChracterAnimator* CBallObject::getAnimator()
{
	return m_pAnimator;
}


bool CBallObject::Init(
					   irr::scene::ISceneManager* pSmgr,
					   irr::video::IVideoDriver* pVideo,
					   irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory,
					   irr::scene::CBulletWorldAnimator* pWorldAnimator,
					   CGameApp *gameApp
					   )
{
	m_pSmgr = pSmgr;
	m_pVideo = pVideo;
	m_pBulletPhysicsFactory = pBulletPhysicsFactory;
	m_pWorldAnimator = pWorldAnimator;
	m_pGameApp = gameApp;
				
	return true;
}

void CBallObject::reset()
{
	BALL_TYPE type;
	switch(m_uStartType)
	{
		case BALL_TYPE_BASIC:
			type = BALL_TYPE_BASIC;
			break;
			
		case BALL_TYPE_beach:
			type = BALL_TYPE_beach;
			break;
			
		case BALL_TYPE_WOOD:
			type = BALL_TYPE_WOOD;
			break;
			
		case BALL_TYPE_IRON:
			type = BALL_TYPE_IRON;
			break;
			
		case BALL_TYPE_TENNIS:
			type = BALL_TYPE_TENNIS;
			break;
	}

	// reset type
	CreateBallObjFromSBallParam(type, NULL);

	// �� ���̱�
	m_pMainNode->setVisible(true);

	// item effect off
	if(m_bHasItemEffect)
		ResetItemEffect();
	
	// reset position
	m_pAnimator->setPosition(m_startPos);
	
	// reset physics param
	m_pAnimator->setLinearVelocity(irr::core::vector3df(0,0,0));
	m_pAnimator->setAngularVelocity(irr::core::vector3df(0,0,0));

	// reset item effect string
	m_sItemEffect = L"";

	// jump timer
	m_fJumpTimer = jumpMinimumTimer;

	// gravity reset
	m_pGameApp->Signal("gravity on", NULL);
	
	// reset status
	SetStatus(BALL_OBJ_READY);
}

bool CBallObject::CollisionCheck()
{
	// ��ħ���� ���� ������
	irr::s32 num = m_pWorldAnimator->getWorld()->getDispatcher()->getNumManifolds();

	// �浹��ü�� �� �Ѱ� ���̹Ƿ� ��ħ���� ������ 1�� �̻��̸� ���� �ܺο� �浹�� ������ ����.
	if(num > 0)
		return true;

	// �浹���� �ʾ����Ƿ� false�� ����
	return false;
}


void CBallObject::ResetItemEffect()
{
	// ������ȿ�� ����
	m_bHasItemEffect = false;

	// ������ Ÿ�� ����
	m_uItemType = NULL;

	// ������ ȿ�� �� ����
	m_fItemValue = 0;

	// ������ ȿ�� ���ڿ� �� ����
	m_sItemEffect = L"";

	// ������ȿ�� ���ӽð�
	m_fItemEffectDurationTime = 0;

	// ������ȿ�� Ÿ�̸�
	m_fItemEffectTimer = 0;

	//// ���� �⺻���� ������� ���� ���� ����� �ٸ��ٸ� �����·� ȸ����Ű��
	//if(m_fBallSize != m_sBallParam[m_eBallType].fBallSize)
	//	CreateBallObjFromSBallParam(m_eBallType, NULL);

	BALL_TYPE type;
	switch(m_uStartType)
	{
	case BALL_TYPE_BASIC:
		type = BALL_TYPE_BASIC;
		break;

	case BALL_TYPE_beach:
		type = BALL_TYPE_beach;
		break;

	case BALL_TYPE_WOOD:
		type = BALL_TYPE_WOOD;
		break;

	case BALL_TYPE_IRON:
		type = BALL_TYPE_IRON;
		break;

	case BALL_TYPE_TENNIS:
		type = BALL_TYPE_TENNIS;
		break;
	}
	// reset type
	CreateBallObjFromSBallParam(type, NULL);

	// ���� ������
	m_fJumpForce = m_sBallParam[m_eBallType].fJumpForce;

	// ���� �̵��ӵ�
	m_fMaxLinearVelocity = m_sBallParam[m_eBallType].fMaxLinearVelocity;

	// �߷� ����
	m_pGameApp->Signal("gravity on", NULL);
	
	
	if(m_pItemEffectNode)
	{
		m_pItemEffectNode->remove();
		m_pItemEffectNode = NULL;
	}

	// ���� ����
	SetStatus(BALL_OBJ_READY);
}

void CBallObject::CreateParticle(irr::u32 time)
{
	irr::scene::IParticleSystemSceneNode* ps = 0;
	ps = m_pSmgr->addParticleSystemSceneNode();

	irr::scene::IParticleEmitter* em = ps->createSphereEmitter(
		irr::core::vector3df(0,0,0),
		m_fBallSize/1.5f,
		irr::core::vector3df(0,0,0),
		5,10,
		irr::video::SColor(255,0,0,0),
		irr::video::SColor(255,255,255,255),
		700, 1000, 0,
		irr::core::dimension2df(0.5f, 0.5f),
		irr::core::dimension2df(1.f, 1.f)
		);
	em->setMaxParticlesPerSecond(2);
	ps->setEmitter(em);
	em->drop();

	irr::scene::IParticleAffector* paf = ps->createFadeOutParticleAffector();
	ps->addAffector(paf);
	paf->drop();

	ps->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	ps->getMaterial(0).setTexture(0, m_pVideo->getTexture("texture/endarrow.png"));
	ps->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);

	//irr::scene::ISceneNodeAnimator* deleteAnimator = m_pSmgr->createDeleteAnimator(time);
	//ps->addAnimator(deleteAnimator);
	//deleteAnimator->drop();

	m_pMainNode->addChild(ps);

	m_pItemEffectNode = ps;
}


void CBallObject::CreateBallObjFromSBallParam(BALL_TYPE type, irr::f32 size)
{
	// Collision shape - ������ ���̰� �߻��Ҽ� �ֱ� ������ �״�� ������� �ʴ´�.
	irr::scene::CBulletObjectAnimatorGeometry geometry = m_sBallParam[type].geometry;

	// size���� NULL�̸� Ÿ�Կ� ����� �⺻������ ���
	if(size == 0)
		m_fBallSize = m_sBallParam[type].fBallSize;
	else
		geometry.geometryUnion.sphere.radius = m_fBallSize = size;

	// ���� ������
	m_fJumpForce = m_sBallParam[type].fJumpForce;
	m_fMaxLinearVelocity = m_sBallParam[type].fMaxLinearVelocity;
	m_fMaxAngularVelocity = m_sBallParam[type].fMaxAngularVelocity;

	// �ų�� ������ �ٲٱ�
	{
		// �̹� ������ ���νų�尡 ��������ٸ� �ڽ��� ��� ����
		if(m_pMainNode)
			m_pMainNode->removeChild(m_pSphereNode);
		// ���� �ų�尡 �������� �����Ƿ� ����
		else
		{
			m_pMainNode = m_pSmgr->addEmptySceneNode();
			m_pMainNode->setName("Ball");
		}
		
		// �� �ų�� ����
		m_pSphereNode = m_pSmgr->addSphereSceneNode(m_fBallSize);
		m_pSphereNode->setRotation(irr::core::vector3df(30,0,0));

		// �� �ų�带 ���νų���� �ڽ����� ����
		m_pMainNode->addChild(m_pSphereNode);

		// ��Ʈ���� ����
		irr::video::ITexture* texture = m_pSmgr->getVideoDriver()->getTexture(m_sBallParam[type].textureFileName.c_str());
		m_pSphereNode->getMaterial(0).setTexture(0, texture);
		m_pSphereNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}

	// ������ �̹� ����ü�� ������� �����̸� ��ġ�� �ӵ��� ������ �д�. - �� ���� ������ ������ ���ؼ� �ʿ��� �۾�
	irr::core::vector3df position, linerVelocity, angularVelocity;
	if(m_pAnimator)
	{
		// ����� �������ϱ� ���� collisionShape�� �ٽ� ������� �ִϸ����͸� �ٽ� �����Ͽ� �Ӽ��� ������Ѿ� �ϱ� ������ ���� �Ӽ� ����. 
		position = m_pAnimator->getPosition();
		linerVelocity = m_pAnimator->getLinearVelocity();
		angularVelocity = m_pAnimator->getAngularVelocity();

		// �������忡�� ����
		m_pAnimator->RemoveFromWorld();
		m_pMainNode->removeAnimators();
	}

	// �Ҹ� �ִϸ����� �ٽ� ����
	{
		irr::scene::CBulletChracterAnimator *pAnim = m_pBulletPhysicsFactory->createBulletCharcterAnimator(
			m_pSmgr,
			m_pMainNode,
			m_pWorldAnimator->getID(),
			&geometry,
			&m_sBallParam[type].physicsParams
			);

		// ������ �ִϸ����͸� ���γ�忡 �߰�
		m_pMainNode->addAnimator(pAnim);

		// RigidBody ������ ����
		m_pRigidBody = pAnim->getRigidBody();

		// ������ �̹� ���� ������� ���¿����� ���� ��ġ�� �ӵ��� �ٽ� ����
		if(m_pAnimator)
		{
			// ��ġ
			pAnim->setPosition(position);

			// ���ӵ�
			pAnim->setLinearVelocity(linerVelocity);

			// ���ӵ�
			pAnim->setAngularVelocity(angularVelocity);
		}

		// �ִϸ������� ������ ����
		m_pAnimator = pAnim;

		// ������ �ִϸ����� ���.
		pAnim->drop();
	}

	// ������ ���� Ÿ���� ����
	m_eBallType = type;
}


bool CBallObject::InitBallParamFromXMLData()
{
	CIrrXMLReader reader;

	// BALL_TYPE_BASIC XML���� �ε�
	{
		// XML���Ϸκ��� float���� strintc���� ������ ����
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML������ �о���� �Լ��� ȣ���ϰ� XML������ �����Ѵٸ� ���� ����
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/basicball.xml",fData, cData) )
		{
			// fData�� cData���� SBallParam����ü�� ����.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_BASIC], fData, cData);
		}
		else
			return false;
	}

	// BALL_TYPE_beach XML���� �ε�
	{
		// XML���Ϸκ��� float���� strintc���� ������ ����
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML������ �о���� �Լ��� ȣ���ϰ� XML������ �����Ѵٸ� ���� ����
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/beachball.xml",fData, cData) )
		{
			// fData�� cData���� SBallParam����ü�� ����.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_beach], fData, cData);
		}
		else
			return false;
	}
	
	// BALL_TYPE_TENNIS XML���� �ε�
	{
		// XML���Ϸκ��� float���� strintc���� ������ ����
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML������ �о���� �Լ��� ȣ���ϰ� XML������ �����Ѵٸ� ���� ����
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/tennisball.xml",fData, cData) )
		{
			// fData�� cData���� SBallParam����ü�� ����.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_TENNIS], fData, cData);
		}
		else
			return false;
	}
	
	// BALL_TYPE_WOOD XML���� �ε�
	{
		// XML���Ϸκ��� float���� strintc���� ������ ����
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML������ �о���� �Լ��� ȣ���ϰ� XML������ �����Ѵٸ� ���� ����
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/woodball.xml",fData, cData) )
		{
			// fData�� cData���� SBallParam����ü�� ����.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_WOOD], fData, cData);
		}
		else
			return false;
	}
	
	// BALL_TYPE_IRON XML���� �ε�
	{
		// XML���Ϸκ��� float���� strintc���� ������ ����
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML������ �о���� �Լ��� ȣ���ϰ� XML������ �����Ѵٸ� ���� ����
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/ironball.xml",fData, cData) )
		{
			// fData�� cData���� SBallParam����ü�� ����.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_IRON], fData, cData);
		}
		else
			return false;
	}
	
	
	

	return true;
}


void CBallObject::InputSBallParamFromXMLData(SBallParam &param, irr::core::array<irr::f32> &fData, irr::core::array<irr::core::stringc> &cData)
{
	// ���� �����Ӽ�
	param.physicsParams.mass = fData[0];
	param.physicsParams.ccdThreshold = fData[1];
	param.physicsParams.linearDamping = fData[2];
	param.physicsParams.angularDamping = fData[3];
	param.physicsParams.friction = fData[4];
	param.physicsParams.restitution = fData[5];
	param.physicsParams.linearFactor.X = fData[6];
	param.physicsParams.linearFactor.Y = fData[7];
	param.physicsParams.linearFactor.Z = fData[8];
	param.physicsParams.angularFactor.X = fData[9];
	param.physicsParams.angularFactor.Y = fData[10];
	param.physicsParams.angularFactor.Z = fData[11];

	// Collision shape �� ���� ������
	param.geometry.type = irr::scene::CBPAGT_SPHERE;
	param.geometry.geometryUnion.sphere.radius = param.fBallSize = fData[18];

	// ���� ������
	param.fJumpForce = fData[19];
	
	param.fMaxLinearVelocity = fData[20];
	
	param.fMaxAngularVelocity = fData[21];

	// ���� �ؽ��� ���ϸ�
	param.textureFileName = cData[0];
	
	//param.boundSoundFileName = cData[1];
	param.jumpSoundFileName = cData[2];
}

void CBallObject::preLoadSound()
{
	irr::u32 i;
	for(i = 0 ; i < BALL_TYPE_COUNT ; ++i)
	{
		//CApp::Get().NativePreLoadSound(m_sBallParam[i].boundSoundFileName.c_str());
		CApp::Get().NativePreLoadSound(m_sBallParam[i].jumpSoundFileName.c_str());
	}
}
