#include "DirectionAnimator.h"


CDirectionAnimator::CDirectionAnimator(void)
{
}

CDirectionAnimator::CDirectionAnimator(CBallObject* pBallObj, irr::core::vector3df endPosition, irr::f32 farValueFromBall, irr::f32 farValueFromBackground)
	: m_pBallObj(pBallObj), m_vEndPosition(endPosition), m_fFarValueFromBall(farValueFromBall), m_fFarValueFromBackgound(farValueFromBackground)
{
}

CDirectionAnimator::~CDirectionAnimator(void)
{
}

void CDirectionAnimator::animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs)
{
	irr::core::vector3df ball_position = m_pBallObj->getPosition();
	irr::core::vector3df direction = m_vEndPosition - ball_position;

	direction.normalize();

	irr::core::vector3df final_postion = ball_position + (direction * m_fFarValueFromBall) + irr::core::vector3df(0, m_fFarValueFromBackgound, 0);
	// 포지션
	node->setPosition(final_postion);

	// 회전시키기
	irr::core::vector3df rotation = direction.getHorizontalAngle();
	node->setRotation( irr::core::vector3df(90, rotation.Y, 0) );

	node->updateAbsolutePosition();
}
