#include "HeroObject.h"
#include "App.h"
#include "SRayResultCallback.h"
#include "ZombieObject.h"
#include "tokenizer.h"
#include "PlasmaRifleObject.h"

CHeroObject::CHeroObject(void)
: ICharacterObject(),
m_pAnimator (NULL),
m_currentWeaponNumber(NULL)
{
	// HeroObject�̹Ƿ� Hero ����. (ICharacterObject Ŭ����)
	m_hero = true;

	// �Է¹��� Ű�ڵ���� ��� �ʱ�ȭ
	int i;
	for(i=0;i<irr::KEY_KEY_CODES_COUNT;i++)
	{
		m_Key[i] = false;
	}	

	// HeroObject�� ���¸� READY�� �ʱ�ȭ
	SetStatus(HERO_READY);

	int j;
	for(j = 0; j < 4 ; j++)
	{
		m_weaponInventory[j].pObj = NULL;
		m_weaponInventory[j].pMeshNode = NULL;
	}
}

CHeroObject::~CHeroObject(void)
{
	m_pSFootStep->drop();
}

bool CHeroObject::Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
{
	if ( ICharacterObject::Init(pNode, startPosition))
	{

		irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;
		ObjectData data = CApp::GetPtr()->m_sHeroData;

		// ���ΰ� ���� �ʱ�ȭ �ϱ�
		{
			m_maxHp = data.spec[0];
			m_hp = m_maxHp;
			m_MoveVelocity = data.spec[1];
			m_RunVelocity = data.spec[2];
			m_runDurationTime = data.spec[3];
			m_runRecovery_stand = data.spec[4];
			m_runRecovery_move = data.spec[5];

			m_mass = data.physics[0];
			m_friction = data.physics[1];

			m_strTypeName = data.str[0];

			// ī�޶� �þ߰Ÿ�
			m_farValue = data.spec[6];
		}

		// ���忣��
		{
			// ����
			m_SoundEngine = CApp::GetPtr()->m_SoundEngine;

			// ����ҽ� ������
			SoundSourceDATA *data = &(CApp::GetPtr()->m_sSoundSourceData);
			
			m_pSSrcFootstep = data->hero[0];
			m_pSFootStep = m_SoundEngine->play3D(m_pSSrcFootstep, getPosition(), true, true);

			m_pSSrcHurt = data->hero[1];
			m_pSSrcbreath = data->hero[2];
			m_pSSrcLeap = data->hero[3];
		}

		// ���������� �ִϸ����� �����
		{
			// ��������ó���� ���� ��� �ҷ�����
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pFNode);

			// �������� �Ķ���� ����
			irr::scene::CBulletObjectAnimatorParams physicsParams;
			physicsParams.mass = m_mass;
			physicsParams.gravity = irr::core::vector3df(0,0,0);
			physicsParams.friction = m_friction;
			
			// ī�޶� ���
			m_pCamNode = pSmgr->addCameraSceneNode();
			m_pCamNode->setName("usr/cam/main");
			m_pCamNode->setFarValue(m_farValue);
			
			irr::scene::ICameraSceneNode *pCam = pSmgr->addCameraSceneNode(0, irr::core::vector3df(-250,500,-250),irr::core::vector3df(0,0,0));
			pCam->setName("usr/cam/debug");
			

			// FPSī�޶� �ִϸ����� �����
			irr::scene::CBulletFpsCamAnimator *pAnim = pBulletPhysicsFactory->createBulletFpsCamAnimator(
				pSmgr,
				m_pCamNode,
				pDevice->getCursorControl(),
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);
			
			// ī�޶� �ִϸ����� �߰��ϰ� �ִϸ����������͸� ��������� ����.
			m_pCamNode->addAnimator(pAnim);
			m_pAnimator = pAnim;
			pAnim->drop();

			// ī�޶�ų�带 �θ�� ����.
			m_pFNode->setParent(m_pCamNode);
		}

		// ī�޶��� ��ġ ����
		{
			irr::scene::ISceneNode *eyeNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("eye",m_pFNode);
			m_pAnimator->setLocalPosition(eyeNode->getPosition());//���� ��ġ ������
		}

		// ������ġ ������ ���� ��� ����
		{
			m_pWeaponPositionNode = pSmgr->getSceneNodeFromName("weaponNode",m_pFNode);
		}

		// ��ü�� �����͸� ��������� ����.
		{
			m_pCloBody = m_pAnimator->getRigidBody();
		}

		// �浹ó���� ���� collusionMngNode ������
		{

			// �浹�Ŵ����� �߰�
			irr::scene::jz3d::CCollusionMngNode *pNode = static_cast<irr::scene::jz3d::CCollusionMngNode *>(pSmgr->getSceneNodeFromName("col_main", m_pFNode));
			m_colManager.add(pNode->getName(), pNode);
		}

	}
	return true;
}

