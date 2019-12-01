#include "BallObject.h"
#include "irrFactory.h"
#include "App.h"
#include "bulletEngine.h"
#include "AppDataStructure.h"
#include "IrrXMLReader.h"

// 중력센서의 민감도때문에 무시할 최소값
const irr::f32 accel_minimum_value = 0.02f;

// 중력센서의 흔들기 값
const irr::f32 accel_jump_value = 1.2f;

// 공의 점프후에 다시 점프를 방지하기 위한 타이머
const irr::f32 jumpMinimumTimer = 0.3f;

// 점프를 위해선 공이 바닥에 닿아 있어야 한다. 닿은상태인지 체크하기 위해 광선검사를 하는데
// 광선검사가 공의 반지름 보다 조금 더 길어야 공이 조금 공간이 남아도 점프를 할수 있다.
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
	// 땅에 있을 때만 점프를 실시한다.
	//if(GetStatus() == BALL_OBJ_COLLISION)
	if(CheckPossibleJump() && GetStatus() != BALL_OBJ_ZERO_GRAVITY)
	{
		// 점프 사운드 재생
		{
			CApp::Get().NativePlaySound(m_sBallParam[m_eBallType].jumpSoundFileName.c_str());
			CApp::Get().NativePlayVibration();
		}

		// 그동안의 힘을 없앤다.
		m_pAnimator->getRigidBody()->clearForces();

		// 점프를 위하여 중력의 반대 방향으로 힘을 준다.
		m_pAnimator->applyImpulse(irr::core::vector3df(0,0,1) * m_fJumpForce, irr::core::vector3df(0,5,0));


		// 점프상태로 전환
		//SetStatus(BALL_OBJ_JUMP);

		// 점프 타이머 리셋
		m_fJumpTimer = 0;

		return true;
	}

	return false;
}

bool CBallObject::CheckPossibleJump()
{
	// 점프 타이머가 낮으므로 점프 불가능
	if(m_fJumpTimer < jumpMinimumTimer)
	{
		return false;
	}

	btTransform xform;	
	m_pRigidBody->getMotionState()->getWorldTransform (xform);
	btVector3 raySource = xform.getOrigin();
	btVector3 rayTarget[3];
	
	// 밑부분 충돌방향
	btVector3 down(0,0,-1);
	// 왼쪽밑부분(왼쪽 45도)
	btVector3 down_left(-1,0,-1);
	// 오른쪽밑부분(오른쪽 45도)
	btVector3 down_right(1,0,-1);

	// 3방향으로 광선 검사하기
	rayTarget[0]= xform.getOrigin() + down.normalize() * m_fBallSize * jumpRayTestValue;
	rayTarget[1]= xform.getOrigin() + down_left.normalize() * m_fBallSize * jumpRayTestValue;
	rayTarget[2]= xform.getOrigin() + down_right.normalize() * m_fBallSize * jumpRayTestValue;

	// 광선 콜백
	btCollisionWorld::ClosestRayResultCallback rayCallback_down(raySource, rayTarget[0]);
	btCollisionWorld::ClosestRayResultCallback rayCallback_down_left(raySource, rayTarget[1]);
	btCollisionWorld::ClosestRayResultCallback rayCallback_down_right(raySource, rayTarget[2]);
	
	// 월드 얻기
	btDynamicsWorld* dynamicsWorld = m_pWorldAnimator->getWorld();

	// 바닥 광선 검사
	dynamicsWorld->rayTest(raySource, rayTarget[0], rayCallback_down);
	if (rayCallback_down.hasHit())
	{
		return true;
	}
	// 대각선 검사
	else
	{
		// 왼쪽 검사
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
		// 형변환
		irr::core::vector3df* senser = static_cast<irr::core::vector3df*> ( pParam );
				
		// 쉐이크기능을 하는 경우, 즉 센서값이 일반적이지 않은 값으로 흔들경우에만 가능함.
		if(senser->getLength() > accel_jump_value)
		{
			if(jump())
				m_pGameApp->Signal("zoomOutJump", NULL);
		}
		else
		{
			setMoveVelocity(senser);
		}

		// 가속도 센서 저장
		m_AccelValue = *senser;
	}
	
	// 점프
	else if(strSignal == "jump")
	{
		jump();
	}

	else if(strSignal == "force")
	{
		m_pAnimator->applyImpulse( ( *(static_cast<irr::core::vector3df*>(pParam))) );
	}

	// 포탈 이동
	else if(strSignal == "portal")
	{
		CApp::Get().NativePlaySound("res/sound/portal.mp3");
		m_pAnimator->setPosition( *(static_cast<irr::core::vector3df*>(pParam)) );
		m_pGameApp->Signal("portalEffect", NULL);
	}

	else if(strSignal == "clear")
	{
		SSignalData* data =  static_cast<SSignalData *>(pParam);

		// 공 안보이기
		m_pMainNode->setVisible(false);

		// 스테이지 클리어 시그널을 GameApp에게 보내기
		m_pGameApp->Signal("clearStage", data);
	}

	else if(strSignal == "item")
	{
		CApp::Get().NativePlaySound("res/sound/itemeffect.mp3");
		
		// 아이템효과 리셋
		ResetItemEffect();

		// 아이템효과 적용실시
		m_bHasItemEffect = true;

		SSignalData* data = static_cast<SSignalData *>(pParam);

		// 아이템 타입 저장
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
	// 슬립상태이면 액티브상태로 전환
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
	//			// 충돌 사운드 처리 및 진동
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

	// 아이템 효과 업데이트
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
				// 게임 제한시간 효과는 잔여시간을 표시할 필요 없음.
				m_pGameApp->DrawBallStatus(m_sItemEffect.c_str());
			}
			else
			{
				// 잔여시간을 추가하여 보낸다.
				irr::core::stringw temp(m_sItemEffect);
				temp += " : ";
				temp += static_cast<irr::u32>(m_fItemEffectTimer);
				m_pGameApp->DrawBallStatus(temp.c_str());
			}
		}
	}
	else
	{
		// 아이템 효과가 없으므로 비어있는 문자열보낸다.
		m_pGameApp->DrawBallStatus(L"");
	}

	// 점프 타이머 누적
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

	// 공 보이기
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
	// 겹침쌍의 갯수 얻어오기
	irr::s32 num = m_pWorldAnimator->getWorld()->getDispatcher()->getNumManifolds();

	// 충돌물체는 공 한개 뿐이므로 겹침쌍의 갯수가 1개 이상이면 공이 외부와 충돌된 것으로 판정.
	if(num > 0)
		return true;

	// 충돌하지 않았으므로 false값 리턴
	return false;
}


