#pragma once

#include "CharacterObject.h"
#include "HeroObject.h"

extern int random(int, int);

//////////////////////////////////
//								//
// 좀비 클래스					//
// ICharacterObject를 상속받음.	//
//								//
//////////////////////////////////
class CZombieObject : public ICharacterObject
{
	//-------------------------------
	// 좀비의 상태를 나타내는 열거자
	//-------------------------------
	enum STATUS { ZOMBIE_READY, ZOMBIE_STAND, ZOMBIE_RANDOM_MOVE, ZOMBIE_TARGET_MOVE, ZOMBIE_ATTACK, ZOMBIE_DAMAGE, ZOMBIE_DIE, ZOMBIE_REMOVE, ZOMBIE_HEAD };

	//-----------------
	// 좀비 애니메이터
	//-----------------
	irr::scene::CBulletChracterAnimator *m_pAnimator;

	//------------------
	// 좀비의 목표 방향
	//------------------
	irr::core::vector3df m_vTargetDir;

	//----------------
	// 누적 시간계산용
	//----------------
	irr::f32 m_tick;

	CHeroObject* m_heroObj;


	// 좀비의 이동움직임 플래그 ( 다리에 공격을 받은경우 애니메이션을 다르기 취하기 위하여)
	bool m_isAttackedLeg;

	// 좀비의 공격력
	irr::f32 m_minDamage;
	irr::f32 m_maxDamage;

	// 좀비 이동 사운드
	irrklang::ISound* m_moveSound;

	irr::f32 m_speed;

	// 좀비의 공격시 물리적인 힘
	irr::f32 m_impulseToHero;

	// 좀비의 스탠드 타임
	irr::f32 m_minStandTime;
	irr::f32 m_maxStandTime;

	// 좀비의 무브 타임
	irr::f32 m_minMoveTime;
	irr::f32 m_maxMoveTime;

	irr::f32 m_randomTimeValue;

	// 좀비와 주인공 사이의 인식 가능한 거리
	irr::f32 m_viewLength;

	// 좀비와 주인공 사이의 공격 가능한 거리
	irr::f32 m_attackLength;

	// 좀비의 공격타입
	irr::u32 m_attackType;

	// 히트된 각도
	irr::f32 m_hitAngle;

	// 방향 신노드
	irr::scene::ISceneNode *m_pTargetNode;

	// 공격을 위한 collusionMngNode
	irr::scene::jz3d::CCollusionMngNode *m_pAttackColNode[2];

	//-------------------
	// 좀비의 사운드 소스
	//-------------------
	irrklang::ISoundSource* m_pSSrcAttack[7];
	irrklang::ISoundSource* m_pSSrcStand[4];
	irrklang::ISoundSource* m_pSSrcDie[5];
	irrklang::ISoundSource* m_pSSrcHurt;
	irrklang::ISoundSource* m_pSSrcMove[2];
	irrklang::ISoundSource* m_pSSrcHead[3];

public:
	CZombieObject(void);
	virtual ~CZombieObject(void);

	///////////////////////////////////////////////////////
	//	ICharacterObject클래스의 가상함수를 구현하는 부분
	//	Signal(), Update(), isDie(), isHero()를 구현
	//	Init()을 ZombieObject에 맞게 구현
	///////////////////////////////////////////////////////

	virtual void Signal(std::string strSignal,void *pParam);
	virtual void Update(irr::f32 fDelta);

	// 강체 얻어오기
	inline virtual btRigidBody* getRigidBody() 
	{
		return m_pAnimator->getRigidBody();
	}

	// 좀비가 죽었는지 판단
	inline virtual bool isDie()
	{
		if(GetStatus() == ZOMBIE_REMOVE)
			return true;
		return false;
	}

	// 주인공이 아니므로 무조건 false값 반환
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

	// 초기화 코드
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);

	irrklang::ISound* playEffectSound(STATUS type, irr::u32 number);
};
