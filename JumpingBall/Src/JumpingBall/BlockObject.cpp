#include "BlockObject.h"
#include "BulletBySceneAnimator.h"
#include "App.h"

const irr::f32 spring_block_finalMoveValue = 20.f;
const irr::f32 spring_block_movingValue = 0.1f;
const irr::f32 spring_block_movingTime = 0.01f;
const irr::f32 spring_block_movingEpsilon = 0.2f;
const irr::f32 spring_block_movingDelayTime = 0.05f;

CBlockObject::CBlockObject(void)
: m_pMainSceneNode(NULL), m_pBallObj(NULL), m_pRigidBody(NULL), m_pMotionState(NULL), m_bIsRemoveFromWorld(true), m_pBulletBySceneAnimator(NULL)
{
}

CBlockObject::~CBlockObject(void)
{
	removeRigidBodyFromWorld();

	if(m_pMotionState)
	{
		delete m_pMotionState;
	}

	if(m_pRigidBody)
	{
		delete m_pRigidBody;
	}
}

irr::core::vector3df CBlockObject::getPosition()
{
	return m_pMainSceneNode->getAbsolutePosition();
}

bool CBlockObject::isDie()
{
	return false;
}

void CBlockObject::Signal(irr::core::stringc strSignal,void *pParam)
{

}

void CBlockObject::Update(irr::f32 fTick)
{
	switch (m_uBlockProperty)
	{
	case irr::scene::jz3d::BLOCK_PROPERTY_ILLUSION:
		{
			// 바운딩박스로 충돌처리 하기
			irr::core::aabbox3df box1, box2;

			// 아이템노드의 바운딩박스 얻기
			box1 = m_pMainSceneNode->getBoundingBox();
			// 바운딩박스에 아이템노드의 변환 적용
			m_pMainSceneNode->getAbsoluteTransformation().transformBox(box1);

			// 공노드의 바운딩박스 얻기
			box2 = m_pBallObj->getSceneNode()->getBoundingBox();
			// 바운딩박스에 공노드의 변환 적용
			m_pBallObj->getSceneNode()->getAbsoluteTransformation().transformBox(box2);

			// 충돌체크 하기
			if(box1.intersectsWithBox(box2))
			{
				m_pMainSceneNode->getMaterial(0).MaterialType = irr::video::EMT_TRANSPARENT_ADD_COLOR;
			}
			else
			{
				m_pMainSceneNode->getMaterial(0).MaterialType = irr::video::EMT_SOLID;
			}
		}
		break;

	case irr::scene::jz3d::BLOCK_PROPERTY_SPRING:
		{

			switch(GetStatus())
			{
			case READY:
				{
					// 바운딩박스로 충돌처리 하기
					irr::core::aabbox3df box1, box2;

					// 아이템노드의 바운딩박스 얻기
					box1 = m_pMainSceneNode->getBoundingBox();
					// 바운딩박스에 아이템노드의 변환 적용
					m_pMainSceneNode->getAbsoluteTransformation().transformBox(box1);

					// 공노드의 바운딩박스 얻기
					box2 = m_pBallObj->getSceneNode()->getBoundingBox();
					// 바운딩박스에 공노드의 변환 적용
					m_pBallObj->getSceneNode()->getAbsoluteTransformation().transformBox(box2);

					// 충돌체크 하기
					if(box1.intersectsWithBox(box2))
					{
						irr::core::vector3df force;

						switch( static_cast<irr::u32>(m_fBlockPropertyValue[0]) )
						{
						case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_UP:
							{
								force.set(0,0,1);
							}
							break;

						case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_DOWN:
							{
								force.set(0,0,-1);
							}
							break;

						case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_LEFT:
							{
								force.set(-1,0,0);
							}
							break;

						case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_RIGHT:
							{
								force.set(1,0,0);
							}
							break;
						}

						force *= m_fBlockPropertyValue[1];
						m_pBallObj->Signal("force", &force);

						SetStatus(ANIMATION);
						m_fBlockPropertyValue[2] = 0;
					}
				}
				break;

			case ANIMATION:
				{
					switch(GetStep())
					{
					case 0:
						{
							

							m_fBlockPropertyValue[2] += fTick;

							if(m_fBlockPropertyValue[2] > spring_block_movingDelayTime)
							{
								// spring sound
								CApp::Get().NativePlaySound("res/sound/spring.mp3");

								m_fBlockPropertyValue[2] = 0;
								IncStep();
							}
						}
						break;

					case 1:
						{
							irr::core::vector3df moving_direction;

							switch( static_cast<irr::u32>(m_fBlockPropertyValue[0]) )
							{
							case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_UP:
								{
									moving_direction.set(0,0,1);
								}
								break;

							case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_DOWN:
								{
									moving_direction.set(0,0,-1);
								}
								break;

							case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_LEFT:
								{
									moving_direction.set(-1,0,0);
								}
								break;

							case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_RIGHT:
								{
									moving_direction.set(1,0,0);
								}
								break;
							}

							m_pMainSceneNode->setPosition( m_pMainSceneNode->getPosition() + (moving_direction * spring_block_finalMoveValue) );

							SetStatus(RETURN);
						}
						break;
					}	
				}
				break;

			case RETURN:
				{
					irr::core::vector3df moving_direction;

					switch( static_cast<irr::u32>(m_fBlockPropertyValue[0]) )
					{
					case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_UP:
						{
							moving_direction.set(0,0,-1);
						}
						break;

					case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_DOWN:
						{
							moving_direction.set(0,0,1);
						}
						break;

					case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_LEFT:
						{
							moving_direction.set(1,0,0);
						}
						break;

					case irr::scene::jz3d::BLOCK_PROPERTY_SPRING_DIRECTION_RIGHT:
						{
							moving_direction.set(-1,0,0);
						}
						break;
					}

					if(  compareVectorsByEpsilon(m_pMainSceneNode->getStartPosition(), m_pMainSceneNode->getPosition() , spring_block_movingEpsilon) )
					{
						m_pMainSceneNode->setPosition(m_pMainSceneNode->getStartPosition());
						SetStatus(READY);
					}
					else
					{
						m_fBlockPropertyValue[2] += fTick;
						if(m_fBlockPropertyValue[2] > spring_block_movingTime)
						{
							m_pMainSceneNode->setPosition(m_pMainSceneNode->getPosition() + (moving_direction * spring_block_movingValue));
							m_fBlockPropertyValue[2] = 0;
						}
					}
				}
				break;
			}

		}
		break;
	}
}

