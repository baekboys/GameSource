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
	// 포탈노드 포인터 저장
	m_pMainPortalNode = pNode;

	// 입구-출구 포탈 포인터 저장 실시
	{
		// 자식 리스트 가져오기
		const irr::core::list<irr::scene::ISceneNode *> children = pNode->getChildren();

		// 자식이 2개가 아니면 초기화 실패.
		if (children.getSize() != 2)
			return false;

		// 반복자 가져오기
		irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = children.begin();

		m_pInPortalNode = *it;
		m_pOutPortalNode = *(++it);
	}


	// 아이템 객체상태 설정
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
			// 충돌체크 하기
			if( m_pInPortalNode->getAbsolutePosition().getDistanceFrom(m_pBallObj->getPosition()) < 10.f )
			{
				// 시그널 데이터 만들기
				SSignalData data;

				// 포탈의 타입. 타입 없음.
				data.type = 0;

				// 포탈의 데이터값은 출구포탈위치를 알려줌. 즉 벡터값(irr::f32값 3개)
				irr::core::vector3df pos = m_pOutPortalNode->getAbsolutePosition();

				// X값
				data.fValue.push_back(pos.X);
				// Y값
				data.fValue.push_back(pos.Y);
				// Z값
				data.fValue.push_back(pos.Z);

				// 공에게 시그널 보내기
				m_pBallObj->Signal("portal", &pos);
			}
		}
		break;
	}
}