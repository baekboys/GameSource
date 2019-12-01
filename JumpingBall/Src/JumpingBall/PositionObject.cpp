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
	// 포탈노드 포인터 저장
	m_pMainNode = pNode;

	// 아이템 객체상태 설정
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

			// 충돌체크 하기
			if( m_pMainNode->getAbsolutePosition().getDistanceFrom(m_pBallObj->getPosition()) < 10.f )
			{
				// 시그널 데이터 만들기
				SSignalData data;

				// 클리어시간 데이터
				data.fValue.push_back(m_fClearTime);

				// 공에게 시그널 보내기
				m_pBallObj->Signal("clear", &data);

				SetStatus(CLEAR);
			}
		}
		break;
	}
}