void CBallObject::ResetItemEffect()
{
	// 아이템효과 없음
	m_bHasItemEffect = false;

	// 아이템 타입 없음
	m_uItemType = NULL;

	// 아이템 효과 값 없음
	m_fItemValue = 0;

	// 아이템 효과 문자열 값 없음
	m_sItemEffect = L"";

	// 아이템효과 지속시간
	m_fItemEffectDurationTime = 0;

	// 아이템효과 타이머
	m_fItemEffectTimer = 0;

	//// 공의 기본적인 사이즈와 현재 공의 사이즈가 다르다면 원상태로 회복시키기
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

	// 공의 점프력
	m_fJumpForce = m_sBallParam[m_eBallType].fJumpForce;

	// 공의 이동속도
	m_fMaxLinearVelocity = m_sBallParam[m_eBallType].fMaxLinearVelocity;

	// 중력 리셋
	m_pGameApp->Signal("gravity on", NULL);
	
	
	if(m_pItemEffectNode)
	{
		m_pItemEffectNode->remove();
		m_pItemEffectNode = NULL;
	}

	// 공의 상태
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
	// Collision shape - 사이즈 차이가 발생할수 있기 때문에 그대로 사용하지 않는다.
	irr::scene::CBulletObjectAnimatorGeometry geometry = m_sBallParam[type].geometry;

	// size값이 NULL이면 타입에 저장된 기본사이즈 사용
	if(size == 0)
		m_fBallSize = m_sBallParam[type].fBallSize;
	else
		geometry.geometryUnion.sphere.radius = m_fBallSize = size;

	// 공의 점프력
	m_fJumpForce = m_sBallParam[type].fJumpForce;
	m_fMaxLinearVelocity = m_sBallParam[type].fMaxLinearVelocity;
	m_fMaxAngularVelocity = m_sBallParam[type].fMaxAngularVelocity;

	// 신노드 사이즈 바꾸기
	{
		// 이미 기존에 메인신노드가 만들어졌다면 자식을 모두 삭제
		if(m_pMainNode)
			m_pMainNode->removeChild(m_pSphereNode);
		// 메인 신노드가 존재하지 않으므로 생성
		else
		{
			m_pMainNode = m_pSmgr->addEmptySceneNode();
			m_pMainNode->setName("Ball");
		}
		
		// 구 신노드 생성
		m_pSphereNode = m_pSmgr->addSphereSceneNode(m_fBallSize);
		m_pSphereNode->setRotation(irr::core::vector3df(30,0,0));

		// 구 신노드를 메인신노드의 자식으로 설정
		m_pMainNode->addChild(m_pSphereNode);

		// 매트리얼 설정
		irr::video::ITexture* texture = m_pSmgr->getVideoDriver()->getTexture(m_sBallParam[type].textureFileName.c_str());
		m_pSphereNode->getMaterial(0).setTexture(0, texture);
		m_pSphereNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}

	// 기존에 이미 공객체가 만들어진 상태이면 위치와 속도를 저장해 둔다. - 즉 공의 사이즈 변경을 위해서 필요한 작업
	irr::core::vector3df position, linerVelocity, angularVelocity;
	if(m_pAnimator)
	{
		// 사이즈를 재조정하기 위해 collisionShape을 다시 만들려면 애니메이터를 다시 생성하여 속성을 적용시켜야 하기 때문에 물리 속성 저장. 
		position = m_pAnimator->getPosition();
		linerVelocity = m_pAnimator->getLinearVelocity();
		angularVelocity = m_pAnimator->getAngularVelocity();

		// 물리월드에서 삭제
		m_pAnimator->RemoveFromWorld();
		m_pMainNode->removeAnimators();
	}

	// 불릿 애니메이터 다시 생성
	{
		irr::scene::CBulletChracterAnimator *pAnim = m_pBulletPhysicsFactory->createBulletCharcterAnimator(
			m_pSmgr,
			m_pMainNode,
			m_pWorldAnimator->getID(),
			&geometry,
			&m_sBallParam[type].physicsParams
			);

		// 생성된 애니메이터를 메인노드에 추가
		m_pMainNode->addAnimator(pAnim);

		// RigidBody 포인터 저장
		m_pRigidBody = pAnim->getRigidBody();

		// 기존에 이미 공이 만들어진 상태였으면 공의 위치와 속도를 다시 적용
		if(m_pAnimator)
		{
			// 위치
			pAnim->setPosition(position);

			// 선속도
			pAnim->setLinearVelocity(linerVelocity);

			// 각속도
			pAnim->setAngularVelocity(angularVelocity);
		}

		// 애니메이터의 포인터 저장
		m_pAnimator = pAnim;

		// 생성된 애니메이터 드롭.
		pAnim->drop();
	}

	// 생성된 공의 타입을 저장
	m_eBallType = type;
}


