#include "AnimationDynamicObject.h"
#include "App.h"

CAnimationDynamicObject::CAnimationDynamicObject(void)
{
}

CAnimationDynamicObject::~CAnimationDynamicObject(void)
{
}

bool CAnimationDynamicObject::Init(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode)
{
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;
	m_pSoundEngine = CApp::GetPtr()->m_SoundEngine;

	m_strTypeName="nonchar/dynamic";

	irr::scene::ISceneNode* node = pNode->getParent();
	m_pFNode = static_cast<irr::scene::jz3d::CFormatedAnimationNode*>(node);

	// 사운드소스 데이터
	SoundSourceDATA *data = &(CApp::GetPtr()->m_sSoundSourceData);

	irr::core::stringc name(node->getName());
	if (name == "metal")
	{
		m_type = DYNAMIC_METAL;
		m_hp = 150;
		m_pCollisionSound = data->stage[0];
	}
	else if (name == "wood")
	{
		m_type = DYNAMIC_WOOD;
		m_hp = 100;
		m_pCollisionSound = data->stage[2];
	}
	

	{
		// 물리 애니메이터 붙이기
		irr::scene::CBulletObjectAnimator *pAnim = pBulletPhysicsFactory->createBulletObjectAnimator(
			pSmgr,
			m_pFNode,
			pWorldAnimator->getID(),
			&(pNode->m_GeometryInfo),
			&(pNode->m_PhysicsParam)
			);

		m_pFNode->addAnimator(pAnim);

		m_pAnimator = pAnim;
		pAnim->drop();
	}

	m_pAnimator->setPosition(m_pFNode->getAbsolutePosition());

	SetStatus(ACTIVE);

	return true;
}

void CAnimationDynamicObject::Signal(std::string strSignal,void *pParam)
{
	if ( strSignal == "damage")
	{
		if(m_pCollisionSound)
		{
			m_pSoundEngine->play3D(m_pCollisionSound, getPosition());
		}

		WeaponAttackDATA* data = static_cast<WeaponAttackDATA*>(pParam);
		m_hp -= data->damage;

		if(m_hp < 0)
		{
			SetStatus(BREAK);
		}
	}
}

void CAnimationDynamicObject::Update(irr::f32 fDelta)
{
	switch (GetStatus())
	{
	case BREAK:
		{
			switch(GetStep())
			{
			case 0:
				{
					m_pFNode->changeAction("break", false);
					
					//사운드 재생
					{

					}

					IncStep();
				}
				break;

			case 1:
				{
					if(m_pFNode->IsActionFinished() )
					{
						SetStatus(DIE);

						if ( m_type == DYNAMIC_WOOD)
						{
							m_pAnimator->RemoveFromWorld();
						}
					}
				}
				break;
			}
		}
		break;

	case DIE:
		{
			
		}
		break;
	}
}

bool CAnimationDynamicObject::isDie()
{
	if(GetStatus()==DELETE)
		return true;
	return false;
}