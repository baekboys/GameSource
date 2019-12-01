#pragma once

#include "FSMObject.h"

#include "BtEngine.h"
#include "UtilHeader.h"
#include "FactoryHeader.h"




//////////////////////////////////////////////
//											//
// ��ĳ���� Ŭ����							//
// ĳ���Ͱ� �ƴ� ��� Ŭ������				//
// �Ʒ��� ��ĳ����Ŭ������ ��� �޴´�.		//
//											//
//////////////////////////////////////////////

class INonCharacterObject : public IFSMObject
{
protected :
	
	//////////////////////
	//	��ĳ������ Ư��	//
	//////////////////////

	// ����
	irr::f32 m_mass;

	// �������
	irr::f32 m_friction;



	//////////////////////
	//	��ĳ������ ���	//
	//////////////////////


	//////////////////
	//	��		��	//
	//////////////////
	
	// ��ĳ������ ���
	irr::scene::jz3d::CFormatedAnimationNode *m_pFNode;
	
	// ��ĳ���Ͱ� �������� ��ġ������ �ִ� ��ġ����
	irr::core::vector3df m_startPosition;

	// ��ĳ������ �ִϸ�����
	irr::scene::CBulletObjectAnimator *m_pAnimator;

	// �浹�������� ���
	irr::scene::jz3d::CCollusionMngNode *m_pCollMngNode;

public :
	// �����ڿ� �Ҹ���
	INonCharacterObject()
	: m_pFNode (NULL), m_pAnimator (NULL), m_pCollMngNode (NULL)
	{
	}
	virtual ~INonCharacterObject() {}

	// ���������Լ�
	virtual void Signal(std::string strSignal,void *pParam) = 0;	
	virtual void Update(irr::f32 fDelta) = 0;
	virtual bool isDie() = 0;
	
	virtual inline irr::scene::jz3d::CFormatedAnimationNode* getFormatedAnimationNode()
	{
		return m_pFNode;
	}
	
	// ��ü ������
	virtual inline  btRigidBody* getRigidBody() 
	{
		return m_pAnimator->getRigidBody();
	}

	// ����� ��ġ ������
	virtual inline irr::core::vector3df getPosition()
	{
		return m_pFNode->getAbsolutePosition();
	}

	//����� �ִϸ����� ���
	virtual inline irr::scene::CBulletObjectAnimator* getAnimator()
	{
		return m_pAnimator;
	}
};