#include "HL2ZombieObject.h"
#include "App.h"

CHL2ZombieObject::CHL2ZombieObject(void)
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

CHL2ZombieObject::~CHL2ZombieObject(void)
{
}

bool CHL2ZombieObject::Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
{
	if ( ICharacterObject::Init(pNode, startPosition) )
	{
		irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;
		ObjectData data = CApp::GetPtr()->m_sHL2ZombieData;

		// 좀비 스탯 초기화 하기
		{
			m_maxHp = data.spec[0];
			m_hp = m_maxHp;
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
			m_pTargetNode =  pSmgr->getSceneNodeFromName("usr/scene/HL2zombie/target", m_pFNode);
			
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
				std::string name = (*it)->getName();
				
				// 3개의 노드를 제외한 나머지를 충돌매니저에 추가
				if ( name != "col_main" || name != "col_lHand" || name != "col_rHand" )
				{
					irr::scene::jz3d::CCollusionMngNode *pNode = static_cast<irr::scene::jz3d::CCollusionMngNode *>(*it);
					m_colManager.add(pNode->getName(), pNode);
				}
			}
		}

		// 공격 충돌처리를 위한 collusionMngNode 얻어오기
		{
			m_pAttackColNode[0] = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_lHand", m_pFNode));
			m_pAttackColNode[1] = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_rHand", m_pFNode));
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
		}
	}

	return true;
}

void CHL2ZombieObject::Signal(std::string strSignal,void *pParam)
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
				}
				else if ( hitName == "col_body" )
				{
					// 몸샷 : 100% 데미지
					//damage = damage;
				}
				else if ( hitName == "col_lArm1" || hitName == "col_rArm1")
				{
					// 팔 : 60% 데미지, 10% 공격력 감소
					damage = damage * 0.6f;
					m_minDamage *= 0.9f;
					m_maxDamage *= 0.9f;
				}
				else if ( hitName == "col_lLeg1" || hitName == "col_rLeg1")
				{
					// 다리 : 70% 데미지, 걷기 애니메이션 변화
					damage = damage * 0.7f;
					m_isAttackedLeg = true;
				}
				else if ( hitName == "col_lLeg2" || hitName == "col_rLeg2" )
				{
					// 발 : 40% 데미지, 걷기 애니메이션 변화
					damage = damage * 0.4f;
					m_isAttackedLeg = true;
				}
				else if ( hitName == "col_lArm2" || hitName == "col_rArm2"  )
				{
					// 손 : 30% 데미지, 20% 공격력 감소
					damage = damage * 0.3f;
					m_minDamage *= 0.7f;
					m_maxDamage *= 0.7f;
				}
				
				// 데미지 계산 및 상태와 애니메이션전환 코드 삽입
				{
					// (1) 변수 전처리
					m_hitAngle = NULL;

					// (2) 데미지계산
					m_hp -= damage;

					// (3) 체력이 0이하면 죽은상태로 전환
					if ( m_hp <= 0 )
					{
						m_hp = 0;

						// deactive상태 전환
						Signal("deactive", NULL);

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

	//////////////////////////////////////
	//									//
	//	좀비가 죽은상태일때의 신호처리	//
	//									//
	//////////////////////////////////////
	else
	{

	}


	//////////////////////////////////////////////////
	//												//
	//	상태에 상관없이 무조건 해야하는 신호처리	//
	//												//
	//////////////////////////////////////////////////
}

void CHL2ZombieObject::Update(irr::f32 fDelta)
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
			m_pFNode->changeAction("test");
		}
		break;

	case ZOMBIE_DIE :
		{
			// 죽음 애니메이션 실시
			if (GetStep() == 0)
			{
				// 총알과의 각도가 40도 이하라면, 거의 정면을 바라보고 있다면
				if ( m_hitAngle < 40.f )
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
					m_pAnimator->RemoveFromWorld();
					m_pFNode->removeAnimators();
					
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
				if ( m_tick <= 3.0f )
				{
					irr::core::vector3df pos = m_pFNode->getPosition();
					pos.Y = pos.Y - (fDelta * 1.f);
					m_pFNode->setPosition(pos);
				}
				else
				{
					irr::scene::ISceneNodeAnimator* pAni =  CApp::GetPtr()->m_pSmgr->createDeleteAnimator(0);
					m_pFNode->addAnimator(pAni);
					pAni->drop();
					SetStatus(ZOMBIE_REMOVE);
				}
			}
		}
	}

	m_tick += fDelta;
}

void CHL2ZombieObject::playEffectSound(STATUS type, irr::u32 number)
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
	}

	m_SoundEngine->play3D(source, getPosition());
}