void CHeroObject::Signal(std::string strSignal, void *pParam)
{
	//////////////////////////////////////
	//									//
	//	���ΰ��� ����������� ��ȣó��	//
	//									//
	//////////////////////////////////////
	if(GetStatus() != HERO_DIE)
	{
		if ( strSignal == "damage")
		{
			// �Ķ���� ����ȯ
			WeaponAttackDATA* data = static_cast<WeaponAttackDATA*>(pParam);

			// ��Ʈ�� ���⺤��
			//irr::core::vector3df vDir = data->colData.hitDir;

			// ��Ʈ�ѹ������� �������� �� �ֱ�
			//m_pAnimator->applyImpulse(vDir * data->impulse);

			// ������ ����
			m_hp -= data->damage;

			// ���� ���
			playEfftectSount(DAMAGE);

			CApp::Get().FadeOutDamage();

			if(m_hp < 0)
			{
				CApp::Get().gameStart = false;
				CApp::Get().FadeOutDieScene();
				CApp::Get().m_pDevice->getCursorControl()->setVisible(true);
				CApp::Get().m_pCrossHairImage->setVisible(false);
				CApp::Get().CreateDieWindow();
			}

		}
	}
}

irr::core::vector3df CHeroObject::getDirectionVector()
{
	// ���⺤�� ���ϱ�
	irr::core::vector3df vDir(0,0,1);
	{
		irr::core::matrix4 mat;
		mat = m_pFNode->getAbsoluteTransformation();
		mat(3,0) = 0;
		mat(3,1) = 0;
		mat(3,2) = 0;
		mat.transformVect(vDir);
		vDir.normalize();
	}

	return vDir;
}

