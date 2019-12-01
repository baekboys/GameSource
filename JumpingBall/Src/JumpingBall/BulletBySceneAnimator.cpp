#include "BulletBySceneAnimator.h"
#include "btBulletDynamicsCommon.h"
#include "CBulletPhysicsUtils.h"

CBulletBySceneAnimator::CBulletBySceneAnimator(btRigidBody* rigidBody) : m_pRigidbody(rigidBody)
{
}

CBulletBySceneAnimator::CBulletBySceneAnimator(void) : m_pRigidbody(NULL)
{
}

CBulletBySceneAnimator::~CBulletBySceneAnimator(void)
{
}

void CBulletBySceneAnimator::animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs)
{
	if (m_pRigidbody && m_pRigidbody->getMotionState())
	{
		irr::core::vector3df pos = node->getAbsolutePosition();
		btVector3 btPos(pos.X, pos.Y, pos.Z);

		// position
		btTransform t = m_pRigidbody->getWorldTransform();
		t.setOrigin(btPos);

		irr::core::quaternion qt(node->getAbsoluteTransformation());

		// angle
		irr::f32 angle;
		// axis
		irr::core::vector3df axis;
		// quaternion on irrlicht
		qt.toAngleAxis(angle, axis);
		// vector on bullet
		btVector3 btAxis(axis.X, -axis.Y, axis.Z);
		// quaternion on bullet
		btQuaternion rot;
		rot.setRotation(btAxis, angle);
		t.setRotation(rot);

		// sync a rigidbody transform with a scene node transform
		m_pRigidbody->setWorldTransform(t);
	}
}