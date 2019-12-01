#include "PlasmaRifleObject.h"
#include "App.h"

bool CPlasmaRifleObject::Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
{
	if (IWeaponObject::Init(pNode, startPosition))
	{
		irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
		irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
		irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
		irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;
		m_xmlData = &(CApp::GetPtr()->m_sPlasmaRifleData);

		
		// ������������� ���� �ʱ�ȭ �ϱ�
		{
			m_weaponType = m_xmlData->spec[0];
			m_minDamage = m_xmlData->spec[1];
			m_maxDamage = m_xmlData->spec[2];
			m_maxShellCount = m_xmlData->spec[3];
			m_hasShellcount = m_maxShellCount;
			m_maxReloadCount = m_xmlData->spec[4];
			m_hasReloadCount = m_maxReloadCount;
			
			m_coolTime = m_xmlData->spec[5];
			m_aimAccuracy = m_xmlData->spec[6];
			m_maxZoomAngle = m_xmlData->spec[7];
			m_minZoomAngle = m_xmlData->spec[8];
			m_basicZoomAngle = m_xmlData->spec[9];
			
			m_zoomSpeedValue = m_xmlData->spec[10];
			m_flashSize.Height = m_xmlData->spec[11];
			m_flashSize.Width = m_xmlData->spec[12];
			m_flashDurationTime = m_xmlData->spec[13];
			m_flashTick = m_flashDurationTime;
			m_reactionVerticalAngle = m_xmlData->spec[14];

			m_reactionHorizontalAngle = m_xmlData->spec[15];
			m_recoveryAngle = m_xmlData->spec[16];

			m_mass = m_xmlData->physics[0];
			m_friction = m_xmlData->physics[1];
			m_impulseToEnemy = m_xmlData->physics[2];
			m_impulseToHero = m_xmlData->physics[3];

			m_strTypeName = m_xmlData->str[0];
		}

		// ī�޶� ��� ���� ���� ���
		{
			//m_pCamNode = static_cast<irr::scene::ICameraSceneNode*>(pSmgr->getActiveCamera());
		}

		// ������ ��Ʈ��忡 �߰�
		{
			m_pRootNode =  pSmgr->addEmptySceneNode();
			m_pRootNode->addChild(m_pFNode);
		}

		// �ŸŴ����� ȭ����, ����Ʈ�ų�� ���̱�
		{
			m_pSmgr = pSmgr;
			m_pFlashTexture = pDevice->getVideoDriver()->getTexture("Noncharacter/Weapon/Plasmarifle/flash.tga");


			// ȭ���� ��ġ�� ��Ÿ�� �ų�� �ҷ�����
			m_pFireSpotNode = pSmgr->getSceneNodeFromName("firespot",m_pFNode);
			m_pFireSpotNode->setVisible(true);
			 
			irr::f32 colorR = m_xmlData->spec[18];
			irr::f32 colorG = m_xmlData->spec[19];
			irr::f32 colorB = m_xmlData->spec[20];

			m_light.Radius = m_xmlData->spec[17];

			m_light.Attenuation.set(0,1.f/m_light.Radius,0);
			m_light.AmbientColor.set(colorR,colorG,colorB);
			m_light.SpecularColor.set(0,0,0);

			m_pFlashEffectNode = m_pSmgr->addBillboardSceneNode();
			m_pFlashEffectNode->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
			m_pFlashEffectNode->setVisible(false);
			m_pFlashEffectNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
			m_pFlashEffectNode->setMaterialTexture(0, m_pFlashTexture);
			m_pFlashEffectNode->setSize(irr::core::dimension2df(m_flashSize.Height, m_flashSize.Width));
			m_pFireSpotNode->addChild(m_pFlashEffectNode);

			m_pLightEffectNode = m_pSmgr->addLightSceneNode();
			m_pLightEffectNode->setLightData(m_light);
			m_pLightEffectNode->setVisible(false);
			irr::core::vector3df rpos(m_xmlData->spec[21], m_xmlData->spec[22], m_xmlData->spec[23]);
			m_pLightEffectNode->setPosition(rpos);
			m_pFireSpotNode->addChild(m_pLightEffectNode);
		}

		// ���� �ҷ�����
		{
			m_SoundEngine = CApp::GetPtr()->m_SoundEngine;

			SoundSourceDATA* data = &(CApp::GetPtr()->m_sSoundSourceData);

			m_sFire = data->plasmarifle[0];
			m_sDryFire = data->plasmarifle[1];
			m_sReload1 = data->plasmarifle[2];
			m_sReload2 = data->plasmarifle[3];
			m_sSelect = data->plasmarifle[4];
			m_sZoom = data->plasmarifle[5];
			m_sPutaway = data->plasmarifle[6];
		}

		// ���������� �ִϸ����� �����
		{
			// ��������ó���� ���� ��� �ҷ�����
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pFNode);

			// ���忡�� ���� �޽��ų�� �ҷ�����
			m_pWorldBodyNode = pSmgr->getSceneNodeFromName("body_world",m_pFNode);
			m_pWorldBodyNode->setVisible(false);

			// ���ΰ����� ���� �޽��ų�� �ҷ�����
			m_pBodyNode = pSmgr->getSceneNodeFromName("body",m_pFNode);
			m_pBodyNode->setVisible(false);

			// �������� �Ķ���� ����
			irr::scene::CBulletObjectAnimatorParams physicsParams;
			physicsParams.mass = m_mass;
			physicsParams.gravity = irr::core::vector3df(0,0,0);
			physicsParams.friction = m_friction;

			// ���� �ִϸ����� ���̱�
			irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
				pSmgr,
				m_pRootNode,
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);

			m_pRootNode->addAnimator(pAnim);
			m_pAnimator = pAnim;

			//�θ����� �����Ϻ�ȯ�� ����ϱ� ���ؼ� �Ѵܰ����� ��ĳ�Ϻ�ȯ�� �����ش�.
			m_pAnimator->setLocalPosition(
				-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
				);
			pAnim->drop();
		}

		// ���忡 �ٷ� ��������
		{
			m_pFNode->setVisible(false);
			m_pWorldBodyNode->setVisible(false);
			m_pAnimator->setPosition(m_startPosition);
			m_pAnimator->zeroForces();
			m_pFNode->changeAction("stop");

			SetStatus (WORLD);
		}
	}

	return true;
}

