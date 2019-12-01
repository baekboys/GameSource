#pragma once

#include "irrlicht.h"

class btRigidBody;

class CBulletBySceneAnimator : public irr::scene::ISceneNodeAnimator
{
	btRigidBody* m_pRigidbody;

public:
	CBulletBySceneAnimator(btRigidBody* rigidBody);
	CBulletBySceneAnimator(void);
	~CBulletBySceneAnimator(void);

	void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs);

	ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager=0) { return NULL; }

	void setRigidbody(btRigidBody* rigidBody) { m_pRigidbody = rigidBody; }
};
