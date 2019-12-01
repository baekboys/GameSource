#pragma once

#include "NonCharacterObject.h"
#include "FSMObject.h"

class IWeaponObject : public INonCharacterObject
{
protected :
	// ������ Ÿ��
	irr::u32 m_weaponType;

	// �Ѿ��� �ִ밹��
	irr::u32 m_maxShellCount;

	// ������ źâ 1���� ����
	irr::u32 m_maxReloadCount;

	// �Ѿ��� ���� źâ�� ������ ����
	irr::u32 m_hasShellcount;

	// �Ѿ��� ���� źâ�� ����ִ� ����
	irr::u32 m_hasReloadCount;

	// ������ ������
	irr::f32 m_minDamage;
	irr::f32 m_maxDamage;

	// ������ ������ �ٶ� ����
	irr::f32 m_impulseToEnemy;

	// ���ΰ����� �� �ݵ� ����
	irr::f32 m_impulseToHero;

	// ������ �����
	irr::f32 m_coolTime;

	// ������ ����Ÿ�̹� ��
	irr::f32 m_coolTimeTick;

	// ������ ��Ȯ��
	irr::f32 m_aimAccuracy;

	// ���ΰ��� �����ϰ� �ִ°�?
	bool m_possessionHero;

	// ���Ⱑ ���忡 ������ �ų��� ���ΰ��� ���������� hud���
	irr::scene::ISceneNode* m_pBodyNode;
	irr::scene::ISceneNode* m_pWorldBodyNode;

	// ��Ʈ���
	irr::scene::ISceneNode* m_pRootNode;


	// FPS�ִϸ�����
	irr::scene::CBulletFpsCamAnimator *m_pFPSAnim;

public:

	// ����Ÿ�� ������
	enum WEAPON_TYPE { TYPE_MELEE, TYPE_PISTOL, TYPE_RIFLE, TYPE_BOME };

	// ���ݴ���� Ÿ�� ������
	enum ATTACK_TYPE { ATTACK_CHAR, ATTACK_NONCHAR, ATTACK_BACKGROUND, ATTACK_UNKOWN };

	// �����ڿ� �Ҹ���
	IWeaponObject(void)
	: m_weaponType (0), m_maxShellCount (0), m_maxReloadCount (36), m_hasShellcount(200) ,m_hasReloadCount (36),
	m_impulseToEnemy (1000.f), m_minDamage (30.f), m_maxDamage(40.f), m_coolTime (0.2f), m_coolTimeTick (0), m_aimAccuracy (70.f), m_impulseToHero(0),
	m_possessionHero (false), m_pBodyNode(NULL), m_pWorldBodyNode(NULL), m_pFPSAnim(NULL)
	{
	}
	virtual ~IWeaponObject(void) {}

	//--------------
	// ���� �����Լ�
	//--------------

	// ������Ʈ �Լ�
	virtual void Update(irr::f32 fDelta) = 0;
	
	// �Ѿ��� �������ΰ�?
	virtual bool isLimitedShell() = 0;
	
	// �ܻ����ΰ�?
	virtual bool isZoom() = 0;
	
	// die�����ΰ�?
	virtual bool isDie() = 0;
	
	// Ư������� ����Ҽ� �ִ°�?
	virtual bool canSpecialSkill() = 0;
	
	// �����Ҽ� �ִ°�?
	virtual bool canAttack() = 0;
	
	// ���⸦ ������ �ִ°�?
	virtual bool canPutaway() = 0;
	
	// ������ �Ҽ� �ִ°�?
	virtual bool canReload() = 0;

	// Ư�� ����� ������ �ִ°�?
	virtual irr::u32 hasSpecialSkill() = 0;

	// ���� ���� ����
	virtual void selectThisWeapon() = 0;
	
	// ���� ���� ��������
	virtual void deselectThisWeapon() = 0;
	
	// ���� ���� ������
	virtual void reloadThisWeapon() = 0;
	
	// ���� ���� ������
	virtual void putawayThisWeapon(irr::scene::ISceneNode* pNode, irr::core::vector3df pos) = 0;

	// ���⸦ ������·� �����
	virtual void move() = 0;
	
	// ���⸦ �޸�����·� �����
	virtual void run() = 0;
	
	// ���⸦ ���ִ»��·� �����
	virtual void stand() = 0;
	
	// �����ϱ� (��ŷ�׽�Ʈ ����� �Ķ���ͷ�)
	virtual void attack(std::vector<PickingResultDATA> &pickingResult) = 0;

	// Ư����� ����ϱ�
	virtual irr::u32 specialSkill() = 0;
	

	//--------------
	// �Լ������κ� 
	//--------------

	// �ʱ�ȭ �Լ�
	virtual bool Init(irr::scene::ISceneNode *pNode, irr::core::vector3df startPosition);

	// �ñ׳� �Լ�
	virtual void Signal(std::string strSignal,void *pParam);	

	// ������ �Լ�
	virtual void reload();
	
	// ���� ������
	inline virtual void takeThisWeapon()
	{
		m_possessionHero = true;
		m_pAnimator->RemoveFromWorld();
		m_pFNode->removeAnimators();
		m_pAnimator = NULL;
		selectThisWeapon();
	}
	
	// ���� �Ž� ���̱�
	inline void weaponVisible(bool world, bool body)
	{
		m_pWorldBodyNode->setVisible(world);
		m_pBodyNode->setVisible(body);
	}

	// ��Ʈ��忡 �Ž���� ���̱�
	inline void addWeaponNodeToRootNode(irr::scene::ISceneNode* pNode)
	{
		m_pRootNode->addChild(pNode);
	}

	// ���ΰ��� �ִϸ����� ���̱�
	inline virtual void setFPSAnimator(irr::scene::CBulletFpsCamAnimator* pFPSAnim)
	{
		m_pFPSAnim = pFPSAnim;
	}

	// ����Ÿ�Ծ�����
	inline irr::s16 getWeaponType()
	{
		return m_weaponType;
	}

	// ������ ���� ������
	inline irr::f32 getMass()
	{
		return m_mass;
	}

	// źâ�� ������ �Ѿ˰��� ������
	inline irr::s16 getHasShellCount()
	{
		return m_hasShellcount;
	}

	// ������ źâ�� �Ѿ� ���� ������
	inline irr::s16 getHasReloadShellCount()
	{
		return m_hasReloadCount;
	}
};