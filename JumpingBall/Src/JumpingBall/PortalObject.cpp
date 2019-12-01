#include "PortalObject.h"

#include "AppDataStructure.h"

CPortalObject::CPortalObject(void)
	: m_pMainPortalNode(NULL), m_pInPortalNode(NULL), m_pOutPortalNode(NULL),
	m_fActiveDistanceFromBall(10.f)
{
}


CPortalObject::~CPortalObject(void)
{
}

bool CPortalObject::Init(irr::scene::ISceneNode* pNode, void* pParam)
{
	// ��Ż��� ������ ����
	m_pMainPortalNode = pNode;

	// �Ա�-�ⱸ ��Ż ������ ���� �ǽ�
	{
		// �ڽ� ����Ʈ ��������
		const irr::core::list<irr::scene::ISceneNode *> children = pNode->getChildren();

		// �ڽ��� 2���� �ƴϸ� �ʱ�ȭ ����.
		if (children.getSize() != 2)
			return false;

		// �ݺ��� ��������
		irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = children.begin();

		m_pInPortalNode = *it;
		m_pOutPortalNode = *(++it);
	}


	// ������ ��ü���� ����
	SetStatus(READY);

	return true;
}

void CPortalObject::Signal(irr::core::stringc strSignal,void *pParam)
{

}

void CPortalObject::Update(irr::f32 fTick)
{
	switch(GetStatus())
	{
	case READY :
		{
			SetStatus(ACTIVE);
		}
		break;

	case ACTIVE :
		{
			// �浹üũ �ϱ�
			if( m_pInPortalNode->getAbsolutePosition().getDistanceFrom(m_pBallObj->getPosition()) < 10.f )
			{
				// �ñ׳� ������ �����
				SSignalData data;

				// ��Ż�� Ÿ��. Ÿ�� ����.
				data.type = 0;

				// ��Ż�� �����Ͱ��� �ⱸ��Ż��ġ�� �˷���. �� ���Ͱ�(irr::f32�� 3��)
				irr::core::vector3df pos = m_pOutPortalNode->getAbsolutePosition();

				// X��
				data.fValue.push_back(pos.X);
				// Y��
				data.fValue.push_back(pos.Y);
				// Z��
				data.fValue.push_back(pos.Z);

				// ������ �ñ׳� ������
				m_pBallObj->Signal("portal", &pos);
			}
		}
		break;
	}
}