void CHeroObject::Update(irr::f32 fDelta)
{
	//-------------------------------------------------------
	// �̱������� �ŸŴ���, ��������̹�, �������� �޾ƿ���
	//-------------------------------------------------------
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::video::IVideoDriver *pVideo = CApp::GetPtr()->m_pVideo;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;


	//// ���⺤�� ���ϱ�
	//irr::core::vector3df vDir(0,0,1);
	//{
	//	irr::core::matrix4 mat;
	//	mat = m_pFNode->getAbsoluteTransformation();
	//	mat(3,0) = 0;
	//	mat(3,1) = 0;
	//	mat(3,2) = 0;
	//	mat.transformVect(vDir);
	//	vDir.normalize();
	//}

	irr::core::vector3df vDir = getDirectionVector();


	//////////////////////////
	//						//
	//		��������		//
	//						//
	//////////////////////////

	switch ( GetStatus() )
	{
	case CHeroObject::HERO_READY :
		{
			m_pFNode->setVisible(true);
			m_pAnimator->setPosition(m_startPosition);
			m_pAnimator->zeroForces();

			pSmgr->setActiveCamera(m_pCamNode);

			SetStatus(HERO_STAND);

			playEfftectSount(BREATH);
		}
		break;

	case CHeroObject::HERO_STAND :
		{
			// �޸��� ���ӽð� ����
			{
				m_runDurationTimeTick -= m_runRecovery_stand;

				if ( m_runDurationTimeTick <0 )
				{
					m_runDurationTimeTick = 0;
				}
			}

			// �̵����� ����
			if ( m_Key[irr::KEY_KEY_W] || m_Key[irr::KEY_KEY_A] || m_Key[irr::KEY_KEY_S] || m_Key[irr::KEY_KEY_D] )
			{
				SetStatus(HERO_MOVE);
			}
		}
		break;

	case CHeroObject::HERO_MOVE :
		{
			// ������ �̵����� �Լ� ȣ��
			{
				if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
				{
					m_weaponInventory[m_currentWeaponNumber].pObj->move();
				}
			}

			// �޸��� ���ӽð� ����
			{
				m_runDurationTimeTick -= m_runRecovery_move;

				if ( m_runDurationTimeTick <0 )
				{
					m_runDurationTimeTick = 0;
				}
			}

			// �̵����� ����
			{
				if ( m_Key[irr::KEY_KEY_W] || m_Key[irr::KEY_KEY_A] || m_Key[irr::KEY_KEY_S] || m_Key[irr::KEY_KEY_D] )
				{
					if ( m_Key[irr::KEY_SHIFT] )
					{
						if ( m_staminaPercent != 0 )
							SetStatus(HERO_RUN);
					}
				}
				else
				{
					SetStatus(HERO_STAND);
				}
			}
		}
		break;

	case CHeroObject::HERO_RUN :
		{
			// ������ �̵����� �Լ� ȣ��
			{
				if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
				{
					m_weaponInventory[m_currentWeaponNumber].pObj->run();
				}
			}

			// �޸��� ���ӽð� ����
			{
				m_runDurationTimeTick += fDelta;

				if ( m_runDurationTimeTick >= m_runDurationTime )
				{
					m_runDurationTimeTick = m_runDurationTime;
					SetStatus(HERO_REST);

					playEfftectSount(BREATH);
				}
			}

			// �̵����� ����
			{
				if ( m_Key[irr::KEY_KEY_W] || m_Key[irr::KEY_KEY_A] || m_Key[irr::KEY_KEY_S] || m_Key[irr::KEY_KEY_D] )
				{
					if ( !m_Key[irr::KEY_SHIFT] )
					{
						SetStatus(HERO_MOVE);
					}
				}
				else
				{
					SetStatus(HERO_STAND);
				}
			}
		}
		break;

	case CHeroObject::HERO_REST :
		{
			// �޸��� ���ӽð� ����
			{
				m_runDurationTimeTick -= m_runRecovery_stand;

				if ( m_runDurationTimeTick <0 )
				{
					m_runDurationTimeTick = 0;
				}

				// ���׹̳ʰ� 30% ȸ���Ǿ����� stand���·� ��ȯ
				if ( m_staminaPercent >= 30 )
				{
					SetStatus(HERO_STAND);
				}
			}
		}
		break;
	}


	//////////////////////////////////////////
	//										//
	//		�̵��ӵ� �� �߼Ҹ� ���� ����	//
	//										//
	//////////////////////////////////////////
	{
		irr::f32 speed;
		
		switch ( GetStatus() )
		{
		case HERO_MOVE :
			{
				// �߼Ҹ��� õõ�� �ϱ�
				if(m_pSFootStep->getIsPaused())
					m_pSFootStep->setIsPaused(false);
				m_pSFootStep->setPlaybackSpeed( (m_MoveVelocity/ m_RunVelocity) );
				
				// �̵��ӵ�
				speed = m_MoveVelocity;
			}
			break;

		case HERO_RUN :
			{
				// �߼Ҹ� ������ �ϱ�
				if(m_pSFootStep->getIsPaused())
					m_pSFootStep->setIsPaused(false);
				m_pSFootStep->setPlaybackSpeed(1.0f);
				
				// �̵��ӵ�
				speed = m_RunVelocity;
			}
			break;

		case HERO_ZOOM :
			{
				// �߼Ҹ� ���� �ʱ�
				m_pSFootStep->setIsPaused(true);
				speed = m_MoveVelocity * 0.5f;
			}
			break;

		default :
			{
				// �߼Ҹ� ���� �ʱ�
				m_pSFootStep->setIsPaused(true);
				speed = 0;
			}
			break;
		}

		// ���⿡ ������ mass���� �����Ͽ� �̵��ӵ��� ����Ѵ�.
		{
			if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
			{
				irr::f32 massValue;

				switch(m_currentWeaponNumber)
				{
				case IWeaponObject::TYPE_RIFLE :
					massValue = 0.75;
					break;

				case IWeaponObject::TYPE_PISTOL :
					massValue = 0.85;
					break;

				case IWeaponObject::TYPE_MELEE :
					massValue = 0.95;
					break;

				case IWeaponObject::TYPE_BOME :
					massValue = 0.95;
					break;
				}

				speed *= massValue;
			}
		}

		m_pAnimator->setMoveSpeed( speed * fDelta );
	}

	
	
	//////////////////////////////
	//							//
	//		�׼ǹ�ư ����		//
	//							//
	//////////////////////////////

	//---------------------------------------------------------
	// ���콺���ʹ�ư�� ������ ��� (��, ������ �ǽ��ϴ� ���)
	//---------------------------------------------------------
	if ( m_Key[irr::KEY_LBUTTON] )
	{
		// ���⽽���� NULL�� �ƴϰ�, ���� ������ ������ ��쿡�� ���� �ǽ�
		if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL && m_weaponInventory[m_currentWeaponNumber].pObj->canAttack())
		{
			std::vector<PickingResultDATA> pickingresult;
			pickingRayTest(pickingresult);

			m_weaponInventory[m_currentWeaponNumber].pObj->attack(pickingresult);
		}

		//m_Key[irr::KEY_LBUTTON] = false;
	}

	//-----------------------------------------------------------
	// ���콺 ������ ��ư ������ ��� (��, Ư������� �ϴ� ���)
	//-----------------------------------------------------------
	if (m_Key[irr::KEY_RBUTTON])
	{
		if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
		{
			switch (m_weaponInventory[m_currentWeaponNumber].pObj->hasSpecialSkill())
			{
			case 0:
				{

				}
				break;

				// �ܸ��
			case 1:
				{
					if (m_weaponInventory[m_currentWeaponNumber].pObj->canSpecialSkill())
					{
						irr::u32 value = m_weaponInventory[m_currentWeaponNumber].pObj->specialSkill();
					}
				}
				break;

				// ���� �ٸ���ų ���ı���
			case 2:
				{

				}
				break;
			}

		}

		m_Key[irr::KEY_RBUTTON] = false;
	}


	//---------------------------------------------------
	// "R"Ű�� ������ ��� (��, ������ Ű�� ������ ���)
	//---------------------------------------------------
	if ( m_Key[irr::KEY_KEY_R] )
	{
		if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
		{
			m_weaponInventory[m_currentWeaponNumber].pObj->reloadThisWeapon();
		}

		m_Key[irr::KEY_KEY_R] = false;
	}

	//----------------------
	// F1Ű FPS���
	//----------------------
	if ( m_Key[irr::KEY_F1] )
	{
		FPSMode();
		m_Key[irr::KEY_F1] = false;
	}

	//----------------------
	// F2Ű FPS���
	//----------------------
	if ( m_Key[irr::KEY_F2] )
	{
		if(CApp::Get().m_pCrossHairImage->isVisible())
			CApp::Get().m_pCrossHairImage->setVisible(false);
		else
			CApp::Get().m_pCrossHairImage->setVisible(true);
		m_Key[irr::KEY_F2] = false;
	}

	//----------------------
	// F3Ű ���콺 ���� ���� 
	//----------------------
	if ( m_Key[irr::KEY_F3] )
	{
		irr::f32 speed = m_pAnimator->getRotateSpeed();
		m_pAnimator->setRotateSpeed(speed + 5.f);

		m_Key[irr::KEY_F3] = false;
	}

	//----------------------
	// F4Ű ���콺 ���� ����
	//----------------------
	if ( m_Key[irr::KEY_F4] )
	{
		irr::f32 speed = m_pAnimator->getRotateSpeed();
		m_pAnimator->setRotateSpeed(speed - 5.f);

		m_Key[irr::KEY_F4] = false;
	}

	//////////////////////////////////////////////////////////
	//														//
	//		�����̳� ���� �ʿ��� ����� �ִ°���� ����		//
	//														//
	//////////////////////////////////////////////////////////

	// ���׹̳� �ܿ� �ۼ�Ʈ ����ϱ�
	{
		m_staminaPercent = static_cast<irr::u16>(100 - ( m_runDurationTimeTick / m_runDurationTime * 100 ));
	}


	//////////////////////////////
	//							//
	//		���� ������Ʈ		//
	//							//
	//////////////////////////////
	{
		int i;
		for(i = 0 ; i < 4 ; i++)
		{
			if (m_weaponInventory[i].pObj != NULL)
			{
				m_weaponInventory[i].pObj->Update(fDelta);
			}
		}
	}

	// ���忣�� ������Ʈ
	{
		// û���� ��ġ ������Ʈ
		m_SoundEngine->setListenerPosition(getPosition(), vDir);

		// �߼Ҹ� ������Ʈ
		m_pSFootStep->setPosition(getPosition()); 
	}

}

