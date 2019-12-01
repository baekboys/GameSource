#include "ZombieObject.h"
#include "App.h"

CZombieObject::CZombieObject(void)
: ICharacterObject(),
m_tick (0),
m_pAnimator (NULL),
m_isAttackedLeg (false), m_hitAngle(NULL), m_viewLength(0)
{
	// ZombieObject이므로 Hero 설정을 false로 한다. (ICharacterObject 클래스)
	m_hero = false;

	// ZombieObject의 상태를 READY로 초기화
	SetStatus(ZOMBIE_READY);
}

CZombieObject::~CZombieObject(void)
{
}

bool CZombieObject::Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
{
	if ( ICharacterObject::Init(pNode, startPosition) )
	{
		irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;
		ObjectData data = CApp::GetPtr()->m_sZombieData;

		// 좀비 스탯 초기화 하기
		{
			m_maxHp = data.spec[0];
			m_hp = random(m_maxHp-50, m_maxHp);
			m_MoveVelocity = data.spec[1];
			m_RunVelocity = data.spec[2];
			m_runDurationTime = data.spec[3];
			m_runRecovery_stand = data.spec[4];
			
			m_runRecovery_move = data.spec[5];
			m_minDamage = data.spec[6];
			m_maxDamage = data.spec[7];
			m_minStandTime = data.spec[8];
			m_maxStandTime = data.spec[9];

			m_minMoveTime = data.spec[10];
			m_maxMoveTime = data.spec[11];
			m_viewLength = data.spec[12];
			m_attackLength = data.spec[13];
			
			m_mass = data.physics[0];
			m_friction = data.physics[1];
			m_impulseToHero = data.physics[2];

			m_strTypeName = data.str[0];
		}

		// 좀비 카메라 노드 붙이기
		{
			// 타겟노드 설정
			m_pTargetNode =  pSmgr->getSceneNodeFromName("usr/scene/zombie/target", m_pFNode);
			
		}

		// 주인공 오브젝트 정보
		{
			m_heroObj = static_cast<CHeroObject*>(CApp::GetPtr()->m_spHeroObject.get());
		}

		// 물리엔진과 애니메이터 만들기
		{
			// 물리엔진처리를 위한 노드 불러오기
			m_pCollMngNode = static_cast<irr::scene::jz3d::CCollusionMngNode*>(pSmgr->getSceneNodeFromName("col_main",m_pFNode));

			// 물리엔진 파라미터 설정
			irr::scene::CBulletObjectAnimatorParams physicsParams;
			physicsParams.mass = m_mass;
			physicsParams.gravity = irr::core::vector3df(0,0,0);
			physicsParams.friction = m_friction;
			
			// 물리 애니메이터 붙이기
			irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
				pSmgr,
				m_pFNode,
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);

			m_pFNode->addAnimator(pAnim);
			m_pAnimator = pAnim;

			//부모노드의 스케일변환을 고려하기 위해서 한단계위의 스캐일변환을 곱해준다.
			m_pAnimator->setLocalPosition(
				-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
				);

			m_pCloBody = m_pAnimator->getRigidBody();

			pAnim->drop();
		}

		// 충돌처리를 위한 collusionMngNode 얻어오기
		{

			// 충돌매니저에 추가
			irr::scene::ISceneNode *pAniNode = pSmgr->getSceneNodeFromName("body", m_pFNode);
			irr::core::list<irr::scene::ISceneNode*> nodes = pAniNode->getChildren();
			irr::core::list<irr::scene::ISceneNode*>::Iterator it = nodes.begin();
			for (; it != nodes.end(); ++it)
			{
				// 3개의 노드를 제외한 나머지를 충돌매니저에 추가
				if ( strcmp((*it)->getName(), "col_main") != 0)
				{
					irr::scene::jz3d::CCollusionMngNode *pNode = static_cast<irr::scene::jz3d::CCollusionMngNode *>(*it);
					m_colManager.add(pNode->getName(), pNode);
				}
			}
		}

		// 공격 충돌처리를 위한 collusionMngNode 얻어오기
		{
			m_pAttackColNode[0] = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_leftArm2", m_pFNode));
			m_pAttackColNode[1] = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_rightLeg2", m_pFNode));
		}

		// 사운드 소스 로딩
		{
			// 엔진
			m_SoundEngine = CApp::GetPtr()->m_SoundEngine;

			// 사운드소스 데이터
			SoundSourceDATA *data = &(CApp::GetPtr()->m_sSoundSourceData);

			// attack 상태 사운드
			{
				int i;
				for(i = 0 ; i<7 ; i++)
				{
					m_pSSrcAttack[i] = data->zombie[i];
				}
			}

			// stand 상태 사운드
			{
				int i;
				for(i = 0 ; i<4 ; i++)
				{
					m_pSSrcStand[i] = data->zombie[7+i];
				}
			}

			// die 상태 사운드
			{
				int i;
				for(i = 0 ; i<5 ; i++)
				{
					m_pSSrcDie[i] = data->zombie[11+i];
				}
			}

			// move 상태 사운드
			{
				int i;
				for(i = 0 ; i<2 ; i++)
				{
					m_pSSrcMove[i] = data->zombie[16+i];
				}
			}

			// damage 상태 사운드
			{
				m_pSSrcHurt = data->zombie[18];	
			}

			// 헤드샷
			{
				int i;
				for(i = 0 ; i<3 ; i++)
				{
					m_pSSrcHead[i] = data->zombie[19+i];
				}
			}
		}

		// 좀비의 움직임 속도
		{
			m_speed = random(m_RunVelocity - 300, m_RunVelocity);
		}
	}

	return true;
}

