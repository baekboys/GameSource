#include "WeaponObject.h"

bool IWeaponObject::Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
{
	if(pNode->getType() != irr::scene::jz3d::CFormatedAnimationNode::TypeID)
		return false;

	m_pFNode = (irr::scene::jz3d::CFormatedAnimationNode *) pNode;

	m_pFNode->setVisible(false);

	m_startPosition = startPosition;

	return true;
}

void IWeaponObject::reload()
{	
	// 무한정의 총알이 아니면
	if ( isLimitedShell() )
	{
		// 재장전해야할 총알의 갯수 구하기
		irr::u32 value = m_maxReloadCount - m_hasReloadCount;

		// 탄창에 재장전 해야할 총알의 갯수가 현재 보유하고 있는 총알보다 많다면
		if ( value > m_hasShellcount )
		{
			// 현재 보유한 총알을 탄창에 더하고
			m_hasReloadCount += m_hasShellcount;
			// 현재 보유갯수는 없으므로
			m_hasShellcount = 0;
		}
		// 현재 보유하고 있는 총알의 갯수가 여유 있다면
		else
		{
			m_hasReloadCount = m_maxReloadCount;
			m_hasShellcount -= value;
		}
	}
	// 무한정의 총알이면
	else
	{
		m_hasReloadCount = m_maxReloadCount;
	}
}


void IWeaponObject::Signal(std::string strSignal,void *pParam)
{

}