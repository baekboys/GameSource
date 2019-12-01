#include "ItemObject.h"

#include "ItemNode.h"
#include "EmptyBBoxNode.h"
#include "AppDataStructure.h"

CItemObject::CItemObject(void)
	: m_pMainBoxNode(NULL), m_pItemSceneNode(NULL),
	m_sItemType(0), m_fItemValue1(0), m_fDurationTime(0), m_fActiveDistanceFromBall(10.f)
{
}


CItemObject::~CItemObject(void)
{
}

bool CItemObject::Init(irr::scene::ISceneNode* pNode, void* pParam)
{
	// 박스 신노드 포인터 저장
	m_pMainBoxNode = pNode;

	// 아이템 신노드 포인터 저장
	m_pItemSceneNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode *>( pNode )->getFirstChild();

	// 아이템 신노드의 데이터 접근
	irr::scene::jz3d::CItemNode *pItemNode = static_cast<irr::scene::jz3d::CItemNode *>( m_pItemSceneNode );
	m_sItemType =  pItemNode->m_sItemType;
	//m_cItemTypeName = pItemNode->m_itemType;
	m_fItemValue1 = pItemNode->m_fValue1;
	m_fDurationTime = pItemNode->m_fDurationTime;

	// 아이템 객체상태 설정
	SetStatus(READY);

	return true;
}

void CItemObject::Signal(irr::core::stringc strSignal,void *pParam)
{

}

void CItemObject::Update(irr::f32 fTick)
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
			if(m_pMainBoxNode->isVisible())
			{
				// 바운딩박스로 충돌처리 하기
				irr::core::aabbox3df box1, box2;
				
				// 아이템노드의 바운딩박스 얻기
				box1 = m_pItemSceneNode->getBoundingBox();
				// 바운딩박스에 아이템노드의 변환 적용
				m_pItemSceneNode->getAbsoluteTransformation().transformBox(box1);
				
				// 공노드의 바운딩박스 얻기
				box2 = m_pBallObj->getSceneNode()->getBoundingBox();
				// 바운딩박스에 공노드의 변환 적용
				m_pBallObj->getSceneNode()->getAbsoluteTransformation().transformBox(box2);
				
				// 충돌체크 하기
				if(box1.intersectsWithBox(box2))
				{
					// 시그널 데이터 만들기
					SSignalData data;
					
					// 아이템의 타입
					data.type = m_sItemType;
					
					// 아이템의 효과값
					data.fValue.push_back(m_fItemValue1);
					
					// 아이템의 지속시간
					data.fValue.push_back(m_fDurationTime);
					
					// 공에게 시그널 보내기
					m_pBallObj->Signal("item", &data);
					
					m_pMainBoxNode->setVisible(false);
				}
			}
		}
		break;

	case DIE :
		{
			
		}
		break;
	}
}