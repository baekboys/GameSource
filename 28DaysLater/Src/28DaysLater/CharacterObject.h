#pragma once

#include "FSMObject.h"
#include "BtEngine.h"
#include "UtilHeader.h"
#include "FactoryHeader.h"
#include "CollisionManager.h"

//////////////////////////////////////////
//										//
// ĳ���� Ŭ����						//
// ��� ĳ���ʹ� �� Ŭ������ ��ӹ޴´�.//
//										//
//////////////////////////////////////////

class ICharacterObject : public IFSMObject
{
protected :
	
	//////////////////////
	//	ĳ������ Ư��	//
	//////////////////////

	// ü��
	irr::f32 m_hp;
	
	// ��ü��
	irr::f32 m_maxHp;
	
	// �̵��ӵ�
	irr::f32 m_MoveVelocity;
	
	// �޸���ӵ�
	irr::f32 m_RunVelocity;

	// �޸��� ���ӽð�
	irr::f32 m_runDurationTime;

	// �޸��� ȸ�� �ӵ� (���ĵ����)
	irr::f32 m_runRecovery_stand;

	// �޸��� ȸ�� �ӵ� (�������)
	irr::f32 m_runRecovery_move;

	// ���¹̳�(�޸���) �ۼ�Ʈ
	irr::u16 m_staminaPercent;

	// �޸��� ���ӽð� ����� ���� ƽ��
	irr::f32 m_runDurationTimeTick;

	// ����
	irr::f32 m_mass;

	// �������
	irr::f32 m_friction;

	// ���ΰ����� �Ǵ�
	bool m_hero;

	//////////////////
	//	��		��	//
	//////////////////
	
	// ĳ������ ���
	irr::scene::jz3d::CFormatedAnimationNode *m_pFNode;
	
	// ĳ���Ͱ� �������� ��ġ������ �ִ� ��ġ����
	irr::core::vector3df m_startPosition;

	//--------------------------
	// �̵��� ���� �����浹���
	//--------------------------
	irr::scene::jz3d::CCollusionMngNode *m_pCollMngNode;

	//-------------------------------------
	// rigidbody, �� ��ü�� ������� ������
	//-------------------------------------
	btCollisionObject *m_pCloBody;

	//----------------------
	// �浹ó�� ���� �Ŵ���
	//----------------------
	CCollisionManager m_colManager;

	//------------
	// ī�޶� ���
	//------------
	irr::scene::ICameraSceneNode* m_pCamNode;
	irr::f32 m_farValue;

	//-----------
	// ���忣��
	//-----------
	irrklang::ISoundEngine* m_SoundEngine;

public:
	//////////////////////
	//	������ �� �Ҹ���
	//////////////////////
	ICharacterObject()
		: IFSMObject(), 
			m_hp (0.f), m_maxHp (0.f), m_MoveVelocity (0.f), m_RunVelocity (0.f), m_runDurationTime (0.f), 
			m_runRecovery_move (0), m_runRecovery_stand (0), m_runDurationTimeTick (0), m_staminaPercent (0),
			m_hero (false), m_pFNode (NULL), m_startPosition (0,0,0), 
			m_pCollMngNode (NULL), m_pCloBody (NULL), m_SoundEngine(NULL)
	{
	}
	virtual ~ICharacterObject() {}
	
	//////////////////////////////////////////////////
	//	ICharacterObject������ ��밡�� �� �Լ� ����
	//////////////////////////////////////////////////

	//-----------------------
	// ĳ������ ��ġ ������
	//-----------------------
	inline irr::core::vector3df getPosition()
	{
		return m_pFNode->getAbsolutePosition();
	}

	//-------------------------
	// ĳ������ �ų�� ������
	//-------------------------
	inline irr::scene::jz3d::CFormatedAnimationNode* getFormatedAnimationNode()
	{
		return m_pFNode;
	}

	//---------------------------
	// ĳ������ �浹���� ������
	//---------------------------
	inline irr::scene::jz3d::CCollusionMngNode* getCollusionMngNode()
	{
		return m_pCollMngNode;
	}

	//-----------------------------
	// ĳ������ �浹�Ŵ��� ������
	//-----------------------------
	inline CCollisionManager* getCollisionMng()
	{
		return &m_colManager;
	}

	///////////////////////////////////////////
	//	�����Լ� �� ������ ���� ���� �����Լ�
	///////////////////////////////////////////

	//-------------
	// ���������Լ�
	//-------------
	virtual btRigidBody* getRigidBody() = 0;
	virtual void Signal(std::string strSignal,void *pParam) = 0;	
	virtual void Update(irr::f32 fDelta) = 0;
	virtual bool isDie()=0;
	virtual bool isHero()=0;

	//------------
	// �ʱ�ȭ �ڵ�
	//------------
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition)
	{
		if(pNode->getType() != irr::scene::jz3d::CFormatedAnimationNode::TypeID)
			return false;

		m_pFNode = (irr::scene::jz3d::CFormatedAnimationNode *) pNode;

		m_pFNode->setVisible(false);

		m_startPosition = startPosition;

		return true;
	}
};