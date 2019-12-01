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

		
		// 프라즈마라이플의 스탯 초기화 하기
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

		// 카메라 제어를 위한 변수 등록
		{
			//m_pCamNode = static_cast<irr::scene::ICameraSceneNode*>(pSmgr->getActiveCamera());
		}

		// 무기의 루트노드에 추가
		{
			m_pRootNode =  pSmgr->addEmptySceneNode();
			m_pRootNode->addChild(m_pFNode);
		}

		// 신매니져와 화염구, 라이트신노드 붙이기
		{
			m_pSmgr = pSmgr;
			m_pFlashTexture = pDevice->getVideoDriver()->getTexture("Noncharacter/Weapon/Plasmarifle/flash.tga");


			// 화염구 위치를 나타낼 신노드 불러오기
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

		// 사운드 불러오기
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

		// 물리엔진과 애니메이터 만들기
		{
			// 물리엔진처리를 위한 노드 불러오기
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pFNode);

			// 월드에서 보일 메쉬신노드 불러오기
			m_pWorldBodyNode = pSmgr->getSceneNodeFromName("body_world",m_pFNode);
			m_pWorldBodyNode->setVisible(false);

			// 주인공에게 보일 메쉬신노드 불러오기
			m_pBodyNode = pSmgr->getSceneNodeFromName("body",m_pFNode);
			m_pBodyNode->setVisible(false);

			// 물리엔진 파라미터 설정
			irr::scene::CBulletObjectAnimatorParams physicsParams;
			physicsParams.mass = m_mass;
			physicsParams.gravity = irr::core::vector3df(0,0,0);
			physicsParams.friction = m_friction;

			// 물리 애니메이터 붙이기
			irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
				pSmgr,
				m_pRootNode,
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);

			m_pRootNode->addAnimator(pAnim);
			m_pAnimator = pAnim;

			//부모노드의 스케일변환을 고려하기 위해서 한단계위의 스캐일변환을 곱해준다.
			m_pAnimator->setLocalPosition(
				-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
				);
			pAnim->drop();
		}

		// 월드에 바로 보여지기
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
	
	// 원래의 카메라로 바꾸기
	m_pCamNode = pSmgr->getActiveCamera();
	m_pCamNode->setFOV(m_basicZoomAngle * irr::core::DEGTORAD);

	// 원래의 노드포인터값 붙이기
	//m_pFNode = (irr::scene::jz3d::CFormatedAnimationNode *) pNode;
	
	// 루트노드에 메쉬신노드를 다시 자식으로 붙이가
	m_pRootNode->addChild(pNode);

	// 물리엔진 파라미터 설정
	irr::scene::CBulletObjectAnimatorParams physicsParams;
	physicsParams.mass = m_mass;
	physicsParams.gravity = irr::core::vector3df(0,0,0);
	physicsParams.friction = m_friction;

	// 물리 애니메이터 붙이기
	irr::scene::CBulletChracterAnimator *pAnim = pBulletPhysicsFactory->createBulletCharcterAnimator(
		pSmgr,
		m_pRootNode,
		pWorldAnimator->getID(),
		m_pCollMngNode->getGeometryInfo(),
		&physicsParams
		);
	m_pRootNode->addAnimator(pAnim);
	m_pAnimator = pAnim;

	// 부모노드의 스케일변환을 고려하기 위해서 한단계위의 스캐일변환을 곱해준다.
	m_pAnimator->setLocalPosition(
		-(m_pCollMngNode->getPosition() * m_pCollMngNode->getParent()->getScale())
		);
	pAnim->drop();

	// 상태를 주인공이 아닌 월드에 나와있는 상태로 설정
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
			// 피킹테스트 후 충돌결과가 있으면
			if(pickingResult[i].hasHit)
			{
				// 캐릭터에 히트 하였다면
				if(pickingResult[i].isCharicterHit)
				{
					//캐릭터 공격 코드
					WeaponAttackDATA data;
					data.damage = random(m_minDamage, m_maxDamage);
					data.impulse = m_impulseToEnemy;
					data.colData = pickingResult[i].colData;

					pickingResult[i].pickedObj->Signal("damage", &data);
				}
				// 캐릭터가 히트하지 않았다면, 즉 아이템이나 배경에 히트하였다면
				else
				{
					// 아이템에 히트하였다면
					if (pickingResult[i].pickedObj != NULL)
					{
						WeaponAttackDATA data;
						data.damage = random(m_minDamage, m_maxDamage);
						data.impulse = m_impulseToEnemy;
						data.colData = pickingResult[i].colData;

						pickingResult[i].pickedObj->Signal("damage", &data);

						// 강체 포인터 얻어오기
						btRigidBody* body = static_cast<btRigidBody*>(pickingResult[i].key);

						// 무기의 힘값
						irr::core::vector3df impulse = pickingResult[i].hitDirection * m_impulseToEnemy;

						// 강체에게 줄 힘값, 상대위치 변수 선언.
						btVector3 bImpulse, bRpos;

						// 강체의 위치 얻어오기
						btVector3 bodyPosition = body->getCenterOfMassPosition();
						irr::core::vector3df bodyPos;
						Bullet2Irrlicht(bodyPosition, bodyPos);

						// 맞은 위치 얻어오기
						irr::core::vector3df hitPosition = pickingResult[i].hitPoint;

						// 강체중앙으로부터 상대적인 위치 계산
						irr::core::vector3df rpos;
						rpos = hitPosition - bodyPos;

						// 불릿엔진에 맞게 변형
						Irrlicht2Bullet(impulse, bImpulse);
						Irrlicht2Bullet(rpos, bRpos);

						// 강체에 힘주기. (힘값, 상대위치)
						body->applyImpulse(bImpulse, bRpos);
					}
					// 배경에 히트하였다면
					else
					{
						// 강체 포인터 얻어오기
						btRigidBody* body = static_cast<btRigidBody*>(pickingResult[i].key);

						// 무기의 힘값
						irr::core::vector3df impulse = pickingResult[i].hitDirection * m_impulseToEnemy;

						// 강체에게 줄 힘값, 상대위치 변수 선언.
						btVector3 bImpulse, bRpos;

						// 강체의 위치 얻어오기
						btVector3 bodyPosition = body->getCenterOfMassPosition();
						irr::core::vector3df bodyPos;
						Bullet2Irrlicht(bodyPosition, bodyPos);

						// 맞은 위치 얻어오기
						irr::core::vector3df hitPosition = pickingResult[i].hitPoint;

						// 강체중앙으로부터 상대적인 위치 계산
						irr::core::vector3df rpos;
						rpos = hitPosition - bodyPos;

						// 불릿엔진에 맞게 변형
						Irrlicht2Bullet(impulse, bImpulse);
						Irrlicht2Bullet(rpos, bRpos);

						// 강체에 힘주기. (힘값, 상대위치)
						body->applyImpulse(bImpulse, bRpos);
					}
				}
			}
			// 결과가 없으면 허공에 발사를 의미
			else
			{

			}
		}

		// 상태를 공격으로 전환
		SetStatus(ATTACK);
		// 총알을 1개 줄임.
		m_hasReloadCount--;

		// 화염구 이펙트 생성
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
			// 왼쪽? 오른쪽? 반동효과
			int value;
			if(rand()%2 == 0)
				value = 1;
			else
				value = -1;

			// 라이플의 반동 효과 주기
			m_pFPSAnim->addRotation(m_reactionVerticalAngle, m_reactionHorizontalAngle * value);	
		}
		
		// 탄피 만들기
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
	//탄피가 튀길 방향 구하기 (주인공의 진행방향 + 오른쪽 + 업벡터)
	irr::core::vector3df vDir = CApp::Get().m_pSmgr->getActiveCamera()->getTarget() - CApp::Get().m_objectManager.getHeroObject()->getPosition();
	vDir.normalize();
	irr::core::vector3df vUpDir = CApp::Get().m_pSmgr->getActiveCamera()->getUpVector();
	irr::core::vector3df vRightDir = -vDir.crossProduct(vUpDir);
	vRightDir.normalize();
	irr::core::vector3df finalDir = (
		vRightDir
		+ vUpDir
		+ irr::core::vector3df(0, random(-9, 9)/30.f, random(-9, 9)/ 30.f) // 높낮이를 랜덤하게 튀기기 위한 효과
		).normalize();

	// 탄피의 시작위치 구하기
	irr::core::vector3df startPos = 
		CApp::Get().m_objectManager.getHeroObject()->getPosition() 
		+ (vDir * m_xmlData->physics[13])
		+ (vRightDir * m_xmlData->physics[11])
		+ (vUpDir * m_xmlData->physics[12]);

	//탄피 생성 코드
	SP_IFSMObject spObj = CApp::Get().m_objectManager.createObject(CObjectManager::OBJ_CARTRIGE,
		startPos);

	//총알을 x축으로 90도 회전하기
	btTransform transform = spObj->getRigidBody()->getCenterOfMassTransform();
	transform.setRotation(btQuaternion(0, 90 * irr::core::DEGTORAD, 0));
	spObj->getRigidBody()->setCenterOfMassTransform(transform);

	// 탄피에 힘 주기
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
					// 재장전 코드 삽입
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
	// 플래쉬 효과 업데이트
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
		// 공격 쿨타임이 다 찼으면 공격가능성 있음
		if ( m_coolTimeTick <= 0 )
		{
			return true;
		}
		// 공격 쿨타임이 다 차지 않으면 공격 불가
		else
			return false;

	}
	return false;
}

bool CPlasmaRifleObject::canReload()
{
	// 근접무기타입이 아니면 재장전 가능
	if ( m_weaponType != TYPE_MELEE )
	{
		// 무한의 총알갯수가 아니면 
		if ( isLimitedShell() )
		{
			// 보유한 총알 갯수가 0개 또는 음수가 아니면 재장전 가능
			if ( m_hasShellcount > 0 )
			{
				// 현재 탄창에 총알이 꽉 차있다면 재장전 필요없으므로 재장전 불가능
				if ( m_maxReloadCount == m_hasReloadCount )
					return false;
				else
					if(m_isZoom)
						return false;
					return true;
			}
		}
		// 무한의 총알갯수면 무조건 재장전 가능
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