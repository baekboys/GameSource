//#pragma once
#ifndef BULLET_FPSCAM_ANIMATOR_H
#define BULLET_FPSCAM_ANIMATOR_H

#include "ISceneNodeAnimatorCameraFPS.h"
#include "vector2d.h"
#include "SKeyMap.h"
#include "irrArray.h"

namespace irr
{

namespace gui
{
	class ICursorControl;
}

namespace scene
{

	//! Special scene node animator for FPS cameras
	class CBulletFpsCamAnimator : public CBulletObjectAnimator
	{
	public:
		//! CreateInstance
		static CBulletFpsCamAnimator* createInstance(
			ISceneManager* pSceneManager,
			ISceneNode* pSceneNode,
			 gui::ICursorControl *CursorControl,
			CBulletAnimatorManager* pBulletMgr,
			CBulletObjectAnimatorGeometry* pGeom,
			CBulletObjectAnimatorParams* pPhysicsParam
			);

		//! Constructor
		CBulletFpsCamAnimator(gui::ICursorControl* cursorControl, 
			f32 rotateSpeed = 100.0f, f32 moveSpeed = 500.0f, f32 jumpSpeed=0.f,
			SKeyMap* keyMapArray=0, u32 keyMapSize=0, bool noVerticalMovement=false);
			
		//! Destructor
		virtual ~CBulletFpsCamAnimator();

		//! Animates the scene node, currently only works on cameras
		virtual void animateNode(ISceneNode* node, u32 timeMs);

		//! Event receiver
		virtual bool OnEvent(const SEvent& event);

		//! Returns the speed of movement in units per millisecond
		virtual f32 getMoveSpeed() const;

		//! Sets the speed of movement in units per millisecond
		virtual void setMoveSpeed(f32 moveSpeed);

		//! Returns the rotation speed
		virtual f32 getRotateSpeed() const;

		//! Set the rotation speed
		virtual void setRotateSpeed(f32 rotateSpeed);

		//! Sets the keyboard mapping for this animator
		//! \param keymap: an array of keyboard mappings, see SKeyMap
		//! \param count: the size of the keyboard map array
		virtual void setKeyMap(SKeyMap *map, u32 count);

		//! Sets whether vertical movement should be allowed.
		virtual void setVerticalMovement(bool allow);

		//! This animator will receive events when attached to the active camera
		virtual bool isEventReceiverEnabled() const
		{
			return true;
		}

		//! Returns the type of this animator
		virtual ESCENE_NODE_ANIMATOR_TYPE getType() const
		{
			return ESNAT_CAMERA_FPS;
			//return (ESCENE_NODE_ANIMATOR_TYPE)0;
		}

		void addRotation(irr::f32 vertical, irr::f32 horizontal);

	private:

		btVector3 m_raySource[2];
		btVector3 m_rayTarget[2];
		btScalar m_rayLambda[2];		

		struct SCamKeyMap
		{
			SCamKeyMap() {};
			SCamKeyMap(s32 a, EKEY_CODE k) : action(a), keycode(k) {}

			s32 action;
			EKEY_CODE keycode;
		};

		void allKeysUp();

		gui::ICursorControl *CursorControl;

		f32 MaxVerticalAngle;

		f32 MoveSpeed;
		f32 RotateSpeed;
		f32 JumpSpeed;

		s32 LastAnimationTime;

		core::vector3df TargetVector;
		core::array<SCamKeyMap> KeyMap;
		core::position2d<f32> CenterCursor, CursorPos;

		bool CursorKeys[6];

		bool firstUpdate;
		bool NoVerticalMovement;
	};

} // end namespace scene
} // end namespace irr

#endif