void CHeroObject::FPSMode()
{ 
	if(m_pAnimator->getFPSCursorContorl())
	{
		m_pAnimator->setFPSCursorControl(false);
		CApp::Get().FPSMode = false;
	}
	else 
	{
		m_pAnimator->setFPSCursorControl(true);
		irr::core::position2di pos(CApp::Get().m_pVideo->getScreenSize().Width/2 - 32, CApp::Get().m_pVideo->getScreenSize().Height/2 - 32);
		CApp::Get().m_pCrossHairImage->setRelativePosition(pos);
		CApp::Get().FPSMode = true;
	}
}

void CHeroObject::pickingRayTest(std::vector<PickingResultDATA> &datas)
{
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;

	// ��ŷ���� �׽�Ʈ�� �ʿ��� ��������
	irr::core::vector3df vAim, vDir;
	RayMultiResultCallback raycallback;
	irr::core::line3df Ray;
	const irr::f32 shellSize = 0.0005f;

	irr::core::position2di mpos = pDevice->getCursorControl()->getPosition();				

	// ��ŷ�������
	Ray = pDevice->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(mpos);

	// ������ �����Ͽ� ���۰� ������ ���
	vDir = Ray.getVector();
	vDir.normalize();
	//vAim = getPosition() + (vDir * pSmgr->getActiveCamera()->getFarValue());
	vAim = getPosition() + (vDir * 1000);

	// ���͸� �Ҹ������� �°� ��ȯ
	btVector3 btStart, btAim;
	Irrlicht2Bullet(getPosition(), btStart);
	Irrlicht2Bullet(vAim, btAim);

	// �����׽�Ʈ �ݹ� ��ü�� �����ϰ� ���͸� ����.
	raycallback.reset();
	raycallback.setRayVector(btStart, btAim);

	// �����׽�Ʈ �ǽ�.
	pWorldAnimator->getWorld()->rayTest(raycallback.m_rayFromWorld, raycallback.m_rayToWorld, raycallback);

	// �����׽�Ʈ ����� �ִٸ�
	if ( raycallback.hasHit() )
	{
		int i;
		for ( i = 0 ; i<raycallback.m_RayResults.size() ; i++)
		{

			// CAtlmap���� ã�⿡ �ʿ��ϱ� ������ ����.
			SP_IFSMObject sp_obj;

			// objectMgr�� �����Ѵٸ�, �� IFSMObject��ü���
			if (CApp::GetPtr()->m_objectManager.getObjectData()->Lookup(raycallback.m_RayResults[i].m_collisionObject, sp_obj))
			{
				// ��ŷ�� ������Ʈ�� Ÿ�Ը��� ����.
				// Ÿ�Ը��� "ū��üŸ��/������üŸ��/������üŸ��"���� ����ȭ��.
				// ���� ��� "char/enemy/zombie" �̷� ������ �̷����.
				std::string typeName = sp_obj->m_strTypeName;

				// ��ũ�������� �̿��Ͽ� Ÿ�Ը��� ����. ��ū���� ���� ���� "/"
				std::vector<string> typeNameResult;
				tokenize(typeName, typeNameResult, "/");

				//-------------------------------------
				// "char" ĳ���Ͱ�ü
				//-------------------------------------
				if ( typeNameResult[0] == "char" )
				{
					ICharacterObject* pCObj = static_cast<ICharacterObject*>(sp_obj.get());

					// ������üŸ���� "enemy", �� ���̸�
					if ( typeNameResult[1] == "enemy" )
					{
						// collisionShape������ �κ��浹�˻縦 �ǽ�
						pCObj->getCollisionMng()->hitTest(getPosition(), vAim, shellSize);

						// ĳ������ �浹�� �Ͼ����
						if ( pCObj->getCollisionMng()->hasHit() )
						{
							PickingResultDATA data;

							//����� ����
							data.pickedObj = sp_obj.get();
							data.colData = pCObj->getCollisionMng()->getCollisionData();
							data.isCharicterHit = true;
							data.hasHit = true;
							data.hitTypeName = typeNameResult;
							data.key = raycallback.m_RayResults[i].m_collisionObject;
							data.hitDirection = vDir;

							datas.push_back(data);

							break;
						}
					}
					// ������üŸ���� "enemy"�� �ƴѰ��... ���� Ȯ�强�� ���Ͽ�
				}

				//-------------------------------------
				// "nonchar" ��ĳ���� ��ü
				//-------------------------------------
				else if ( typeNameResult[0] == "nonchar" )
				{
					INonCharacterObject* pNObj = static_cast<INonCharacterObject*>(sp_obj.get());

					PickingResultDATA data;
			
					// ��ŷ�� ������Ʈ
					data.pickedObj = sp_obj.get();

					irr::core::vector3df hitpoint, hitnormal;
					Bullet2Irrlicht(raycallback.m_hitPointWorlds[i], hitpoint);
					Bullet2Irrlicht(raycallback.m_RayResults[i].m_hitNormalLocal, hitnormal);
					
					// ��ŷ�� ��ġ
					data.hitPoint = hitpoint;
					
					// ��ŷ�� ����
					data.hitNormal = hitnormal;
					data.hasHit = true;
					data.hitTypeName = typeNameResult;
					data.key = raycallback.m_RayResults[i].m_collisionObject;
					data.hitDirection = vDir;

					datas.push_back(data);

					break;
				}
			}

			// objectMgr�� �������� �ʴ´ٸ�, ��IFSMObject��ü�� �ƴ� ����̶��
			else
			{
				PickingResultDATA data;

				// ��ŷ�� ������Ʈ�� �����Ƿ� NULL
				data.pickedObj = NULL;

				irr::core::vector3df hitpoint, hitnormal;
				Bullet2Irrlicht(raycallback.m_hitPointWorlds[i], hitpoint);
				Bullet2Irrlicht(raycallback.m_RayResults[i].m_hitNormalLocal, hitnormal);

				// ��ŷ�� ��ġ
				data.hitPoint = hitpoint;

				// ��ŷ�� ����
				data.hitNormal = hitnormal;
				data.hasHit = true;
				data.key = raycallback.m_RayResults[i].m_collisionObject;
				data.hitDirection = vDir;
				
				datas.push_back(data);
			}
		}

	}
}

