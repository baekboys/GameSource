#include "DirectionObject.h"

CDirectionObject::CDirectionObject(void) 
	: m_pMainNode(NULL), m_fSize(10), m_pDirectionAnimator(NULL)
{
}


CDirectionObject::~CDirectionObject(void)
{
}

bool CDirectionObject::Init(
	irr::scene::ISceneManager *pSmgr,
	CBallObject* pObj,
	irr::f32 size, irr::f32 farValueFromBall, irr::f32 farValueFromBackground,
	irr::core::vector3df endPosition
	)
{
	// �ŸŴ��� ����
	m_pSmgr = pSmgr;

	// ������
	m_fSize = size;

	// ȭ��ǥ�޽� �����
	irr::scene::IMesh *pMesh = m_pSmgr->addArrowMesh(
				"DirectionMesh",
				irr::video::SColor(255,255,255,255),
				irr::video::SColor(255,255,255,255),
				4, 8, m_fSize, m_fSize * 0.6f, m_fSize * 0.3f, m_fSize * 0.5f
				);

	// ȭ��ǥ �ų�� �����
	m_pMainNode = m_pSmgr->addMeshSceneNode(pMesh);

	// ȭ��ǥ �ų�� ����
	m_pMainNode->setName("Direction");
	m_pMainNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	irr::video::ITexture *texture1 = m_pSmgr->getVideoDriver()->getTexture("texture/directionarrow.png");
	irr::video::ITexture *texture2 = m_pSmgr->getVideoDriver()->getTexture("texture/directionarrow.png");
	m_pMainNode->getMaterial(0).setTexture(0, texture1);
	m_pMainNode->getMaterial(1).setTexture(0, texture2);
	m_pMainNode->setVisible(false);

	// �ִϸ����� �߰�
	m_pDirectionAnimator = new CDirectionAnimator(pObj, endPosition, farValueFromBall, farValueFromBackground);
	m_pMainNode->addAnimator(m_pDirectionAnimator);
	m_pDirectionAnimator->drop();

	return true;
}

void CDirectionObject::Signal(irr::core::stringc strSignal,void *pParam)
{
	if(strSignal == "visible on")
	{
		m_pMainNode->setVisible(true);
	}

	else if(strSignal == "visible off")
	{
		m_pMainNode->setVisible(false);
	}
}

void CDirectionObject::Update(irr::f32 fTick)
{
	
}
