#pragma once

#include "CharacterObject.h"
#include "WeaponObject.h"

//////////////////////////////////
//								//
// ���ΰ� Ŭ����				//
// ICharacterObject�� ��ӹ���.	//
//								//
//////////////////////////////////

class CHeroObject : public ICharacterObject, public irr::IEventReceiver
{
	//--------------------------------
	// ���ΰ��� ���¸� ��Ÿ���� ������
	//--------------------------------
	enum STATUS{ HERO_READY, HERO_STAND, HERO_REST, HERO_MOVE, HERO_RUN, HERO_ZOOM, HERO_DAMAGE, HERO_DIE };

	enum SOUND { DAMAGE, BREATH, LEAP };

	//------------------------------
	// ������ ������ ��Ÿ���� ������
	//------------------------------
	enum WEAPON{ KNIFE, PISTOL, RIFLE };

	//--------
	// Ű�ڵ�
	//--------
	bool m_Key[irr::KEY_KEY_CODES_COUNT];

	//----------------------
	// FPSī�޶� �ִϸ�����
	//----------------------
	irr::scene::CBulletFpsCamAnimator *m_pAnimator;

	// ������ ��ġ�� ������ ���
	irr::scene::ISceneNode* m_pWeaponPositionNode;

	struct WeaponInventory
	{
		SP_IFSMObject sp_pObj;
		IWeaponObject* pObj;
		irr::scene::ISceneNode* pMeshNode;
	} m_weaponInventory[4];

	// ���� ���õǾ� �ִ� ����
	irr::u32 m_currentWeaponNumber;

	void pickingRayTest(std::vector<PickingResultDATA> &datas);
	
	void putawayWeapon();

	//------------
	// ����ҽ�
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
	// ������ �� �Ҹ���
	//-----------------
	CHeroObject(void);
	virtual ~CHeroObject(void);


	///////////////////////////////////////////////////////
	//	ICharacterObjectŬ������ �����Լ��� �����ϴ� �κ�
	//	Signal(), Update(), isDie(), isHero()�� ����
	//	Init()�� HeroObject�� �°� ����
	///////////////////////////////////////////////////////

	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);

	// ��ü ������
	inline virtual btRigidBody* getRigidBody() 
	{
		return m_pAnimator->getRigidBody();
	}
	
	// ���ΰ��� �׾����� �Ǵ�
	inline virtual bool isDie()
	{
		if(GetStatus() == HERO_DIE)
			return true;
		return false;
	}

	// ���ΰ��̹Ƿ� ������ true�� ��ȯ
	inline virtual bool isHero()
	{
		return true;
	}

	// ���ΰ� ��ġ ����
	inline void setPosition(irr::core::vector3df pos)
	{
		m_pAnimator->setPosition(pos);
	}

	// �ʱ�ȭ �ڵ�
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);

	///////////////////////////////////////////
	//	irr::IEventReceiver �� ����Ͽ����Ƿ�
	//	onEvent() �����Լ� ����
	///////////////////////////////////////////

	virtual bool OnEvent(const irr::SEvent& event);	

	// ���ΰ��� ���׹̳� �ܿ� �ۼ�Ʈ ������
	inline irr::u16 getStanimaPercent()
	{
		return m_staminaPercent;
	}

	// ���ΰ��� ���⺤�� ���ϱ�
	irr::core::vector3df getDirectionVector();
};