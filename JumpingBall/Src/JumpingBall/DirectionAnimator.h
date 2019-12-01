#ifndef DIRECTION_ANIMATOR_H
#define DIRECTION_ANIMATOR_H

#include "irrlicht.h"
#include "BallObject.h"

class CDirectionAnimator : public irr::scene::ISceneNodeAnimator
{
	// �� ��ü
	CBallObject* m_pBallObj;

	// ��������
	irr::core::vector3df m_vEndPosition;

	// ������ ���� ȭ��ǥ��尡 ��ŭ ������ ���ΰ��� �Ÿ�
	irr::f32 m_fFarValueFromBall;

	// ȭ��ǥ��尡 ȭ�鿡 ǥ�õǱ� ���ؼ��� ���� ���������� �ʵǹǷ� ��ġ�� y�� ���� 0�� �ƴ� 20�̻��̾�� �Ѵ�.
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

	// �����Լ� ����
	virtual void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs);
	virtual ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager) { return 0; }
};

#endif