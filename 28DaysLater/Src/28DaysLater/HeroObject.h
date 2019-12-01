#pragma once

#include "CharacterObject.h"
#include "WeaponObject.h"

//////////////////////////////////
//								//
// 주인공 클래스				//
// ICharacterObject를 상속받음.	//
//								//
//////////////////////////////////

class CHeroObject : public ICharacterObject, public irr::IEventReceiver
{
	//--------------------------------
	// 주인공의 상태를 나타내는 열거자
	//--------------------------------
	enum STATUS{ HERO_READY, HERO_STAND, HERO_REST, HERO_MOVE, HERO_RUN, HERO_ZOOM, HERO_DAMAGE, HERO_DIE };

	enum SOUND { DAMAGE, BREATH, LEAP };

	//------------------------------
	// 무기의 종류를 나타내는 열거자
	//------------------------------
	enum WEAPON{ KNIFE, PISTOL, RIFLE };

	//--------
	// 키코드
	//--------
	bool m_Key[irr::KEY_KEY_CODES_COUNT];

	//----------------------
	// FPS카메라 애니메이터
	//----------------------
	irr::scene::CBulletFpsCamAnimator *m_pAnimator;

	// 무기의 위치가 지정된 노드
	irr::scene::ISceneNode* m_pWeaponPositionNode;

	struct WeaponInventory
	{
		SP_IFSMObject sp_pObj;
		IWeaponObject* pObj;
		irr::scene::ISceneNode* pMeshNode;
	} m_weaponInventory[4];

	// 현재 선택되어 있는 무기
	irr::u32 m_currentWeaponNumber;

	void pickingRayTest(std::vector<PickingResultDATA> &datas);
	
	void putawayWeapon();

	//------------
	// 사운드소스
	//------------
	irrklang::ISoundSource* m_pSSrcFootstep;
	irrklang::ISoundSource* m_pSSrcHurt;
	irrklang::ISoundSource* m_pSSrcbreath;
	irrklang::ISoundSource* m_pSSrcLeap;

	irrklang::ISound* m_pSFootStep;

	void playEfftectSount(SOUND type);

public:

	void addWeapon(IWeaponObject* obj);
	void FPSMode(); 

	//-----------------
	// 생성자 및 소멸자
	//-----------------
	CHeroObject(void);
	virtual ~CHeroObject(void);


	///////////////////////////////////////////////////////
	//	ICharacterObject클래스의 가상함수를 구현하는 부분
	//	Signal(), Update(), isDie(), isHero()를 구현
	//	Init()을 HeroObject에 맞게 구현
	///////////////////////////////////////////////////////

	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);

	// 강체 얻어오기
	inline virtual btRigidBody* getRigidBody() 
	{
		return m_pAnimator->getRigidBody();
	}
	
	// 주인공이 죽었는지 판단
	inline virtual bool isDie()
	{
		if(GetStatus() == HERO_DIE)
			return true;
		return false;
	}

	// 주인공이므로 무조건 true값 반환
	inline virtual bool isHero()
	{
		return true;
	}

	// 주인공 위치 변경
	inline void setPosition(irr::core::vector3df pos)
	{
		m_pAnimator->setPosition(pos);
	}

	// 초기화 코드
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);

	///////////////////////////////////////////
	//	irr::IEventReceiver 를 상속하였으므로
	//	onEvent() 가상함수 구현
	///////////////////////////////////////////

	virtual bool OnEvent(const irr::SEvent& event);	

	// 주인공의 스테미너 잔여 퍼센트 얻어오기
	inline irr::u16 getStanimaPercent()
	{
		return m_staminaPercent;
	}

	// 주인공의 방향벡터 구하기
	irr::core::vector3df getDirectionVector();
};