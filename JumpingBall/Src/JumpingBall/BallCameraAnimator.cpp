#include "BallCameraAnimator.h"
#include "bulletEngine.h"

CBallCameraAnimator::CBallCameraAnimator(void)
: m_uStatus(NORMAL), 
m_fMaxLength (260), m_fMinLength (60), m_fJumpLength(150),
m_fDelta (500.f), m_fJumpDelta(200.f), m_uLastTick(0)
{

}


CBallCameraAnimator::~CBallCameraAnimator(void)
{

}

void CBallCameraAnimator::animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs)
{
	// fDelta값 얻어오기
	irr::f32 fDelta = (float)(timeMs - m_uLastTick) / 1000.f;	
	m_uLastTick = timeMs;

	irr::scene::ICameraSceneNode* pCam = static_cast<irr::scene::ICameraSceneNode*>(node);

	if(m_ballObj != NULL)
	{
		// (1) 공의 위치 얻어오기
		irr::core::vector3df pos = m_ballObj->getAnimator()->getPosition();
		irr::core::vector3df camPos = pCam->getAbsolutePosition();
		camPos.X = pos.X;
		camPos.Z = pos.Z;

		switch (getStatus())
		{
		case NORMAL:
			{
				camPos.Y = m_fMinLength;
			}
			break;

		case ZOOMIN:
			{				
				if (m_fMinLength > camPos.Y) 
				{
					camPos.Y = m_fMinLength;
					setStatus(NORMAL);
				}
				else 
				{
					camPos.Y = camPos.Y - fDelta * m_fDelta;
				}

			}
			break;

		case ZOOMIN_JUMP:
			{				
				if (m_fMinLength > camPos.Y) 
				{
					camPos.Y = m_fMinLength;
					setStatus(NORMAL);
				}
				else 
				{
					camPos.Y = camPos.Y - fDelta * m_fJumpDelta;
				}

			}
			break;

		case ZOOMOUT:
			{
				if (m_fMaxLength < camPos.Y)
				{
					camPos.Y = m_fMaxLength;
					setStatus(ZOOMIN);
				}
				else 
				{
					camPos.Y = camPos.Y + fDelta * m_fDelta;
				}
			}
			break;

		case ZOOMOUT_JUMP:
			{
				if (m_fJumpLength < camPos.Y)
				{
					camPos.Y = m_fJumpLength;
					setStatus(ZOOMIN_JUMP);
				}
				else 
				{
					camPos.Y = camPos.Y + fDelta * m_fJumpDelta;
				}
			}
			break;
		}

		pCam->setPosition(camPos);
		pCam->setTarget(pos);
	}
}

irr::scene::ISceneNodeAnimator* CBallCameraAnimator::createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager)
{
	return 0;
}