void CPlasmaRifleObject::putawayThisWeapon(irr::scene::ISceneNode* pNode, irr::core::vector3df pos)
{
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;
	
	// ������ ī�޶�� �ٲٱ�
	m_pCamNode = pSmgr->getActiveCamera();
	m_pCamNode->setFOV(m_basicZoomAngle * irr::core::DEGTORAD);

	// ������ ��������Ͱ� ���̱�
	//m_pFNode = (irr::scene::jz3d::CFormatedAnimationNode *) pNode;
	
	// ��Ʈ��忡 �޽��ų�带 �ٽ� �ڽ����� ���̰�
	m_pRootNode->addChild(pNode);

	// �������� �Ķ���� ����
	irr::scene::CBulletObjectAnimatorParams physicsParams;
	physicsParams.mass = m_mass;
	physicsParams.gravity = irr::core::vector3df(0,0,0);
	physicsParams.friction = m_friction;

	// ���� �ִϸ����� ���̱�
	irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
		pSmgr,
		m_pRootNode,
		pWorldAnimator->getID(),
		m_pCollMngNode->getGeometryInfo(),
		&physicsParams
		);
	m_pRootNode->addAnimator(pAnim);
	m_pAnimator = pAnim;

	// �θ����� �����Ϻ�ȯ�� ����ϱ� ���ؼ� �Ѵܰ����� ��ĳ�Ϻ�ȯ�� �����ش�.
	m_pAnimator->setLocalPosition(
		-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
		);
	pAnim->drop();

	// ���¸� ���ΰ��� �ƴ� ���忡 �����ִ� ���·� ����
	m_pFNode->setVisible(true);
	weaponVisible(true, false);
	m_pAnimator->setPosition(pos);
	m_pAnimator->zeroForces();
	m_pFNode->changeAction("stop");

	SetStatus (WORLD);

	playEffectSound(PUTAWAY);
}