void CZombieObject::Signal(std::string strSignal,void *pParam)
{
	//////////////////////////////////////
	//									//
	//	좀비가 살아있을때의  신호처리	//
	//									//
	//////////////////////////////////////
	if ( GetStatus() != ZOMBIE_DIE )
	{
		//-----------------------
		// 데미지받았을때의 신호
		//-----------------------
		if ( strSignal == "damage")
		{
			// 체력이 남아있는지 검사
			if ( m_hp > 0 )
			{
				WeaponAttackDATA* data = static_cast<WeaponAttackDATA*>(pParam);
				std::string hitName = data->colData.hitNodeName;
				irr::f32 damage = data->damage;
				irr::f32 impulse = data->impulse;

				if ( hitName == "col_head" )
				{
					// 헤드샷 : 무조건 한방 !!
					damage = m_maxHp;

					playEffectSound(ZOMBIE_HEAD, random(0, 3));
				}
				else if ( hitName == "col_vest" )
				{
					// 몸샷 : 100% 데미지
					//damage = damage;
				}
				else if ( hitName == "col_leftArm1" || hitName == "col_rightArm1" )
				{
					// 팔 : 60% 데미지, 10% 공격력 감소
					damage = damage * 0.6f;
					m_minDamage *= 0.9f;
					m_maxDamage *= 0.9f;
				}
				else if ( hitName == "col_leftLeg1" || hitName == "col_rightLeg1" )
				{
					// 다리 : 70% 데미지, 걷기 애니메이션 변화
					damage = damage * 0.7f;
					m_isAttackedLeg = true;
				}
				else if ( hitName == "col_leftLeg2" || hitName == "col_rightLeg2" )
				{
					// 발 : 40% 데미지, 걷기 애니메이션 변화
					damage = damage * 0.4f;
					m_isAttackedLeg = true;
				}
				else if ( hitName == "col_leftArm2" || hitName == "col_rightArm2"  )
				{
					// 손 : 30% 데미지, 20% 공격력 감소
					damage = damage * 0.3f;
					m_minDamage *= 0.7f;
					m_maxDamage *= 0.7f;
				}
				
				// 데미지 계산 및 상태와 애니메이션전환 코드 삽입
				{
					// damage 상태의 사운드 재생
					playEffectSound(ZOMBIE_DAMAGE, 0);

					// (1) 변수 전처리
					m_hitAngle = NULL;

					// (2) 데미지계산
					m_hp -= damage;

					// (3) 체력이 0이하면 죽은상태로 전환
					if ( m_hp <= 0 )
					{
						m_hp = m_maxHp;

						// 죽은상태로 전환
						SetStatus (ZOMBIE_DIE);

					}
					// (4) 체력이 아직 남아있으면 데미지 상태로 전환하고 물리충돌 처리 실시
					else
					{
						// 데미지 상태전환
						SetStatus (ZOMBIE_DAMAGE);

						// 물리충돌 구현코드 삽입
						irr::core::vector3df damageVector =  data->colData.hitDir;
						//damageVector.Y = 0;

						m_pAnimator->applyImpulse(damageVector * impulse);
						m_pAnimator->applyImpulse(irr::core::vector3df(0,1,0) * m_mass);
					}

					// (5) 히트된 각도 구하기
					{
						// 총알의 방향구하기
						irr::core::vector3df damageDirVector =  data->colData.hitDir;

						// 좀비의 방향구하기
						irr::core::vector3df zombieDir(0,0,1);
						irr::core::matrix4 mat;
						mat = m_pFNode->getAbsoluteTransformation();
						mat(3,0) = 0;
						mat(3,1) = 0;
						mat(3,2) = 0;
						mat.transformVect(zombieDir);
						zombieDir.normalize();

						// 좀비의 방향을 반대로 하기
						zombieDir *= -1.f;

						// 히트된 각도 구하기
						irr::f32 value = (damageDirVector.dotProduct(zombieDir)) / (damageDirVector.getLength() * zombieDir.getLength() );
						m_hitAngle =  (acosf(value) * irr::core::RADTODEG);
					}
				}
			}
		}
	}
}

