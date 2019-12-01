#pragma once

#include "NonCharacterObject.h"

class CShellObject : public INonCharacterObject
{
	//////////////////
	//	총알의 특성	//
	//////////////////

	// 총알의 데미지
	irr::f32 m_damage;

	// 이동속도
	irr::f32 m_force;



	//////////////////////////////////////
	//	총알의 상태를 나타내는 열거자	//
	//////////////////////////////////////
	enum STATUS { SHELL_READY, SHELL_MOVE, SHELL_HIT, SHELL_DIE };

public:
	// 생성자와 소멸자
	CShellObject(void);
	~CShellObject(void);

	/////////////////////////////////////////////////////////
	//	INonCharacterObject클래스의 가상함수를 구현하는 부분
	//	Signal(), Update(), isDie()를 구현
	//	Init()을 ShellObject에 맞게 구현
	/////////////////////////////////////////////////////////

	virtual void Signal(std::string strSignal,void *pParam);	
	virtual void Update(irr::f32 fDelta);
	virtual bool isDie();
	virtual bool Init(void *pParam);
};