void CHeroObject::addWeapon(IWeaponObject* obj)
{
	// ������ Ÿ�� ������
	irr::u32 type = obj->getWeaponType();

	// ���� Ž���� ������ ���Ͽ� ���� Ű������ �����ϱ� ���Ͽ� ����.
	btCollisionObject* key = obj->getRigidBody();

	// ������ �ų�带 ���ΰ��� �����忡 �ڽ����� ���̱�
	irr::scene::ISceneNode* pNode = obj->getFormatedAnimationNode();
	m_pWeaponPositionNode->addChild(pNode);
	
	// ������ �Լ�ȣ��
	obj->takeThisWeapon();

	// ���⿡�� ���ΰ��� FPS�ִϸ����� �Ѱ��ֱ�
	obj->setFPSAnimator(m_pAnimator);
	
	if(m_weaponInventory[type].pObj != NULL)
	{
		putawayWeapon();
	}

	// ���� �κ��丮�� ����.
	SP_IFSMObject sp_obj;
	CApp::GetPtr()->m_objectManager.getObjectData()->Lookup(key, sp_obj);
	m_weaponInventory[type].pObj = obj;
	m_weaponInventory[type].pMeshNode = pNode;
	m_weaponInventory[type].sp_pObj._Reset(sp_obj);

	// ���� ����� ���ΰ����� �����Ƿ�, �ʵ����Ϳ��� ������ ���� ����ť�� �ְ�, ������ ������Ʈ�� ���ΰ����� ���� �Ѵ�.
	CApp::GetPtr()->m_objectManager.removeObjectFromMap(key);
	
	// ���� ���⸦ ���� ���� ����� ����.
	m_currentWeaponNumber = type;
}

