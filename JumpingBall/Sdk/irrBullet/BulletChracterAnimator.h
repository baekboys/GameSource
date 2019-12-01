//#pragma once
#ifndef BULLET_CHARACTER_ANIMATOR_H
#define BULLET_CHARACTER_ANIMATOR_H

namespace irr
{
namespace scene
{


class CBulletChracterAnimator :
	public CBulletObjectAnimator
	       
{
	btVector3 m_raySource[2];
	btVector3 m_rayTarget[2];
	btScalar m_rayLambda[2];
	//irr::core::vector3df m_LocalPos;
	//btVector3 m_rayNormal[2];
	
public:
	CBulletChracterAnimator(void);
	virtual ~CBulletChracterAnimator(void);

	//! CreateInstance
    static CBulletChracterAnimator* createInstance(
      ISceneManager* pSceneManager,
      ISceneNode* pSceneNode,
      CBulletAnimatorManager* pBulletMgr,
      CBulletObjectAnimatorGeometry* pGeom,
      CBulletObjectAnimatorParams* pPhysicsParam
    );
	

    virtual void animateNode(ISceneNode* node, u32 timeMs);

	bool IsOnGround()
	{
		if(m_rayLambda[0] < 1.0f)
			return true;
		else
			return false;
	}

	//���� ������ ���ϰ��ִ� ���������Ѵ�.(0,0,1) ����
	btScalar getBodyFrontAngle();	

	//! ��ó������, ��ֹ�����
	void preStep();	

	void controlStep_Walker(btVector3 &WalkVelocity);
	//! Angle�� ����
	void controlStep_Walker(btScalar Angle,btScalar Speed);

	//! WalkVelocity ���� �ӵ�(�ӷ�+����)
	//! bVelforce ������ ����
	//! bSyncMoveDirection �̵�����ٶ󺸱�
	void controlStep(btVector3 &WalkVelocity,bool bVelforce=false,bool bSyncMoveDirection=false);

};


}
}

#endif