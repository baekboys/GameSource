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
	// �ڽ� �ų�� ������ ����
	m_pMainBoxNode = pNode;

	// ������ �ų�� ������ ����
	m_pItemSceneNode = static_cast<irr::scene::jz3d::CEmptyBBoxNode *>( pNode )->getFirstChild();

	// ������ �ų���� ������ ����
	irr::scene::jz3d::CItemNode *pItemNode = static_cast<irr::scene::jz3d::CItemNode *>( m_pItemSceneNode );
	m_sItemType =  pItemNode->m_sItemType;
	//m_cItemTypeName = pItemNode->m_itemType;
	m_fItemValue1 = pItemNode->m_fValue1;
	m_fDurationTime = pItemNode->m_fDurationTime;

	// ������ ��ü���� ����
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
				// �ٿ���ڽ��� �浹ó�� �ϱ�
				irr::core::aabbox3df box1, box2;
				
				// �����۳���� �ٿ���ڽ� ���
				box1 = m_pItemSceneNode->getBoundingBox();
				// �ٿ���ڽ��� �����۳���� ��ȯ ����
				m_pItemSceneNode->getAbsoluteTransformation().transformBox(box1);
				
				// ������� �ٿ���ڽ� ���
				box2 = m_pBallObj->getSceneNode()->getBoundingBox();
				// �ٿ���ڽ��� ������� ��ȯ ����
				m_pBallObj->getSceneNode()->getAbsoluteTransformation().transformBox(box2);
				
				// �浹üũ �ϱ�
				if(box1.intersectsWithBox(box2))
				{
					// �ñ׳� ������ �����
					SSignalData data;
					
					// �������� Ÿ��
					data.type = m_sItemType;
					
					// �������� ȿ����
					data.fValue.push_back(m_fItemValue1);
					
					// �������� ���ӽð�
					data.fValue.push_back(m_fDurationTime);
					
					// ������ �ñ׳� ������
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