btRigidBody* CBlockObject::getRigidBody()
{
	return m_pRigidBody;
}

irr::scene::ISceneNode* CBlockObject::getSceneNode()
{
	return m_pMainSceneNode;
}

irr::scene::CBulletChracterAnimator* CBlockObject::getAnimator()
{
	return NULL;
}

bool CBlockObject::isBall()
{
	return false;
}

void CBlockObject::setTargetObject(IFSMObject* obj)
{
	m_pBallObj = obj;
}

bool CBlockObject::Init(irr::scene::jz3d::CBlockNode* pNode, irr::scene::CBulletWorldAnimator* pWorldAnimator)
{
	// 월드애니메이터
	m_pWorldAnimator = pWorldAnimator;

	// 메인 신노드
	m_pMainSceneNode = pNode;

	// 블럭속성
	m_uBlockProperty = pNode->getBlockProperty();

	// 블럭속성값
	irr::u32 i;
	for(i = 0 ; i < 3 ; ++i)
		m_fBlockPropertyValue[i] = pNode->getPropertyValue(i);

	if(m_uBlockProperty != irr::scene::jz3d::BLOCK_PROPERTY_ILLUSION)
	{
		// 블럭노드로부터 collisionShape 가져오기
		btCollisionShape *pColShape = pNode->getCollisionShape();

		// 지형이므로 static rigidbody이다. 즉 움직이지 않는 강체. 질략은 0
		btScalar	mass(0.f);
		btVector3 localInertia(0,0,0);
		pColShape->calculateLocalInertia(mass,localInertia);

		// 매트릭스 생성하여 좌표와 각도 계산.
		btTransform startTransform;
		irr::core::matrix4 mat;
		irr::core::matrix4 mat_j = pNode->getAbsoluteTransformation();
		irr::core::matrix4 mat_t, mat_r;
		mat_t.setTranslation(mat_j.getTranslation());
		mat_r.setRotationDegrees(mat_j.getRotationDegrees());
		mat = mat_t * mat_r;
		startTransform.setFromOpenGLMatrix(mat.pointer());
 
		// motionState 생성
		btDefaultMotionState* pMotionState = new btDefaultMotionState(startTransform);

		// 강체 생성정보 만들기
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, pMotionState, pColShape, localInertia);

		// 위에서만든 강체생성정보를 가지고 강체 생성
		btRigidBody* pRigidBody = new btRigidBody(rbInfo);	

		// 물리속성 추가
		pRigidBody->setFriction(pNode->m_fFriction); // 마찰력
		pRigidBody->setRestitution(pNode->m_fRestitution); // 탄성력

		// 월드에 박스 강체 추가
		m_pWorldAnimator->getWorld()->addRigidBody(pRigidBody);

		// 포인터 저장
		m_pMotionState = pMotionState;
		m_pRigidBody = pRigidBody;


		CBulletBySceneAnimator* pAnimator = new CBulletBySceneAnimator(pRigidBody);
		m_pMainSceneNode->addAnimator(pAnimator);
		m_pBulletBySceneAnimator = pAnimator;
		pAnimator->drop();

		m_bIsRemoveFromWorld = false;
	}

	SetStatus(READY);
	
	return true;
}

void CBlockObject::reset()
{
	m_pMainSceneNode->setPosition(m_pMainSceneNode->getStartPosition());
	SetStatus(READY);
}

void CBlockObject::removeRigidBodyFromWorld()
{
	if(!m_bIsRemoveFromWorld)
	{
		if(m_pRigidBody)
		{
			m_pWorldAnimator->getWorld()->removeRigidBody(m_pRigidBody);
			m_bIsRemoveFromWorld = true;
			m_pMainSceneNode->removeAnimator(m_pBulletBySceneAnimator);
		}
	}
}

bool CBlockObject::compareVectorsByEpsilon(const irr::core::vector3df& val1, const irr::core::vector3df& val2, irr::f32 epsilon)
{
	if( 
		irr::core::abs_(val1.X - val2.X) < epsilon &&
		irr::core::abs_(val1.Y - val2.Y) < epsilon && 
		irr::core::abs_(val1.Z - val2.Z) < epsilon
		)
		return true;

	return false;
}