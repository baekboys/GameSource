#pragma once

#include "CharacterObject.h"
#include "HeroObject.h"

extern int random(int, int);

//////////////////////////////////
//								//
// ���� Ŭ����					//
// ICharacterObject�� ��ӹ���.	//
//								//
//////////////////////////////////
class CHL2ZombieObject : public ICharacterObject
{
	//-------------------------------
	// ������ ���¸� ��Ÿ���� ������
	//-------------------------------
	enum STATUS { ZOMBIE_READY, ZOMBIE_STAND, ZOMBIE_RANDOM_MOVE, ZOMBIE_TARGET_MOVE, ZOMBIE_ATTACK, ZOMBIE_DAMAGE, ZOMBIE_DIE, ZOMBIE_REMOVE };

	//-----------------
	// ���� �ִϸ�����
	//-----------------
	irr::scene::CBulletChracterAnimator *m_pAnimator;

	//------------------
	// ������ ��ǥ ����
	//------------------
	irr::core::vector3df m_vTargetDir;

	//----------------
	// ���� �ð�����
	//----------------
	irr::f32 m_tick;

	CHeroObject* m_heroObj;

	// ������ ���ݷ�
	irr::f32 m_minDamage;
	irr::f32 m_maxDamage;

	// ������ ���ݽ� �������� ��
	irr::f32 m_impulseToHero;

	// ������ ���ĵ� Ÿ��
	irr::f32 m_minStandTime;
	irr::f32 m_maxStandTime;

	// ������ ���� Ÿ��
	irr::f32 m_minMoveTime;
	irr::f32 m_maxMoveTime;
	irr::f32 m_randomTimeValue;

	// ������ �̵������� �÷��� ( �ٸ��� ������ ������� �ִϸ��̼��� �ٸ��� ���ϱ� ���Ͽ�)
	bool m_isAttackedLeg;

	// ����� ���ΰ� ������ �ν� ������ �Ÿ�
	irr::f32 m_viewLength;

	// ����� ���ΰ� ������ ���� ������ �Ÿ�
	irr::f32 m_attackLength;

	// ������ ����Ÿ��
	irr::u32 m_attackType;

	// ��Ʈ�� ����
	irr::f32 m_hitAngle;

	// ���� �ų��
	irr::scene::ISceneNode *m_pTargetNode;

	// ������ ���� collusionMngNode
	irr::scene::jz3d::CCollusionMngNode *m_pAttackColNode[2];

	//-------------------
	// ������ ���� �ҽ�
	//-------------------
	irrklang::ISoundSource* m_pSSrcAttack[7];
	irrklang::ISoundSource* m_pSSrcStand[4];
	irrklang::ISoundSource* m_pSSrcDie[5];
	irrklang::ISoundSource* m_pSSrcHurt;
	irrklang::ISoundSource* m_pSSrcMove[2];

public:
	CHL2ZombieObject(void);
	virtual ~CHL2ZombieObject(void);

	///////////////////////////////////////////////////////
	//	ICharacterObjectŬ������ �����Լ��� �����ϴ� �κ�
	//	Signal(), Update(), isDie(), isHero()�� ����
	//	Init()�� ZombieObject�� �°� ����
	///////////////////////////////////////////////////////

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fDelta);

	// ��ü ������
	inline virtual btRigidBody* getRigidBody() 
	{
		return m_pAnimator->getRigidBody();
	}

	// ���� �׾����� �Ǵ�
	inline virtual bool isDie()
	{
		if(GetStatus() == ZOMBIE_REMOVE)
			return true;
		return false;
	}

	// ���ΰ��� �ƴϹǷ� ������ false�� ��ȯ
	inline virtual bool isHero()
	{
		return false;
	}

	irr::f32 calculateLengthToHero()
	{
		return getPosition().getDistanceFrom(m_heroObj->getPosition());
	}

	irr::core::vector3df calculateDirectionToHero()
	{
		return (m_heroObj->getPosition() - getPosition()).normalize();
	}

	// �ʱ�ȭ �ڵ�
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);

	void playEffectSound(STATUS type, irr::u32 number);
};