void CPlasmaRifleObject::playEffectSound(STATUS type)
{
	irrklang::ISoundSource* source;

	switch (type)
	{
	case ATTACK :
		source = m_sFire;
		break;

	case RELOAD :
		source = m_sReload1;
		break;

	case RELOAD2 :
		source = m_sReload2;
		break;

	case DRYFIRE :
		source = m_sDryFire;
		break;

	case SELECT :
		source = m_sSelect;
		break;

	case PUTAWAY :
		source = m_sPutaway;
		break;

	case ZOOMIN :
		source = m_sZoom;
		break;

	case ZOOMOUT :
		source = m_sZoom;
		break;
	}

	m_SoundEngine->play3D(source, getPosition());
}

void CPlasmaRifleObject::selectThisWeapon()
{
	m_pFNode->setVisible(true);
	weaponVisible(false, true);
	m_coolTimeTick = 0.f;
	SetStatus(SELECT);
}

void CPlasmaRifleObject::deselectThisWeapon()
{
	m_pFNode->setVisible(false);
	weaponVisible(false, false);
	m_pFNode->changeAction("stop");
}

void CPlasmaRifleObject::attack(std::vector<PickingResultDATA> &pickingResult)
{
	if(m_hasReloadCount !=0)
	{
		int i;
		for(i = 0 ; i < pickingResult.size() ; i++)
		{
			// ��ŷ�׽�Ʈ �� �浹����� ������
			if(pickingResult[i].hasHit)
			{
				// ĳ���Ϳ� ��Ʈ �Ͽ��ٸ�
				if(pickingResult[i].isCharicterHit)
				{
					//ĳ���� ���� �ڵ�
					WeaponAttackDATA data;
					data.damage = random(m_minDamage, m_maxDamage);
					data.impulse = m_impulseToEnemy;
					data.colData = pickingResult[i].colData;

					pickingResult[i].pickedObj->Signal("damage", &data);
				}
				// ĳ���Ͱ� ��Ʈ���� �ʾҴٸ�, �� �������̳� ��濡 ��Ʈ�Ͽ��ٸ�
				else
				{
					// �����ۿ� ��Ʈ�Ͽ��ٸ�
					if (pickingResult[i].pickedObj != NULL)
					{
						WeaponAttackDATA data;
						data.damage = random(m_minDamage, m_maxDamage);
						data.impulse = m_impulseToEnemy;
						data.colData = pickingResult[i].colData;

						pickingResult[i].pickedObj->Signal("damage", &data);

						// ��ü ������ ������
						btRigidBody* body = static_cast<btRigidBody*>(pickingResult[i].key);

						// ������ ����
						irr::core::vector3df impulse = pickingResult[i].hitDirection * m_impulseToEnemy;

						// ��ü���� �� ����, �����ġ ���� ����.
						btVector3 bImpulse, bRpos;

						// ��ü�� ��ġ ������
						btVector3 bodyPosition = body->getCenterOfMassPosition();
						irr::core::vector3df bodyPos;
						Bullet2Irrlicht(bodyPosition, bodyPos);

						// ���� ��ġ ������
						irr::core::vector3df hitPosition = pickingResult[i].hitPoint;

						// ��ü�߾����κ��� ������� ��ġ ���
						irr::core::vector3df rpos;
						rpos = hitPosition - bodyPos;

						// �Ҹ������� �°� ����
						Irrlicht2Bullet(impulse, bImpulse);
						Irrlicht2Bullet(rpos, bRpos);

						// ��ü�� ���ֱ�. (����, �����ġ)
						body->applyImpulse(bImpulse, bRpos);
					}
					// ��濡 ��Ʈ�Ͽ��ٸ�
					else
					{
						// ��ü ������ ������
						btRigidBody* body = static_cast<btRigidBody*>(pickingResult[i].key);

						// ������ ����
						irr::core::vector3df impulse = pickingResult[i].hitDirection * m_impulseToEnemy;

						// ��ü���� �� ����, �����ġ ���� ����.
						btVector3 bImpulse, bRpos;

						// ��ü�� ��ġ ������
						btVector3 bodyPosition = body->getCenterOfMassPosition();
						irr::core::vector3df bodyPos;
						Bullet2Irrlicht(bodyPosition, bodyPos);

						// ���� ��ġ ������
						irr::core::vector3df hitPosition = pickingResult[i].hitPoint;

						// ��ü�߾����κ��� ������� ��ġ ���
						irr::core::vector3df rpos;
						rpos = hitPosition - bodyPos;

						// �Ҹ������� �°� ����
						Irrlicht2Bullet(impulse, bImpulse);
						Irrlicht2Bullet(rpos, bRpos);

						// ��ü�� ���ֱ�. (����, �����ġ)
						body->applyImpulse(bImpulse, bRpos);
					}
				}
			}
			// ����� ������ ����� �߻縦 �ǹ�
			else
			{

			}
		}

		// ���¸� �������� ��ȯ
		SetStatus(ATTACK);
		// �Ѿ��� 1�� ����.
		m_hasReloadCount--;

		// ȭ���� ����Ʈ ����
		if(!isVisibleFalshEffect())
		{
			if(!m_isZoom)
				setVisibleFlashEffect(true, true);
			else
				setVisibleFlashEffect(false, true);
		}
		m_flashTick = m_flashDurationTime; 


		if(m_pFPSAnim->getFPSCursorContorl())
		{
			// ����? ������? �ݵ�ȿ��
			int value;
			if(rand()%2 == 0)
				value = 1;
			else
				value = -1;

			// �������� �ݵ� ȿ�� �ֱ�
			m_pFPSAnim->addRotation(m_reactionVerticalAngle, m_reactionHorizontalAngle * value);	
		}
		
		// ź�� �����
		makeCartrige();
	}
	else
	{
		if(m_isZoom)
			SetStatus(DRYFIRE);
		else
		{
			if (canReload())
				SetStatus(RELOAD);
			else
				SetStatus(DRYFIRE);
		}
	}
}

