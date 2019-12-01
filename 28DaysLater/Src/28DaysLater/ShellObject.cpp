#include "ShellObject.h"
#include "App.h"

CShellObject::CShellObject(void)
: m_damage (0), m_force (0)
{
	SetStatus (SHELL_READY);
	m_strTypeName = "Shell";
}

CShellObject::~CShellObject(void)
{

}

void CShellObject::Signal(std::string strSignal,void *pParam)
{
	if(isDie() == false)
	{
		if(strSignal == "hit")
		{
			m_pAnimator->zeroForces();		
			SetStatus(SHELL_HIT);
		}
	}
}

void CShellObject::Update(irr::f32 fDelta)
{
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;

	switch(GetStatus())
	{
	case SHELL_READY:
		{
			SetStatus(SHELL_MOVE);
		}
		break;
	
	case SHELL_MOVE:
		{

		}
		break;
	
	case SHELL_HIT:
		{
			if(GetStep() == 0)
			{
				m_pAnimator->RemoveFromWorld();
				irr::scene::ISceneNodeAnimator *pAniDel = pSmgr->createDeleteAnimator(0);
				m_pNode->addAnimator(pAniDel);
				pAniDel->drop();
				IncStep();
			}		
			else
			{
				SetStatus(SHELL_DIE);
			}
		}
		break;
	
	case SHELL_DIE:
		{

		}
		break;	
	}
}

bool CShellObject::isDie()
{
	if ( GetStatus() == SHELL_DIE )
		return true;
	return false;
}

bool CShellObject::Init(void *pParam)
{
	irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;

	ShellDATA *data = static_cast<ShellDATA *>(pParam);

	{
		irr::scene::CBulletObjectAnimatorGeometry geom;
		irr::scene::CBulletObjectAnimatorParams physicsParams;

		irr::scene::ISceneNode *pNode = pSmgr->addSphereSceneNode(.25f, 8);
		pNode->setMaterialFlag(irr::video::EMF_LIGHTING,false);
		pNode->setMaterialFlag(irr::video::EMF_WIREFRAME,true);				
		pNode->setPosition(data->startPosition);
		pNode->setVisible(data->visible);

		geom.type = irr::scene::CBPAGT_SPHERE;
		geom.sphere.radius = .5f;

		physicsParams.mass = data->mass;
		physicsParams.gravity = irr::core::vector3df(0,0,0);
		physicsParams.friction = data->friction;
		physicsParams.restitution = 0.0f;

		irr::scene::CBulletObjectAnimator *pAnim = pBulletPhysicsFactory->createBulletObjectAnimator(
			pSmgr,
			pNode,
			pWorldAnimator->getID(),
			&geom,
			&physicsParams
			);

		pAnim->applyImpulse(data->force * data->aimVector);
		//터널링방지
		//pAnim->setCCD(1,.2f * (.05f));

		pNode->addAnimator(pAnim);

		m_pNode = pNode;
		m_pAnimator = pAnim;

		pAnim->drop();
	}
	return true;
}