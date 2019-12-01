// An animator that makes one node follow another

#ifndef __C_THIRD_PERSON__ANIMATOR_H_INCLUDED__
#define __C_THIRD_PERSON__ANIMATOR_H_INCLUDED__

#include "ISceneNodeAnimator.h"
#include "ISceneNode.h"
#include "ICameraSceneNode.h"

namespace irr
{

namespace scene
{

	class CThirdPersonAnimator : public ISceneNodeAnimator
	{
	public:

		//! constructor
		CThirdPersonAnimator() : 
			Target(0),m_vBaseDir(0,0,-1)
			{}
			/*, FollowNode(0), lastTime(0), 
			Velocity(0,0,0), MaxSpeedSQ(0.3f), Thrust(0.3f), MinDist(1.0f) {}*/

		//! destructor
		virtual ~CThirdPersonAnimator() 
		{
			if (Target)
				Target->drop();

			//if (FollowNode)
			//	FollowNode->drop();
		}

		//! Animates a scene node.
		//! \param node: Node to animate.
		//! \param timeMs: Current time in milli seconds.
		virtual void animateNode(ISceneNode* node, u32 timeMs)
		{		

			bool isCamera = (node->getType() == ESNT_CAMERA);

			// get positions in world space
			core::vector3df currentPos, targetPos, followPos;

 			currentPos = node->getAbsolutePosition();

			if (isCamera && Target)
			{
				ICameraSceneNode* cam = (ICameraSceneNode*)node;
				targetPos = Target->getAbsolutePosition();
				cam->setTarget(Target->getAbsolutePosition());

				followPos = targetPos;

				irr::core::vector3df vdir = m_vBaseDir;
				vdir = m_qtRotation * vdir;

				vdir *= m_fDist;
				followPos += vdir;

				node->setPosition(followPos);
			}			

		}

		
		virtual void setTargetNode(ISceneNode* target)
		{
			if (Target)
				Target->drop();

			Target = target;

			if (Target)
				Target->grab();
		}
		
		//! 따라다는 물체 기준으로 카메라의 거리지정.
		//! \param Rotation: quaternion
		void setDistance(f32 Dist)
		{
			m_fDist = Dist;
		}

		//! 따라다는 물체 기준으로 카메라 바라보는 각도를 설정한다.
		//! \param Rotation: quaternion
		void setRotation(core::quaternion Rotation)
		{
			m_qtRotation = Rotation;
		}

		//! 기준 방향 벡터 정의
		//! \param Rotation: quaternion
		void setBaseDirection(irr::core::vector3df baseDir)
		{
			baseDir = m_vBaseDir;
		}

	private:

		ISceneNode *Target;		
		irr::core::vector3df m_vBaseDir;
		core::quaternion m_qtRotation;
		f32 m_fDist;

	};
} // end namespace scene
} // end namespace irr

#endif // __C_THIRD_PERSON_FOLLOW_ANIMATOR_H_INCLUDED__