void CPlasmaRifleObject::makeCartrige()
{
	//ź�ǰ� Ƣ�� ���� ���ϱ� (���ΰ��� ������� + ������ + ������)
	irr::core::vector3df vDir = CApp::Get().m_pSmgr->getActiveCamera()->getTarget() - CApp::Get().m_objectManager.getHeroObject()->getPosition();
	vDir.normalize();
	irr::core::vector3df vUpDir = CApp::Get().m_pSmgr->getActiveCamera()->getUpVector();
	irr::core::vector3df vRightDir = -vDir.crossProduct(vUpDir);
	vRightDir.normalize();
	irr::core::vector3df finalDir = (
		vRightDir
		+ vUpDir
		+ irr::core::vector3df(0, random(-9, 9)/30.f, random(-9, 9)/ 30.f) // �����̸� �����ϰ� Ƣ��� ���� ȿ��
		).normalize();

	// ź���� ������ġ ���ϱ�
	irr::core::vector3df startPos = 
		CApp::Get().m_objectManager.getHeroObject()->getPosition() 
		+ (vDir * m_xmlData->physics[13])
		+ (vRightDir * m_xmlData->physics[11])
		+ (vUpDir * m_xmlData->physics[12]);

	//ź�� ���� �ڵ�
	SP_IFSMObject spObj = CApp::Get().m_objectManager.createObject(CObjectManager::OBJ_CARTRIGE,
		startPos);

	//�Ѿ��� x������ 90�� ȸ���ϱ�
	btTransform transform = spObj->getRigidBody()->getCenterOfMassTransform();
	transform.setRotation(btQuaternion(0, 90 * irr::core::DEGTORAD, 0));
	spObj->getRigidBody()->setCenterOfMassTransform(transform);

	// ź�ǿ� �� �ֱ�
	static_cast<CCartrigeObject *>( spObj.get() )->applyImpulse(finalDir); 
}

