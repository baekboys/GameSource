#pragma once

#include "NonCharacterObject.h"
#include "FSMObject.h"

class IWeaponObject : public INonCharacterObject
{
protected :
	// 무기의 타입
	irr::u32 m_weaponType;

	// 총알의 최대갯수
	irr::u32 m_maxShellCount;

	// 무기의 탄창 1개당 갯수
	irr::u32 m_maxReloadCount;

	// 총알의 현재 탄창외 보유한 갯수
	irr::u32 m_hasShellcount;

	// 총알의 현재 탄창에 들어있는 갯수
	irr::u32 m_hasReloadCount;

	// 무기의 데미지
	irr::f32 m_minDamage;
	irr::f32 m_maxDamage;

	// 무기의 데미지 줄때 힘값
	irr::f32 m_impulseToEnemy;

	// 주인공에게 줄 반동 힘값
	irr::f32 m_impulseToHero;

	// 무기의 연사력
	irr::f32 m_coolTime;

	// 무기의 연사타이밍 값
	irr::f32 m_coolTimeTick;

	// 무기의 정확도
	irr::f32 m_aimAccuracy;

	// 주인공이 소지하고 있는가?
	bool m_possessionHero;

	// 무기가 월드에 있을때 신노드와 주인공이 가지고있을 hud노드
	irr::scene::ISceneNode* m_pBodyNode;
	irr::scene::ISceneNode* m_pWorldBodyNode;

	// 루트노드
	irr::scene::ISceneNode* m_pRootNode;


	// FPS애니메이터
	irr::scene::CBulletFpsCamAnimator *m_pFPSAnim;

public:

	// 무기타입 열거자
	enum WEAPON_TYPE { TYPE_MELEE, TYPE_PISTOL, TYPE_RIFLE, TYPE_BOME };

	// 공격대상의 타입 열거자
	enum ATTACK_TYPE { ATTACK_CHAR, ATTACK_NONCHAR, ATTACK_BACKGROUND, ATTACK_UNKOWN };

	// 생성자와 소멸자
	IWeaponObject(void)
	: m_weaponType (0), m_maxShellCount (0), m_maxReloadCount (36), m_hasShellcount(200) ,m_hasReloadCount (36),
	m_impulseToEnemy (1000.f), m_minDamage (30.f), m_maxDamage(40.f), m_coolTime (0.2f), m_coolTimeTick (0), m_aimAccuracy (70.f), m_impulseToHero(0),
	m_possessionHero (false), m_pBodyNode(NULL), m_pWorldBodyNode(NULL), m_pFPSAnim(NULL)
	{
	}
	virtual ~IWeaponObject(void) {}

	//--------------
	// 순수 가상함수
	//--------------

	// 업데이트 함수
	virtual void Update(irr::f32 fDelta) = 0;
	
	// 총알이 제한적인가?
	virtual bool isLimitedShell() = 0;
	
	// 줌상태인가?
	virtual bool isZoom() = 0;
	
	// die상태인가?
	virtual bool isDie() = 0;
	
	// 특수기술을 사용할수 있는가?
	virtual bool canSpecialSkill() = 0;
	
	// 공격할수 있는가?
	virtual bool canAttack() = 0;
	
	// 무기를 버릴수 있는가?
	virtual bool canPutaway() = 0;
	
	// 재장전 할수 있는가?
	virtual bool canReload() = 0;

	// 특수 기술을 가지고 있는가?
	virtual irr::u32 hasSpecialSkill() = 0;

	// 지금 무기 선택
	virtual void selectThisWeapon() = 0;
	
	// 지금 무기 선택해제
	virtual void deselectThisWeapon() = 0;
	
	// 지금 무기 재장전
	virtual void reloadThisWeapon() = 0;
	
	// 지금 무기 버리기
	virtual void putawayThisWeapon(irr::scene::ISceneNode* pNode, irr::core::vector3df pos) = 0;

	// 무기를 무브상태로 만들기
	virtual void move() = 0;
	
	// 무기를 달리기상태로 만들기
	virtual void run() = 0;
	
	// 무기를 서있는상태로 만들기
	virtual void stand() = 0;
	
	// 공격하기 (피킹테스트 결과를 파라미터로)
	virtual void attack(std::vector<PickingResultDATA> &pickingResult) = 0;

	// 특수기술 사용하기
	virtual irr::u32 specialSkill() = 0;
	

	//--------------
	// 함수구현부분 
	//--------------

	// 초기화 함수
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);

	// 시그널 함수
	virtual void Signal(std::string strSignal,void *pParam);	

	// 재장전 함수
	virtual void reload();
	
	// 무기 집어들기
	inline virtual void takeThisWeapon()
	{
		m_possessionHero = true;
		m_pAnimator->RemoveFromWorld();
		m_pFNode->removeAnimators();
		m_pAnimator = NULL;
		selectThisWeapon();
	}
	
	// 무기 매쉬 보이기
	inline void weaponVisible(bool world, bool body)
	{
		m_pWorldBodyNode->setVisible(world);
		m_pBodyNode->setVisible(body);
	}

	// 루트노드에 매쉬노드 붙이기
	inline void addWeaponNodeToRootNode(irr::scene::ISceneNode* pNode)
	{
		m_pRootNode->addChild(pNode);
	}

	// 주인공의 애니메이터 붙이기
	inline virtual void setFPSAnimator(irr::scene::CBulletFpsCamAnimator* pFPSAnim)
	{
		m_pFPSAnim = pFPSAnim;
	}

	// 무기타입얻어오기
	inline irr::s16 getWeaponType()
	{
		return m_weaponType;
	}

	// 무기의 무게 얻어오기
	inline irr::f32 getMass()
	{
		return m_mass;
	}

	// 탄창외 보유한 총알갯수 얻어오기
	inline irr::s16 getHasShellCount()
	{
		return m_hasShellcount;
	}

	// 무기의 탄창의 총알 갯수 얻어오기
	inline irr::s16 getHasReloadShellCount()
	{
		return m_hasReloadCount;
	}
};