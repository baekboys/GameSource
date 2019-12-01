#ifndef DIRECTION_ANIMATOR_H
#define DIRECTION_ANIMATOR_H

#include "irrlicht.h"
#include "BallObject.h"

class CDirectionAnimator : public irr::scene::ISceneNodeAnimator
{
	// 공 객체
	CBallObject* m_pBallObj;

	// 종료지점
	irr::core::vector3df m_vEndPosition;

	// 공으로 부터 화살표노드가 얼만큼 떨어질 것인가의 거리
	irr::f32 m_fFarValueFromBall;

	// 화살표노드가 화면에 표시되기 위해서는 블럭에 가려져서는 않되므로 위치의 y축 값이 0이 아닌 20이상이어야 한다.
	irr::f32 m_fFarValueFromBackgound;
	
public:
	CDirectionAnimator(void);
	CDirectionAnimator(CBallObject* pBallObj, irr::core::vector3df endPosition, irr::f32 farValueFromBall, irr::f32 farValueFromBackground);
	~CDirectionAnimator(void);

	inline void setBallObject(CBallObject* pBallObj)
	{
		m_pBallObj = pBallObj;
	}

	inline void setEndPosition(irr::core::vector3df endPosition)
	{
		m_vEndPosition = endPosition;
	}

	inline void setFarValuefromBall(irr::f32 farValueFromBall)
	{
		m_fFarValueFromBall = farValueFromBall;
	}

	inline void setFarValuefromBackground(irr::f32 farValueFromBackground)
	{
		m_fFarValueFromBackgound = farValueFromBackground;
	}

	// 가상함수 구현
	virtual void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs);
	virtual ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager) { return 0; }
};

#endif