void CPlasmaRifleObject::Update(irr::f32 fDelta)
{
	m_pCamNode = CApp::GetPtr()->m_pSmgr->getActiveCamera();

	switch ( GetStatus() )
	{ 
	case SELECT :
		{
			if(GetStep() == 0)
			{
				playEffectSound(SELECT);

				m_pFNode->changeAction("select", false);
				IncStep();
			}
			else if (GetStep() == 1)
			{
				if(m_pFNode->IsActionFinished())
					SetStatus(STAND);
			}
		}
		break;

	case STAND :
		{
			if(GetStep() == 0)
			{
				m_pFNode->changeAction("stand");
				IncStep();
			}
			
			m_coolTimeTick -=fDelta;
			if (m_coolTimeTick <= 0)
				m_coolTimeTick = 0;
		}
		break;

	case MOVE :
		{
			if(GetStep()==0)
			{
				m_pFNode->changeAction("move", false);
				IncStep();
			}
			else if(GetStep() == 1)
			{
				if(m_pFNode->IsActionFinished())
					SetStatus(STAND);
			}

			m_coolTimeTick -=fDelta;
			if (m_coolTimeTick <= 0)
				m_coolTimeTick = 0;
		}
		break;

	case RUN :
		{
			if(GetStep()==0)
			{
				m_pFNode->changeAction("run", false);
				IncStep();
			}
			else if(GetStep() == 1)
			{
				if(m_pFNode->IsActionFinished())
					SetStatus(STAND);
			}

			m_coolTimeTick -=fDelta;
			if (m_coolTimeTick <= 0)
				m_coolTimeTick = 0;
		}
		break;

	case ATTACK :
		{
			if(GetStep() == 0)
			{
				playEffectSound(ATTACK);

				m_pFNode->changeAction("fire", false);
				IncStep();

				m_coolTimeTick = m_coolTime;
			}
			else if(GetStep() == 1)
			{
				if(m_pFNode->IsActionFinished())
					SetStatus(STAND);

				if(m_pFPSAnim->getFPSCursorContorl())
					m_pFPSAnim->addRotation( (m_recoveryAngle * -1 * fDelta), 0);

				m_coolTimeTick -=fDelta;
				if (m_coolTimeTick <= 0)
					m_coolTimeTick = 0;
			}
		}
		break;

	case DRYFIRE :
		{
			if(GetStep() == 0)
			{
				playEffectSound(DRYFIRE);

				m_pFNode->changeAction("dryfire1", false);
				IncStep();

				m_coolTimeTick = 0.5f;
			}
			else if(GetStep() == 1)
			{
				if(m_pFNode->IsActionFinished())
					SetStatus(STAND);

				m_coolTimeTick -=fDelta;
				if (m_coolTimeTick <= 0)
					m_coolTimeTick = 0;
			}
		}
		break;

	case RELOAD :
		{
			if(GetStep() == 0)
			{
				if (canReload())
				{
					if(m_isZoom)
					{
						IncStep();
						IncStep();
						IncStep();
					}
					else
					{
						playEffectSound(RELOAD);

						m_pFNode->changeAction("reload1", false);
						IncStep();
					}
				}
				else
				{
					SetStatus(STAND);
					IncStep();
				}
			}
			else if(GetStep() == 1)
			{
				if(m_pFNode->IsActionFinished())
				{
					//playEffectSound(RELOAD2);

					m_pFNode->changeAction("reload2", false);
					IncStep();
				}
			}
			else if(GetStep() == 2)
			{
				if(m_pFNode->IsActionFinished())
				{
					// ������ �ڵ� ����
					reload();

					SetStatus(STAND);
					m_coolTimeTick = 0;
				}
			}

			else if(GetStep() == 3)
			{
				if(!zoomOUT(fDelta))
				{
					m_isZoom = false;
					DecStep();
					DecStep();
					DecStep();
				}
			}

		}
		break;

	case ZOOMIN :
		{
			if(!zoomIN(fDelta))
			{
				m_isZoom = true;
				SetStatus(STAND);
			}

			m_coolTimeTick -=fDelta;
			if (m_coolTimeTick <= 0)
				m_coolTimeTick = 0;
		}
		break;

	case ZOOMOUT :
		{
			if(!zoomOUT(fDelta))
			{
				m_isZoom = false;
				SetStatus(STAND);
			}

			m_coolTimeTick -=fDelta;
			if (m_coolTimeTick <= 0)
				m_coolTimeTick = 0;
		}
		break;
	}

	//---------------------
	// �÷��� ȿ�� ������Ʈ
	//---------------------
	m_flashTick -= fDelta;

	if(m_flashTick <= 0.f)
	{
		m_flashTick = 0.f;

		setVisibleFlashEffect(false, false);
	}

	CApp::Get().DrawShellCount(true, getHasReloadShellCount());
}

