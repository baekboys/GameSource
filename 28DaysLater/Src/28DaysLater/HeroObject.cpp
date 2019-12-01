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
	// HeroObject이므로 Hero 설정. (ICharacterObject 클래스)
	m_hero = true;

	// 입력받을 키코드들을 모두 초기화
	int i;
	for(i=0;i<irr::KEY_KEY_CODES_COUNT;i++)
	{
		m_Key[i] = false;
	}	

	// HeroObject의 상태를 READY로 초기화
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

		// 주인공 스탯 초기화 하기
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

			// 카메라 시야거리
			m_farValue = data.spec[6];
		}

		// 사운드엔진
		{
			// 엔진
			m_SoundEngine = CApp::GetPtr()->m_SoundEngine;

			// 사운드소스 데이터
			SoundSourceDATA *data = &(CApp::GetPtr()->m_sSoundSourceData);
			
			m_pSSrcFootstep = data->hero[0];
			m_pSFootStep = m_SoundEngine->play3D(m_pSSrcFootstep, getPosition(), true, true);

			m_pSSrcHurt = data->hero[1];
			m_pSSrcbreath = data->hero[2];
			m_pSSrcLeap = data->hero[3];
		}

		// 물리엔진과 애니메이터 만들기
		{
			// 물리엔진처리를 위한 노드 불러오기
			m_pCollMngNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("col_main",m_pFNode);

			// 물리엔진 파라미터 설정
			irr::scene::CBulletObjectAnimatorParams physicsParams;
			physicsParams.mass = m_mass;
			physicsParams.gravity = irr::core::vector3df(0,0,0);
			physicsParams.friction = m_friction;
			
			// 카메라 얻기
			m_pCamNode = pSmgr->addCameraSceneNode();
			m_pCamNode->setName("usr/cam/main");
			m_pCamNode->setFarValue(m_farValue);
			
			irr::scene::ICameraSceneNode *pCam = pSmgr->addCameraSceneNode(0, irr::core::vector3df(-250,500,-250),irr::core::vector3df(0,0,0));
			pCam->setName("usr/cam/debug");
			

			// FPS카메라 애니메이터 만들기
			irr::scene::CBulletFpsCamAnimator *pAnim = pBulletPhysicsFactory->createBulletFpsCamAnimator(
				pSmgr,
				m_pCamNode,
				pDevice->getCursorControl(),
				pWorldAnimator->getID(),
				m_pCollMngNode->getGeometryInfo(),
				&physicsParams
				);
			
			// 카메라에 애니메이터 추가하고 애니매이터포인터를 멤버변수에 저장.
			m_pCamNode->addAnimator(pAnim);
			m_pAnimator = pAnim;
			pAnim->drop();

			// 카메라신노드를 부모로 설정.
			m_pFNode->setParent(m_pCamNode);
		}

		// 카메라의 위치 설정
		{
			irr::scene::ISceneNode *eyeNode = (irr::scene::jz3d::CCollusionMngNode *)pSmgr->getSceneNodeFromName("eye",m_pFNode);
			m_pAnimator->setLocalPosition(eyeNode->getPosition());//눈의 위치 재조정
		}

		// 무기위치 지정을 위한 노드 설정
		{
			m_pWeaponPositionNode = pSmgr->getSceneNodeFromName("weaponNode",m_pFNode);
		}

		// 강체의 포인터를 멤버변수에 저장.
		{
			m_pCloBody = m_pAnimator->getRigidBody();
		}

		// 충돌처리를 위한 collusionMngNode 얻어오기
		{

			// 충돌매니저에 추가
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
	//	주인공이 살아있을때의 신호처리	//
	//									//
	//////////////////////////////////////
	if(GetStatus() != HERO_DIE)
	{
		if ( strSignal == "damage")
		{
			// 파라미터 형변환
			WeaponAttackDATA* data = static_cast<WeaponAttackDATA*>(pParam);

			// 히트한 방향벡터
			//irr::core::vector3df vDir = data->colData.hitDir;

			// 히트한방향으로 물리적인 힘 주기
			//m_pAnimator->applyImpulse(vDir * data->impulse);

			// 데미지 감소
			m_hp -= data->damage;

			// 사운드 재생
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
	// 방향벡터 구하기
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
	// 싱글턴으로 신매니저, 비디오드라이버, 물리엔진 받아오기
	//-------------------------------------------------------
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::video::IVideoDriver *pVideo = CApp::GetPtr()->m_pVideo;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;


	//// 방향벡터 구하기
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
	//		상태제어		//
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
			// 달리기 지속시간 관리
			{
				m_runDurationTimeTick -= m_runRecovery_stand;

				if ( m_runDurationTimeTick <0 )
				{
					m_runDurationTimeTick = 0;
				}
			}

			// 이동상태 관리
			if ( m_Key[irr::KEY_KEY_W] || m_Key[irr::KEY_KEY_A] || m_Key[irr::KEY_KEY_S] || m_Key[irr::KEY_KEY_D] )
			{
				SetStatus(HERO_MOVE);
			}
		}
		break;

	case CHeroObject::HERO_MOVE :
		{
			// 무기의 이동상태 함수 호출
			{
				if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
				{
					m_weaponInventory[m_currentWeaponNumber].pObj->move();
				}
			}

			// 달리기 지속시간 관리
			{
				m_runDurationTimeTick -= m_runRecovery_move;

				if ( m_runDurationTimeTick <0 )
				{
					m_runDurationTimeTick = 0;
				}
			}

			// 이동상태 관리
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
			// 무기의 이동상태 함수 호출
			{
				if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL)
				{
					m_weaponInventory[m_currentWeaponNumber].pObj->run();
				}
			}

			// 달리기 지속시간 관리
			{
				m_runDurationTimeTick += fDelta;

				if ( m_runDurationTimeTick >= m_runDurationTime )
				{
					m_runDurationTimeTick = m_runDurationTime;
					SetStatus(HERO_REST);

					playEfftectSount(BREATH);
				}
			}

			// 이동상태 관리
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
			// 달리기 지속시간 관리
			{
				m_runDurationTimeTick -= m_runRecovery_stand;

				if ( m_runDurationTimeTick <0 )
				{
					m_runDurationTimeTick = 0;
				}

				// 스테미너가 30% 회복되었을때 stand상태로 변환
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
	//		이동속도 및 발소리 사운드 제어	//
	//										//
	//////////////////////////////////////////
	{
		irr::f32 speed;
		
		switch ( GetStatus() )
		{
		case HERO_MOVE :
			{
				// 발소리를 천천히 하기
				if(m_pSFootStep->getIsPaused())
					m_pSFootStep->setIsPaused(false);
				m_pSFootStep->setPlaybackSpeed( (m_MoveVelocity/ m_RunVelocity) );
				
				// 이동속도
				speed = m_MoveVelocity;
			}
			break;

		case HERO_RUN :
			{
				// 발소리 빠르게 하기
				if(m_pSFootStep->getIsPaused())
					m_pSFootStep->setIsPaused(false);
				m_pSFootStep->setPlaybackSpeed(1.0f);
				
				// 이동속도
				speed = m_RunVelocity;
			}
			break;

		case HERO_ZOOM :
			{
				// 발소리 내기 않기
				m_pSFootStep->setIsPaused(true);
				speed = m_MoveVelocity * 0.5f;
			}
			break;

		default :
			{
				// 발소리 내지 않기
				m_pSFootStep->setIsPaused(true);
				speed = 0;
			}
			break;
		}

		// 여기에 무기의 mass값을 참고하여 이동속도를 계산한다.
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
	//		액션버튼 제어		//
	//							//
	//////////////////////////////

	//---------------------------------------------------------
	// 마우스왼쪽버튼을 눌렀을 경우 (즉, 공격을 실시하는 경우)
	//---------------------------------------------------------
	if ( m_Key[irr::KEY_LBUTTON] )
	{
		// 무기슬롯이 NULL이 아니고, 공격 가능한 상태일 경우에만 공격 실시
		if(m_weaponInventory[m_currentWeaponNumber].pObj != NULL && m_weaponInventory[m_currentWeaponNumber].pObj->canAttack())
		{
			std::vector<PickingResultDATA> pickingresult;
			pickingRayTest(pickingresult);

			m_weaponInventory[m_currentWeaponNumber].pObj->attack(pickingresult);
		}

		//m_Key[irr::KEY_LBUTTON] = false;
	}

	//-----------------------------------------------------------
	// 마우스 오른쪽 버튼 눌렀을 경우 (즉, 특수기능을 하는 경우)
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

				// 줌모드
			case 1:
				{
					if (m_weaponInventory[m_currentWeaponNumber].pObj->canSpecialSkill())
					{
						irr::u32 value = m_weaponInventory[m_currentWeaponNumber].pObj->specialSkill();
					}
				}
				break;

				// 뭔가 다른스킬 차후구현
			case 2:
				{

				}
				break;
			}

		}

		m_Key[irr::KEY_RBUTTON] = false;
	}


	//---------------------------------------------------
	// "R"키를 눌렀을 경우 (즉, 재장전 키를 눌렀을 경우)
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
	// F1키 FPS모드
	//----------------------
	if ( m_Key[irr::KEY_F1] )
	{
		FPSMode();
		m_Key[irr::KEY_F1] = false;
	}

	//----------------------
	// F2키 FPS모드
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
	// F3키 마우스 감도 증가 
	//----------------------
	if ( m_Key[irr::KEY_F3] )
	{
		irr::f32 speed = m_pAnimator->getRotateSpeed();
		m_pAnimator->setRotateSpeed(speed + 5.f);

		m_Key[irr::KEY_F3] = false;
	}

	//----------------------
	// F4키 마우스 감도 감소
	//----------------------
	if ( m_Key[irr::KEY_F4] )
	{
		irr::f32 speed = m_pAnimator->getRotateSpeed();
		m_pAnimator->setRotateSpeed(speed - 5.f);

		m_Key[irr::KEY_F4] = false;
	}

	//////////////////////////////////////////////////////////
	//														//
	//		누적이나 차후 필요한 계산이 있는경우의 구현		//
	//														//
	//////////////////////////////////////////////////////////

	// 스테미너 잔여 퍼센트 계산하기
	{
		m_staminaPercent = static_cast<irr::u16>(100 - ( m_runDurationTimeTick / m_runDurationTime * 100 ));
	}


	//////////////////////////////
	//							//
	//		무기 업데이트		//
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

	// 사운드엔진 업데이트
	{
		// 청취자 위치 업데이트
		m_SoundEngine->setListenerPosition(getPosition(), vDir);

		// 발소리 업데이트
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

	// 피킹레이 테스트에 필요한 변수선언
	irr::core::vector3df vAim, vDir;
	RayMultiResultCallback raycallback;
	irr::core::line3df Ray;
	const irr::f32 shellSize = 0.0005f;

	irr::core::position2di mpos = pDevice->getCursorControl()->getPosition();				

	// 피킹광선얻기
	Ray = pDevice->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(mpos);

	// 광선을 참조하여 시작과 끝벡터 얻기
	vDir = Ray.getVector();
	vDir.normalize();
	//vAim = getPosition() + (vDir * pSmgr->getActiveCamera()->getFarValue());
	vAim = getPosition() + (vDir * 1000);

	// 벡터를 불릿엔진에 맞게 변환
	btVector3 btStart, btAim;
	Irrlicht2Bullet(getPosition(), btStart);
	Irrlicht2Bullet(vAim, btAim);

	// 레이테스트 콜백 객체를 리셋하고 벡터를 설정.
	raycallback.reset();
	raycallback.setRayVector(btStart, btAim);

	// 레이테스트 실시.
	pWorldAnimator->getWorld()->rayTest(raycallback.m_rayFromWorld, raycallback.m_rayToWorld, raycallback);

	// 레이테스트 결과가 있다면
	if ( raycallback.hasHit() )
	{
		int i;
		for ( i = 0 ; i<raycallback.m_RayResults.size() ; i++)
		{

			// CAtlmap에서 찾기에 필요하기 때문에 선언.
			SP_IFSMObject sp_obj;

			// objectMgr에 존재한다면, 즉 IFSMObject객체라면
			if (CApp::GetPtr()->m_objectManager.getObjectData()->Lookup(raycallback.m_RayResults[i].m_collisionObject, sp_obj))
			{
				// 피킹된 오브젝트의 타입명을 얻어옴.
				// 타입명은 "큰객체타입/작은객체타입/실제객체타입"으로 세분화됨.
				// 예를 들어 "char/enemy/zombie" 이런 식으로 이루어짐.
				std::string typeName = sp_obj->m_strTypeName;

				// 토크나이저를 이용하여 타입명을 나눔. 토큰으로 나눌 값은 "/"
				std::vector<string> typeNameResult;
				tokenize(typeName, typeNameResult, "/");

				//-------------------------------------
				// "char" 캐릭터객체
				//-------------------------------------
				if ( typeNameResult[0] == "char" )
				{
					ICharacterObject* pCObj = static_cast<ICharacterObject*>(sp_obj.get());

					// 작은객체타입이 "enemy", 즉 적이면
					if ( typeNameResult[1] == "enemy" )
					{
						// collisionShape단위로 부분충돌검사를 실시
						pCObj->getCollisionMng()->hitTest(getPosition(), vAim, shellSize);

						// 캐릭터의 충돌이 일어났으면
						if ( pCObj->getCollisionMng()->hasHit() )
						{
							PickingResultDATA data;

							//결과를 저장
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
					// 작은객체타입이 "enemy"가 아닌경우... 차후 확장성을 위하여
				}

				//-------------------------------------
				// "nonchar" 비캐릭터 객체
				//-------------------------------------
				else if ( typeNameResult[0] == "nonchar" )
				{
					INonCharacterObject* pNObj = static_cast<INonCharacterObject*>(sp_obj.get());

					PickingResultDATA data;
			
					// 피킹된 오브젝트
					data.pickedObj = sp_obj.get();

					irr::core::vector3df hitpoint, hitnormal;
					Bullet2Irrlicht(raycallback.m_hitPointWorlds[i], hitpoint);
					Bullet2Irrlicht(raycallback.m_RayResults[i].m_hitNormalLocal, hitnormal);
					
					// 피킹된 위치
					data.hitPoint = hitpoint;
					
					// 피킹된 법선
					data.hitNormal = hitnormal;
					data.hasHit = true;
					data.hitTypeName = typeNameResult;
					data.key = raycallback.m_RayResults[i].m_collisionObject;
					data.hitDirection = vDir;

					datas.push_back(data);

					break;
				}
			}

			// objectMgr에 존재하지 않는다면, 즉IFSMObject객체가 아닌 배경이라면
			else
			{
				PickingResultDATA data;

				// 피킹된 오브젝트가 없으므로 NULL
				data.pickedObj = NULL;

				irr::core::vector3df hitpoint, hitnormal;
				Bullet2Irrlicht(raycallback.m_hitPointWorlds[i], hitpoint);
				Bullet2Irrlicht(raycallback.m_RayResults[i].m_hitNormalLocal, hitnormal);

				// 피킹된 위치
				data.hitPoint = hitpoint;

				// 피킹된 법선
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
	// 무기의 타입 얻어오기
	irr::u32 type = obj->getWeaponType();

	// 추후 탐색과 삭제를 위하여 맵의 키값으로 삭제하기 위하여 저장.
	btCollisionObject* key = obj->getRigidBody();

	// 무기의 신노드를 주인공의 무기노드에 자식으로 붙이기
	irr::scene::ISceneNode* pNode = obj->getFormatedAnimationNode();
	m_pWeaponPositionNode->addChild(pNode);
	
	// 무기의 함수호출
	obj->takeThisWeapon();

	// 무기에게 주인공의 FPS애니메이터 넘겨주기
	obj->setFPSAnimator(m_pAnimator);
	
	if(m_weaponInventory[type].pObj != NULL)
	{
		putawayWeapon();
	}

	// 무기 인벤토리에 저장.
	SP_IFSMObject sp_obj;
	CApp::GetPtr()->m_objectManager.getObjectData()->Lookup(key, sp_obj);
	m_weaponInventory[type].pObj = obj;
	m_weaponInventory[type].pMeshNode = pNode;
	m_weaponInventory[type].sp_pObj._Reset(sp_obj);

	// 현재 무기는 주인공에게 있으므로, 맵데이터에서 삭제를 위해 삭제큐에 넣고, 무기의 업데이트는 주인공에서 같이 한다.
	CApp::GetPtr()->m_objectManager.removeObjectFromMap(key);
	
	// 현재 무기를 지금 얻은 무기로 선택.
	m_currentWeaponNumber = type;
}

void CHeroObject::putawayWeapon()
{
	// 무기가 비어있지 않으면
	if( m_weaponInventory[m_currentWeaponNumber].pObj != NULL 
		&& m_weaponInventory[m_currentWeaponNumber].pObj->canPutaway() )
	{
		// 방향벡터 구하기
		irr::core::vector3df vDir(0,0,1);
		irr::core::matrix4 mat;
		mat = m_pFNode->getAbsoluteTransformation();
		mat(3,0) = 0;
		mat(3,1) = 0;
		mat(3,2) = 0;
		mat.transformVect(vDir);
		vDir.normalize();

		// 무기가 던져질 벡터 구하기 (앞방향으로 10.f만큼 떨어진곳에 생성)
		irr::core::vector3df pos = getPosition() + (vDir * 10.f);
		pos.Y = 2.f;

		// 무기의 함수 호출
		m_weaponInventory[m_currentWeaponNumber].pObj->putawayThisWeapon(m_weaponInventory[m_currentWeaponNumber].pMeshNode, pos);

		// 무기를 맵데이터에 다시 넣기 위해, 추가큐에 등록
		objectMgrData data;
		data.colObj = m_weaponInventory[m_currentWeaponNumber].pObj->getRigidBody();
		data.pObj._Reset(m_weaponInventory[m_currentWeaponNumber].sp_pObj);
		CApp::GetPtr()->m_objectManager.addObjectToMap(data);

		// 무기인벤토리를 비움.
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