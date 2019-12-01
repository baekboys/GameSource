#ifndef BALL_CAMERA_ANIMATOR_H
#define BALL_CAMERA_ANIMATOR_H

#include "irrlicht.h"
#include "BallObject.h"
#include "App.h"

class CBallCameraAnimator : public irr::scene::ISceneNodeAnimator
{
	enum STATUS 
	{
		ZOOMIN,
		ZOOMIN_JUMP,
		ZOOMOUT,
		ZOOMOUT_JUMP,
		NORMAL
	};	
	CBallObject* m_ballObj;
	
	irr::u32 m_uStatus;
	irr::f32 m_fMaxLength;
	irr::f32 m_fJumpLength;
	irr::f32 m_fMinLength;
	irr::f32 m_fDelta;
	irr::f32 m_fJumpDelta;

	irr::u32 m_uLastTick;
	
	void setStatus(STATUS status){ m_uStatus = status; }
	irr::u32 getStatus() { return m_uStatus; }
	
public:
	CBallCameraAnimator(void);
	~CBallCameraAnimator(void);

	// 가상함수 구현
	virtual void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs);

	virtual ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager);

	// 타겟 설정
	inline void setTargetBall(CBallObject* obj)
	{
		m_ballObj = obj;
	}

	inline void zoomOut(){ if(m_uStatus == NORMAL) { m_uStatus = ZOOMOUT; CApp::Get().NativePlaySound("res/sound/zoom.mp3");}}
	inline void zoomOutJump(){ if(m_uStatus == NORMAL) { m_uStatus = ZOOMOUT_JUMP;} }
};

#endif