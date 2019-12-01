#ifndef FSM_OBJECT_H
#define FSM_OBJECT_H

#include "irrlicht.h"
#include "bulletEngine.h"

class IFSMObject
{
private:
	irr::s32 m_nStatus;
	irr::s32 m_nStep;
	
public:
	IFSMObject(void)
		:m_nStatus(0),m_nStep(0)
	{
	}

	virtual ~IFSMObject(void)
	{
	}

	inline void SetStatus(irr::s32 status)
	{
		m_nStep = 0; //Ω∫≈«√ ±‚»≠
		m_nStatus = status;
	}
	inline void SetStep(irr::s32 step)
	{
		m_nStep = step;
	}

	inline irr::s32 GetStep()
	{
		return m_nStep;
	}

	inline irr::s32 GetStatus()
	{
		return m_nStatus;
	}

	inline void IncStep()
	{
		m_nStep++;
	}
	inline void DecStep()
	{
		m_nStep--;
	}	

	virtual irr::core::vector3df getPosition() = 0;
	virtual bool isDie() = 0;
	virtual void Signal(irr::core::stringc strSignal,void *pParam) = 0;
	virtual void Update(irr::f32 fTick) = 0;	
	virtual btRigidBody* getRigidBody() = 0;
	virtual irr::scene::ISceneNode* getSceneNode() = 0;
	virtual irr::scene::CBulletChracterAnimator* getAnimator() = 0;
	virtual bool isBall() = 0;
	virtual void setTargetObject(IFSMObject* obj) = 0;
	virtual void reset() = 0;
};

#endif