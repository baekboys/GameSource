#include "CartrigeObject.h"
#include "App.h"

CCartrigeObject::CCartrigeObject(void)
{

}

CCartrigeObject::~CCartrigeObject(void)
{

}

bool CCartrigeObject::Init(irr::core::vector3df startPosition)
{
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	m_pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;

	m_strTypeName = "nonchar/cartrige";

	ObjectData data = CApp::GetPtr()->m_sPlasmaRifleData;

	m_mass = data.physics[4];
	m_friction = data.physics[5];
	m_radius = data.physics[6];
	m_length = data.physics[7];
	irr::video::SColor color(0, (irr::u32)data.physics[8], (irr::u32)data.physics[9], (irr::u32)data.physics[10]);
	m_force = data.physics[14];
	m_durationTime = data.physics[15];

	m_pMainNode = CreateCartrigeScene(m_radius, m_length, color);
	
	irr::scene::CBulletObjectAnimatorGeometry geometry;
	geometry.type = irr::scene::CBPAGT_CYLINDER;
	geometry.box.X = m_radius;
	geometry.box.Y = m_length/2;	
	geometry.box.Z = m_radius;

	// 물리엔진 파라미터 설정
	irr::scene::CBulletObjectAnimatorParams physicsParams;
	physicsParams.mass = m_mass;
	physicsParams.gravity = irr::core::vector3df(0,-50.f,0);
	physicsParams.friction = m_friction;

	{
		// 물리 애니메이터 붙이기
		irr::scene::CBulletObjectAnimator *pAnim = pBulletPhysicsFactory->createBulletObjectAnimator(
			pSmgr,
			m_pMainNode,
			m_pWorldAnimator->getID(),
			&geometry,
			&physicsParams
			);

		m_pMainNode->addAnimator(pAnim);

		m_pAnimator = pAnim;
		pAnim->drop();
	}

	//사운드
	{
		m_pSoundEngine = CApp::GetPtr()->m_SoundEngine;
		SoundSourceDATA* data = &(CApp::GetPtr()->m_sSoundSourceData);
		m_pCartrigeSound = data->plasmarifle[7];
	}

	// 시작위치
	m_pAnimator->setPosition(startPosition);

	// 공중에 떠있는 상태
	SetStatus(AIR);

	return true;
}

void CCartrigeObject::Update(irr::f32 fDelta)
{
	switch (GetStatus())
	{
	case AIR:
		{
			if ( checkCollision() )
			{
				m_timer = 0;
				SetStatus(LAND);
			}
		}
		break;

	case LAND:
		{
			switch(GetStep())
			{
			case 0:
				{
					// 사운드 재생
					m_pSoundEngine->play3D(m_pCartrigeSound, getPosition(), false);

					IncStep();
				}
				break;

			case 1:
				{
					if(m_timer > m_durationTime)
					{
						m_pAnimator->RemoveFromWorld();
						m_pMainNode->remove();
						SetStatus(DIE);
					}
				}
				break;
			}

			m_timer += fDelta;
		}
		break;

	case DIE:
		{

		}
		break;
	}
}

bool CCartrigeObject::checkCollision()
{
	//btTransform transform = getRigidBody()->getWorldTransform();
	//btVector3 down = -transform.getBasis()[1]; //Y축 정보
	//btVector3 forward = transform.getBasis()[2]; //Z축 정보
	//btVector3 pos = transform.getOrigin();

	//btVector3 target[2];
	//target[0] = pos + forward * ( m_radius ) * 1.5f;
	//target[1] = pos + down * ( m_length * 0.5f ) * 1.5f;

	//btCollisionWorld::ClosestRayResultCallback rayCallback1(pos, target[0]);
	//btCollisionWorld::ClosestRayResultCallback rayCallback2(pos, target[1]);

	//m_pWorldAnimator->getWorld()->rayTest(pos, target[0], rayCallback1);
	//m_pWorldAnimator->getWorld()->rayTest(pos, target[1], rayCallback2);

	//if(rayCallback1.hasHit() || rayCallback2.hasHit())
	//	return true;

	irr::u32 count = m_pWorldAnimator->getWorld()->getDispatcher()->getNumManifolds();
	
	irr::u32 i;
	for(i = 0 ; i < count ; i++)
	{
		btPersistentManifold* mani = m_pWorldAnimator->getWorld()->getDispatcher()->getManifoldByIndexInternal(i);
		btRigidBody *body0 = static_cast<btRigidBody*>(mani->getBody0());
		btRigidBody *body1 = static_cast<btRigidBody*>(mani->getBody1());

		if(body0 == getRigidBody() || body1 == getRigidBody())
			return true;
	}

	return false;
}

bool CCartrigeObject::isDie()
{
	if (GetStatus() == DIE)
		return true;
	return false;
}

void CCartrigeObject::Signal(std::string strSignal, void *pParam)
{

}

irr::scene::ISceneNode* CCartrigeObject::CreateCartrigeScene(irr::f32 radius, irr::f32 length, irr::video::SColor color)
{
	irr::scene::ISceneManager* pSmgr = CApp::Get().m_pSmgr;
	irr::scene::IMesh* pMesh = pSmgr->getGeometryCreator()->createCylinderMesh(radius, length, 8, color);
	irr::scene::IMeshSceneNode* pMeshNode = pSmgr->addMeshSceneNode(pMesh);
	pMesh->drop();

	return pMeshNode;
}