#include "DynamicObject.h"
#include "App.h"

CDynamicObject::CDynamicObject(void)
{
}

CDynamicObject::~CDynamicObject(void)
{
}

bool CDynamicObject::Init(irr::scene::jz3d::CBulletPhysicsObjectNode* pNode)
{
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;

	m_pSoundEngine = CApp::GetPtr()->m_SoundEngine;

	// 사운드소스 데이터
	SoundSourceDATA *data = &(CApp::GetPtr()->m_sSoundSourceData);

	m_strTypeName="nonchar/dynamic";

	irr::scene::ISceneNode* node = pNode->getParent();
	m_pMainNode = node;

	irr::core::stringc name(node->getName());
	if (name == "cylinder")
	{
		m_type = DYNAMIC_METAL;
		m_pCollisionSound = data->stage[0];
	}
	else if (name == "wood")
	{
		m_type = DYNAMIC_WOOD;
		m_pCollisionSound = data->stage[2];
	}
	else if (name == "ball")
	{
		m_type = DYNAMIC_BALL;
		m_pCollisionSound = data->stage[1];
	}

	{
		// 물리 애니메이터 붙이기
		irr::scene::CBulletObjectAnimator *pAnim = pBulletPhysicsFactory->createBulletObjectAnimator(
			pSmgr,
			m_pMainNode,
			pWorldAnimator->getID(),
			&(pNode->m_GeometryInfo),
			&(pNode->m_PhysicsParam)
			);

		m_pMainNode->addAnimator(pAnim);

		m_pAnimator = pAnim;
		pAnim->drop();
	}

	m_pAnimator->setPosition(m_pMainNode->getAbsolutePosition());

	return true;
}

void CDynamicObject::Signal(std::string strSignal,void *pParam)
{
	if ( strSignal == "damage")
	{
		if(m_pCollisionSound)
		{
			m_pSoundEngine->play3D(m_pCollisionSound, getPosition());
		}
	}
}

void CDynamicObject::Update(irr::f32 fDelta)
{

}

bool CDynamicObject::isDie()
{
	return false;
}