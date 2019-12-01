#pragma once

#include "NonCharacterObject.h"

class CShellObject : public INonCharacterObject
{
	//////////////////
	//	�Ѿ��� Ư��	//
	//////////////////

	// �Ѿ��� ������
	irr::f32 m_damage;

	// �̵��ӵ�
	irr::f32 m_force;



	//////////////////////////////////////
	//	�Ѿ��� ���¸� ��Ÿ���� ������	//
	//////////////////////////////////////
	enum STATUS { SHELL_READY, SHELL_MOVE, SHELL_HIT, SHELL_DIE };

public:
	// �����ڿ� �Ҹ���
	CShellObject(void);
	~CShellObject(void);

	/////////////////////////////////////////////////////////
	//	INonCharacterObjectŬ������ �����Լ��� �����ϴ� �κ�
	//	Signal(), Update(), isDie()�� ����
	//	Init()�� ShellObject�� �°� ����
	/////////////////////////////////////////////////////////

	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);
	virtual bool isDie();
	virtual bool Init(void *pParam);
};