bool CBallObject::InitBallParamFromXMLData()
{
	CIrrXMLReader reader;

	// BALL_TYPE_BASIC XML파일 로딩
	{
		// XML파일로부터 float값과 strintc값을 저장할 변수
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML파일을 읽어오는 함수를 호출하고 XML파일이 존재한다면 값을 저장
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/basicball.xml",fData, cData) )
		{
			// fData와 cData값을 SBallParam구조체에 저장.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_BASIC], fData, cData);
		}
		else
			return false;
	}

	// BALL_TYPE_beach XML파일 로딩
	{
		// XML파일로부터 float값과 strintc값을 저장할 변수
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML파일을 읽어오는 함수를 호출하고 XML파일이 존재한다면 값을 저장
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/beachball.xml",fData, cData) )
		{
			// fData와 cData값을 SBallParam구조체에 저장.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_beach], fData, cData);
		}
		else
			return false;
	}
	
	// BALL_TYPE_TENNIS XML파일 로딩
	{
		// XML파일로부터 float값과 strintc값을 저장할 변수
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML파일을 읽어오는 함수를 호출하고 XML파일이 존재한다면 값을 저장
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/tennisball.xml",fData, cData) )
		{
			// fData와 cData값을 SBallParam구조체에 저장.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_TENNIS], fData, cData);
		}
		else
			return false;
	}
	
	// BALL_TYPE_WOOD XML파일 로딩
	{
		// XML파일로부터 float값과 strintc값을 저장할 변수
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML파일을 읽어오는 함수를 호출하고 XML파일이 존재한다면 값을 저장
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/woodball.xml",fData, cData) )
		{
			// fData와 cData값을 SBallParam구조체에 저장.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_WOOD], fData, cData);
		}
		else
			return false;
	}
	
	// BALL_TYPE_IRON XML파일 로딩
	{
		// XML파일로부터 float값과 strintc값을 저장할 변수
		irr::core::array<irr::f32> fData;
		irr::core::array<irr::core::stringc> cData;
		// XML파일을 읽어오는 함수를 호출하고 XML파일이 존재한다면 값을 저장
		if( reader.LoadToFloatStringArrayDataFromXMLFile("xml/balldata/ironball.xml",fData, cData) )
		{
			// fData와 cData값을 SBallParam구조체에 저장.
			InputSBallParamFromXMLData(m_sBallParam[BALL_TYPE_IRON], fData, cData);
		}
		else
			return false;
	}
	
	
	

	return true;
}


void CBallObject::InputSBallParamFromXMLData(SBallParam &param, irr::core::array<irr::f32> &fData, irr::core::array<irr::core::stringc> &cData)
{
	// 공의 물리속성
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

	// Collision shape 과 공의 사이즈
	param.geometry.type = irr::scene::CBPAGT_SPHERE;
	param.geometry.geometryUnion.sphere.radius = param.fBallSize = fData[18];

	// 공의 점프력
	param.fJumpForce = fData[19];
	
	param.fMaxLinearVelocity = fData[20];
	
	param.fMaxAngularVelocity = fData[21];

	// 공의 텍스쳐 파일명
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