bool CPlasmaRifleObject::canAttack()
{
	if ( GetStatus() == STAND || GetStatus() == MOVE || GetStatus() == RUN || GetStatus() == ATTACK)
	{
		// ���� ��Ÿ���� �� á���� ���ݰ��ɼ� ����
		if ( m_coolTimeTick <= 0 )
		{
			return true;
		}
		// ���� ��Ÿ���� �� ���� ������ ���� �Ұ�
		else
			return false;

	}
	return false;
}

bool CPlasmaRifleObject::canReload()
{
	// ��������Ÿ���� �ƴϸ� ������ ����
	if ( m_weaponType != TYPE_MELEE )
	{
		// ������ �Ѿ˰����� �ƴϸ� 
		if ( isLimitedShell() )
		{
			// ������ �Ѿ� ������ 0�� �Ǵ� ������ �ƴϸ� ������ ����
			if ( m_hasShellcount > 0 )
			{
				// ���� źâ�� �Ѿ��� �� ���ִٸ� ������ �ʿ�����Ƿ� ������ �Ұ���
				if ( m_maxReloadCount == m_hasReloadCount )
					return false;
				else
					if(m_isZoom)
						return false;
					return true;
			}
		}
		// ������ �Ѿ˰����� ������ ������ ����
		else
			return true;
	}

	return false;
}

bool CPlasmaRifleObject::zoomIN(irr::f32 fDelta)
{
	irr::f32 angle = m_pCamNode->getFOV();
	irr::f32 minAngle = m_minZoomAngle * irr::core::DEGTORAD;

	if ( angle <= minAngle)
	{
		weaponVisible(false, false);
		m_pCamNode->setFOV(minAngle);
		return false;
	}

	angle -= (m_zoomSpeedValue * fDelta);

	m_pCamNode->setFOV(angle);

	return true;
}

bool CPlasmaRifleObject::zoomOUT(irr::f32 fDelta)
{
	weaponVisible(false, true);

	irr::f32 angle = m_pCamNode->getFOV();
	irr::f32 maxAngle = m_basicZoomAngle * irr::core::DEGTORAD;

	if ( angle >= maxAngle)
	{
		m_pCamNode->setFOV(maxAngle);
		return false;
	}

	angle += (m_zoomSpeedValue * fDelta);

	m_pCamNode->setFOV(angle);

	return true;
}