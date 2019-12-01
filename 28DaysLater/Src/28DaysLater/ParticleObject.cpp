#include "ParticleObject.h"

CParticleObject::CParticleObject(void)
{
}

CParticleObject::~CParticleObject(void)
{
}

bool CParticleObject::Init(irr::core::vector3df pos, irr::core::vector3df dir, irr::u32 type)
{
	return true;
}

CParticleObject::SParticle CParticleObject::CreateParticleObj(irr::core::vector3df pos, irr::core::vector3df dir, irr::u32 type)
{
	//irr::scene::ISceneManager *pSmgr = CApp::GetPtr()->m_pSmgr;
	//irr::IrrlichtDevice *pDevice = CApp::GetPtr()->m_pDevice;
	//irr::scene::CBulletAnimatorManager* pBulletPhysicsFactory = CApp::GetPtr()->m_pBulletPhysicsFactory; 
	//irr::scene::CBulletWorldAnimator* pWorldAnimator = CApp::GetPtr()->m_pWorldAnimator;

	//ObjectData data = CApp::GetPtr()->m_sParticleData;

	//irr::f32 size;
	//irr::f32 number;
	//irr::f32 lightRadius;
	//irr::f32 force;
	//irr::video::SColor color;
	//irr::f32 mass;
	//irr::f32 friction;

	//irr::scene::ISceneNode* pNode = pSmgr->addCubeSceneNode(size);
	//pNode->getMaterial(0).AmbientColor = color;


	//irr::scene::CBulletObjectAnimatorGeometry geometry;
	//geometry.type = irr::scene::CBPAGT_BOX;
	//geometry.box.X = size/2;
	//geometry.box.Y = size/2;	
	//geometry.box.Z = size/2;

	//// 물리엔진 파라미터 설정
	//irr::scene::CBulletObjectAnimatorParams physicsParams;
	//physicsParams.mass = mass;
	//physicsParams.friction = friction;
	//
	//// 물리 애니메이터 붙이기
	//irr::scene::CBulletObjectAnimator *pAnim = pBulletPhysicsFactory->createBulletObjectAnimator(
	//	pSmgr,
	//	pNode,
	//	pWorldAnimator->getID(),
	//	&geometry,
	//	&physicsParams
	//	);

	//pNode->addAnimator(pAnim);
	//pAnim->drop();

	SParticle particle;
	

	
	return particle;
}

void CParticleObject::Signal(std::string strSignal, void *pParam)
{

}

bool CParticleObject::isDie()
{
	if(GetStatus() == DIE)
		return true;
	return false;
}

void CParticleObject::Update(irr::f32 fDelta)
{

}