void CZombieObject::Update(irr::f32 fDelta)
{
	switch ( GetStatus() )
	{
	case ZOMBIE_READY :
		{
			m_pFNode->setVisible(true);
			m_pAnimator->setPosition(m_startPosition);
			m_pAnimator->zeroForces();

			SetStatus (ZOMBIE_STAND);

			m_randomTimeValue = random(m_minStandTime, m_maxStandTime);
		}
		break;

	case ZOMBIE_STAND :
		{
			if(GetStep() == 0)
			{
				// stand상태의 애니메이션 설정
				if(rand()%2)
					m_pFNode->changeAction("stand1");
				else
					m_pFNode->changeAction("stand2");

				// stand상태의 사운드 재생
				playEffectSound(ZOMBIE_STAND, random(0,3));

				IncStep();
				m_tick = 0; //누적시간 리셋
			}
			else if(GetStep() == 1)
			{		
				if(m_tick >= m_randomTimeValue)
				{
					SetStatus(ZOMBIE_RANDOM_MOVE);
					m_randomTimeValue = random(m_minMoveTime, m_maxMoveTime);

					m_vTargetDir.X = (rand() % 1000) / 1000.f - 0.5f;
					m_vTargetDir.Z = (rand() % 1000) / 1000.f - 0.5f;
					m_vTargetDir.Y = 0;

					m_vTargetDir.normalize();
				}
			}
		}
		break;

	case ZOMBIE_RANDOM_MOVE :
		{
			if(GetStep() == 0)
			{
				m_pFNode->changeAction("walk2");

				// move상태의 사운드 재생
				playEffectSound(ZOMBIE_TARGET_MOVE, random(0,1));
				
				IncStep();

				m_tick = 0; //누적시간 리셋
			}
			else if(GetStep() == 1)
			{
				if(m_tick >= m_randomTimeValue)
				{
					SetStatus(ZOMBIE_STAND);
					m_randomTimeValue = random(m_minStandTime, m_maxStandTime);
				}
				else
				{
					btVector3 WalkVelocity(0,0,0);
					btScalar speed = 0;
					speed = btScalar(1.1) * m_MoveVelocity * fDelta;

					btVector3 btDir;
					Irrlicht2Bullet(m_vTargetDir,btDir);

					WalkVelocity = btDir * speed;
					m_pAnimator->controlStep_Walker(WalkVelocity);

				}			
			}
		}
		break;

	
	case ZOMBIE_DAMAGE :
		{
			if(GetStep() == 0)
			{
				// 정면
				if ( m_hitAngle <= 70.f )
					m_pFNode->changeAction("damage1", false);
				// 측면
				else if ( m_hitAngle > 70.f && m_hitAngle <= 110.f )
					m_pFNode->changeAction("damage3", false);
				// 뒷면
				else
					m_pFNode->changeAction("damage2", false);

				// damage 상태의 사운드 재생
				//playEffectSound(ZOMBIE_DAMAGE, 0);

				IncStep();

				m_tick = 0; //누적시간 리셋
			}
			else if(GetStep() == 1)
			{
				if ( m_pFNode->IsActionFinished() )
				{
					SetStatus(ZOMBIE_STAND);
				}
			}
		}
		break;

	case ZOMBIE_DIE :
		{
			// 죽음 애니메이션 실시
			if (GetStep() == 0)
			{
				// 총알과의 각도가 40도 이하라면, 거의 정면을 바라보고 있다면
				if ( m_hitAngle < 70.f )
					m_pFNode->changeAction("die2", false);
				else
					m_pFNode->changeAction("die1", false);
				
				// attack 상태의 사운드 재생
				playEffectSound(ZOMBIE_DIE, random(0,4));

				IncStep();
			}
			// 죽음 애니메이션이 끝나면 물리엔진 애니메이터 제거
			else if (GetStep() == 1)
			{
				if ( m_pFNode->IsActionFinished() )
				{
					m_tick = 0;
					IncStep();
				}
			}
			// 3초동안 죽음 애니메이션 유지
			else if (GetStep() == 2)
			{
				if ( m_tick > 3.0f)
				{
					m_tick = 0;
					IncStep();
				}
			}
			//  3초동안 바닥으로 신노드를 떨어뜨리기
			else if (GetStep() == 3)
			{
				SetStatus(ZOMBIE_STAND);
			}
		}
	}

	m_tick += fDelta;
}

irrklang::ISound* CZombieObject::playEffectSound(STATUS type, irr::u32 number)
{
	irrklang::ISoundSource* source;

	switch (type)
	{
	case ZOMBIE_ATTACK :
		source = m_pSSrcAttack[number];
		break;

	case ZOMBIE_STAND :
		source = m_pSSrcStand[number];
		break;

	case ZOMBIE_DIE :
		source = m_pSSrcDie[number];
		break;

	case ZOMBIE_TARGET_MOVE :
		source = m_pSSrcMove[number];
		break;

	case ZOMBIE_DAMAGE :
		source = m_pSSrcHurt;
		break;

	case ZOMBIE_HEAD :
		source = m_pSSrcHead[number];
		break;
	}

	return m_SoundEngine->play3D(source, getPosition());
}