void CHeroObject::putawayWeapon()
{
	// ���Ⱑ ������� ������
	if( m_weaponInventory[m_currentWeaponNumber].pObj != NULL 
		&& m_weaponInventory[m_currentWeaponNumber].pObj->canPutaway() )
	{
		// ���⺤�� ���ϱ�
		irr::core::vector3df vDir(0,0,1);
		irr::core::matrix4 mat;
		mat = m_pFNode->getAbsoluteTransformation();
		mat(3,0) = 0;
		mat(3,1) = 0;
		mat(3,2) = 0;
		mat.transformVect(vDir);
		vDir.normalize();

		// ���Ⱑ ������ ���� ���ϱ� (�չ������� 10.f��ŭ ���������� ����)
		irr::core::vector3df pos = getPosition() + (vDir * 10.f);
		pos.Y = 2.f;

		// ������ �Լ� ȣ��
		m_weaponInventory[m_currentWeaponNumber].pObj->putawayThisWeapon(m_weaponInventory[m_currentWeaponNumber].pMeshNode, pos);

		// ���⸦ �ʵ����Ϳ� �ٽ� �ֱ� ����, �߰�ť�� ���
		objectMgrData data;
		data.colObj = m_weaponInventory[m_currentWeaponNumber].pObj->getRigidBody();
		data.pObj._Reset(m_weaponInventory[m_currentWeaponNumber].sp_pObj);
		CApp::GetPtr()->m_objectManager.addObjectToMap(data);

		// �����κ��丮�� ���.
		m_weaponInventory[m_currentWeaponNumber].pMeshNode = NULL;
		m_weaponInventory[m_currentWeaponNumber].pObj = NULL;
		m_weaponInventory[m_currentWeaponNumber].sp_pObj._Reset();

		SetStatus(HERO_STAND);
	}
}

