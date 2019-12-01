#include "PositionObject.h"
#include "AppDataStructure.h"

CPositionObject::CPositionObject(void) 
	: m_pMainNode(NULL), m_fActiveDistanceFromBall(10.f), m_fClearTime(0)
{
}


CPositionObject::~CPositionObject(void)
{
}

bool CPositionObject::Init(irr::scene::ISceneNode* pNode, void* pParam)
{
	// ��Ż��� ������ ����
	m_pMainNode = pNode;

	// ������ ��ü���� ����
	SetStatus(READY);

	return true;
}

void CPositionObject::Signal(irr::core::stringc strSignal,void *pParam)
{

}

void CPositionObject::Update(irr::f32 fTick)
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
			m_fClearTime += fTick;

			// �浹üũ �ϱ�
			if( m_pMainNode->getAbsolutePosition().getDistanceFrom(m_pBallObj->getPosition()) < 10.f )
			{
				// �ñ׳� ������ �����
				SSignalData data;

				// Ŭ����ð� ������
				data.fValue.push_back(m_fClearTime);

				// ������ �ñ׳� ������
				m_pBallObj->Signal("clear", &data);

				SetStatus(CLEAR);
			}
		}
		break;
	}
}