#include "HL2ZombieObject.h"
#include "App.h"

CHL2ZombieObject::CHL2ZombieObject(void)
: ICharacterObject(),
m_tick (0),
m_pAnimator (NULL),
m_isAttackedLeg (false), m_hitAngle(NULL), m_viewLength(0)
{
	// ZombieObject�̹Ƿ� Hero ������ false�� �Ѵ�. (ICharacterObject Ŭ����)
	m_hero = false;

	// ZombieObject�� ���¸� READY�� �ʱ�ȭ
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

		// ���� ���� �ʱ�ȭ �ϱ�
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

		// ���� ī�޶� ��� ���̱�
		{
			// Ÿ�ٳ�� ����
			m_pTargetNode =  pSmgr->getSceneNodeFromName("usr/scene/HL2zombie/target", m_pFNode);
			
		}

		// ���ΰ� ������Ʈ ����
		{
			m_heroObj = static_cast<CHeroObject*>(CApp::GetPtr()->m_spHeroObject.get());
		}

		// ���������� �ִϸ����� �����
		{
			// ��������ó���� ���� ��� �ҷ�����
			m_pCollMngNode = static_cast<irr::scene::jz3d::CCollusionMngNode*>(pSmgr->getSceneNodeFromName("col_main",m_pFNode));

			// �������� �Ķ���� ����
			irr::scene::CBulletObjectAnimatorParams physicsParams;
			physicsParams.mass = m_mass;
			physicsParams.gravity = irr::core::vector3df(0,0,0);
			physicsParams.friction = m_friction;
			
			// ���� �ִϸ����� ���̱�
			irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
				pSmgr,
				m_pFNode,
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);

			m_pFNode->addAnimator(pAnim);
			m_pAnimator = pAnim;

			//�θ����� �����Ϻ�ȯ�� ����ϱ� ���ؼ� �Ѵܰ����� ��ĳ�Ϻ�ȯ�� �����ش�.
			m_pAnimator->setLocalPosition(
				-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
				);

			m_pCloBody = m_pAnimator->getRigidBody();

			pAnim->drop();
		}

		// �浹ó���� ���� collusionMngNode ������
		{

			// �浹�Ŵ����� �߰�
			irr::scene::ISceneNode *pAniNode = pSmgr->getSceneNodeFromName("body", m_pFNode);
			irr::core::list<irr::scene::ISceneNode*> nodes = pAniNode->getChildren();
			irr::core::list<irr::scene::ISceneNode*>::Iterator it = nodes.begin();
			for (; it != nodes.end(); ++it)
			{
				std::string name = (*it)->getName();
				
				// 3���� ��带 ������ �������� �浹�Ŵ����� �߰�
				if ( name != "col_main" || name != "col_lHand" || name != "col_rHand" )
				{
					irr::scene::jz3d::CCollusionMngNode *pNode = static_cast<irr::scene::jz3d::CCollusionMngNode *>(*it);
					m_colManager.add(pNode->getName(), pNode);
				}
			}
		}

		// ���� �浹ó���� ���� collusionMngNode ������
		{
			m_pAttackColNode[0] = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_lHand", m_pFNode));
			m_pAttackColNode[1] = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_rHand", m_pFNode));
		}

		// ���� �ҽ� �ε�
		{
			// ����
			m_SoundEngine = CApp::GetPtr()->m_SoundEngine;

			// ����ҽ� ������
			SoundSourceDATA *data = &(CApp::GetPtr()->m_sSoundSourceData);

			// attack ���� ����
			{
				int i;
				for(i = 0 ; i<7 ; i++)
				{
					m_pSSrcAttack[i] = data->zombie[i];
				}
			}

			// stand ���� ����
			{
				int i;
				for(i = 0 ; i<4 ; i++)
				{
					m_pSSrcStand[i] = data->zombie[7+i];
				}
			}

			// die ���� ����
			{
				int i;
				for(i = 0 ; i<5 ; i++)
				{
					m_pSSrcDie[i] = data->zombie[11+i];
				}
			}

			// move ���� ����
			{
				int i;
				for(i = 0 ; i<2 ; i++)
				{
					m_pSSrcMove[i] = data->zombie[16+i];
				}
			}

			// damage ���� ����
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
	//	���� �����������  ��ȣó��	//
	//									//
	//////////////////////////////////////
	if ( GetStatus() != ZOMBIE_DIE )
	{
		//-----------------------
		// �������޾������� ��ȣ
		//-----------------------
		if ( strSignal == "damage")
		{
			// ü���� �����ִ��� �˻�
			if ( m_hp > 0 )
			{
				WeaponAttackDATA* data = static_cast<WeaponAttackDATA*>(pParam);
				std::string hitName = data->colData.hitNodeName;
				irr::f32 damage = data->damage;
				irr::f32 impulse = data->impulse;

				if ( hitName == "col_head" )
				{
					// ��弦 : ������ �ѹ� !!
					damage = m_maxHp;
				}
				else if ( hitName == "col_body" )
				{
					// ���� : 100% ������
					//damage = damage;
				}
				else if ( hitName == "col_lArm1" || hitName == "col_rArm1")
				{
					// �� : 60% ������, 10% ���ݷ� ����
					damage = damage * 0.6f;
					m_minDamage *= 0.9f;
					m_maxDamage *= 0.9f;
				}
				else if ( hitName == "col_lLeg1" || hitName == "col_rLeg1")
				{
					// �ٸ� : 70% ������, �ȱ� �ִϸ��̼� ��ȭ
					damage = damage * 0.7f;
					m_isAttackedLeg = true;
				}
				else if ( hitName == "col_lLeg2" || hitName == "col_rLeg2" )
				{
					// �� : 40% ������, �ȱ� �ִϸ��̼� ��ȭ
					damage = damage * 0.4f;
					m_isAttackedLeg = true;
				}
				else if ( hitName == "col_lArm2" || hitName == "col_rArm2"  )
				{
					// �� : 30% ������, 20% ���ݷ� ����
					damage = damage * 0.3f;
					m_minDamage *= 0.7f;
					m_maxDamage *= 0.7f;
				}
				
				// ������ ��� �� ���¿� �ִϸ��̼���ȯ �ڵ� ����
				{
					// (1) ���� ��ó��
					m_hitAngle = NULL;

					// (2) ���������
					m_hp -= damage;

					// (3) ü���� 0���ϸ� �������·� ��ȯ
					if ( m_hp <= 0 )
					{
						m_hp = 0;

						// deactive���� ��ȯ
						Signal("deactive", NULL);

						// �������·� ��ȯ
						SetStatus (ZOMBIE_DIE);

					}
					// (4) ü���� ���� ���������� ������ ���·� ��ȯ�ϰ� �����浹 ó�� �ǽ�
					else
					{
						// ������ ������ȯ
						SetStatus (ZOMBIE_DAMAGE);

						// �����浹 �����ڵ� ����
						irr::core::vector3df damageVector =  data->colData.hitDir;
						//damageVector.Y = 0;

						m_pAnimator->applyImpulse(damageVector * impulse);
						m_pAnimator->applyImpulse(irr::core::vector3df(0,1,0) * m_mass);
					}

					// (5) ��Ʈ�� ���� ���ϱ�
					{
						// �Ѿ��� ���ⱸ�ϱ�
						irr::core::vector3df damageDirVector =  data->colData.hitDir;

						// ������ ���ⱸ�ϱ�
						irr::core::vector3df zombieDir(0,0,1);
						irr::core::matrix4 mat;
						mat = m_pFNode->getAbsoluteTransformation();
						mat(3,0) = 0;
						mat(3,1) = 0;
						mat(3,2) = 0;
						mat.transformVect(zombieDir);
						zombieDir.normalize();

						// ������ ������ �ݴ�� �ϱ�
						zombieDir *= -1.f;

						// ��Ʈ�� ���� ���ϱ�
						irr::f32 value = (damageDirVector.dotProduct(zombieDir)) / (damageDirVector.getLength() * zombieDir.getLength() );
						m_hitAngle =  (acosf(value) * irr::core::RADTODEG);
					}
				}
			}
		}
	}

	//////////////////////////////////////
	//									//
	//	���� ���������϶��� ��ȣó��	//
	//									//
	//////////////////////////////////////
	else
	{

	}


	//////////////////////////////////////////////////
	//												//
	//	���¿� ������� ������ �ؾ��ϴ� ��ȣó��	//
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
			// ���� �ִϸ��̼� �ǽ�
			if (GetStep() == 0)
			{
				// �Ѿ˰��� ������ 40�� ���϶��, ���� ������ �ٶ󺸰� �ִٸ�
				if ( m_hitAngle < 40.f )
					m_pFNode->changeAction("die2", false);
				else
					m_pFNode->changeAction("die1", false);

				// attack ������ ���� ���
				playEffectSound(ZOMBIE_DIE, random(0,4));

				IncStep();
			}
			// ���� �ִϸ��̼��� ������ �������� �ִϸ����� ����
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
			// 3�ʵ��� ���� �ִϸ��̼� ����
			else if (GetStep() == 2)
			{
				if ( m_tick > 3.0f)
				{
					m_tick = 0;

					IncStep();
				}
			}
			//  3�ʵ��� �ٴ����� �ų�带 ����߸���
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