void CHeroObject::playEfftectSount(SOUND type)
{
	irrklang::ISoundSource* source;

	switch (type)
	{
	case DAMAGE :
		source = m_pSSrcHurt;
		break;

	case BREATH :
		source = m_pSSrcbreath;
		break;

	case LEAP :
		source = m_pSSrcLeap;
		break;
	}

	m_SoundEngine->play3D(source, getPosition());
}

bool CHeroObject::OnEvent(const irr::SEvent &event)
{
switch(event.EventType)
	{
	case irr::EET_GUI_EVENT:
		{

		}
		break;
	case irr::EET_KEY_INPUT_EVENT:
		{
			if(event.KeyInput.PressedDown)
			{
				m_Key[event.KeyInput.Key] = true;
			}
			else
			{
				m_Key[event.KeyInput.Key] = false;
			}

			/*irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
			irr::scene::ICameraSceneNode *pCam;
			if(event.KeyInput.Key == irr::KEY_F1)
			{
				pCam = (irr::scene::ICameraSceneNode *)pSmgr->getSceneNodeFromName("usr/cam/main");
				pSmgr->setActiveCamera( pCam );					
			}
			if(event.KeyInput.Key == irr::KEY_F2)
			{
				pCam = (irr::scene::ICameraSceneNode *)pSmgr->getSceneNodeFromName("usr/cam/debug");
				pSmgr->setActiveCamera( pCam );
				pCam->setTarget(getPosition());
			}*/
		}
		break;
	case irr::EET_MOUSE_INPUT_EVENT:
		{
			if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
			{
				m_Key[irr::KEY_LBUTTON] = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP)
			{
				m_Key[irr::KEY_LBUTTON] = false;
			}

			if(event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN)
			{
				m_Key[irr::KEY_RBUTTON] = true;
			}
			else if(event.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP)
			{
				m_Key[irr::KEY_RBUTTON] = false;
			}
		}
		break;
	case irr::EET_USER_EVENT:
		break;
	}
	return false;
}