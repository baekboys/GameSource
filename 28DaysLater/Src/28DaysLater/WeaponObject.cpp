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
	// �������� �Ѿ��� �ƴϸ�
	if ( isLimitedShell() )
	{
		// �������ؾ��� �Ѿ��� ���� ���ϱ�
		irr::u32 value = m_maxReloadCount - m_hasReloadCount;

		// źâ�� ������ �ؾ��� �Ѿ��� ������ ���� �����ϰ� �ִ� �Ѿ˺��� ���ٸ�
		if ( value > m_hasShellcount )
		{
			// ���� ������ �Ѿ��� źâ�� ���ϰ�
			m_hasReloadCount += m_hasShellcount;
			// ���� ���������� �����Ƿ�
			m_hasShellcount = 0;
		}
		// ���� �����ϰ� �ִ� �Ѿ��� ������ ���� �ִٸ�
		else
		{
			m_hasReloadCount = m_maxReloadCount;
			m_hasShellcount -= value;
		}
	}
	// �������� �Ѿ��̸�
	else
	{
		m_hasReloadCount = m_maxReloadCount;
	}
}


void IWeaponObject::Signal(std::string strSignal,void *pParam)
{

}