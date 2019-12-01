#pragma warning(disable:949)
#pragma warning(disable:4819)
#include "CBulletWorldAnimator.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

//#define USE_PARALLEL_DISPATCHER 1
#ifdef USE_PARALLEL_DISPATCHER
#include "SpuGatheringCollisionDispatcher.h"
#endif//USE_PARALLEL_DISPATCHER

//2008,10.7 �޸𸮸�����, ����Ʈ���� �����߰�
//2009,9.29 irredit�� ����

#include "CBulletAnimatorManager.h"

#include "CBulletPhysicsUtils.h"

namespace irr
{
	namespace scene
	{
		const c8* CBulletWorldAnimator::WorldTypeList[] =			
		{
			"Rigid",
			"Soft",			
			0
		};	

		//------------------------------------------------------------------------------
		//! createBulletPhysicsAnimator
		//! Create CBulletWorldAnimatorAnimator
		CBulletWorldAnimator* CBulletWorldAnimator::createInstance(
			ISceneManager* scenemanager,
			//ISceneNode* sceneNode,
			CBulletAnimatorManager* bulletMgr,
			Params* params)
		{
			CBulletWorldAnimator* animator;

			animator = new CBulletWorldAnimator(bulletMgr,params);

			/*
			if(params->bSoft)
			{
			animator = new CBulletWorldAnimator(bulletMgr,params);
			}
			else
			{
			animator = new CBulletWorldAnimator(bulletMgr,params->m_AabbWorld);
			}
			*/

			// set params
			animator->setGravity(params->gravity);
			//animator-> m_gravity.setValue(0,-10,0);
			animator->m_softBodyWorldInfo.m_sparsesdf.Initialize();

			// set world's ID
			static s32 IDCounter = 1;
			animator->iID = IDCounter++;

			return animator;
		}

		//------------------------------------------------------------------------------
		//! createBulletPhysicsAnimator
		//! Create CBulletWorldAnimatorAnimator
		CBulletWorldAnimator* CBulletWorldAnimator::createEmptyInstance(
			ISceneManager* scenemanager,
			ISceneNode* sceneNode,
			CBulletAnimatorManager* bulletMgr
			)
		{
			CBulletWorldAnimator* animator = new CBulletWorldAnimator(bulletMgr,NULL);			

			return animator;
		}


		//------------------------------------------------------------------------------
		//! Ctor
		//! Base initialization
		CBulletWorldAnimator::CBulletWorldAnimator(CBulletAnimatorManager* bulletMgr,Params *params)
			: iBulletManager(bulletMgr), iID(-1)
		{
#ifdef _DEBUG
			setDebugName("CBulletWorldAnimator");
#endif
			iDynamicsWorld = NULL;

			if(params)
			{
				/*btVector3 aWorldMin;
				btVector3 aWorldMax;
				Irrlicht2Bullet(params->m_AabbWorld.MinEdge,aWorldMin);
				Irrlicht2Bullet(params->m_AabbWorld.MaxEdge,aWorldMax);*/

				InitDynamicsWorld(params);
//
//#ifdef USE_PARALLEL_DISPATCHER
//				iCollisionDispatcher = new	SpuGatheringCollisionDispatcher();
//#else
//
//				btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
//				iCollisionDispatcher = new	btCollisionDispatcher(collisionConfiguration);
//				iCollisionConfiguration = collisionConfiguration;
//
//#endif//USE_PARALLEL_DISPATCHER
//
//				//register algorithm
//				btGImpactCollisionAlgorithm::registerAlgorithm(iCollisionDispatcher);
//
//
//				btVector3 aWorldMin;
//				btVector3 aWorldMax;
//
//
//				Irrlicht2Bullet(params->m_AabbWorld.MinEdge,aWorldMin);
//				Irrlicht2Bullet(params->m_AabbWorld.MaxEdge,aWorldMax);
//
//
//
//				iPairCache = new btAxisSweep3(aWorldMin,aWorldMax);
//
//				//btOverlappingPairCache* aPairCache = new btAxisSweep3(aWorldMin,aWorldMax);
//				//iPairCache = new btSimpleBroadphase(); 
//
//				iConstraintSolver = new btSequentialImpulseConstraintSolver();
//				iDynamicsWorld = new btDiscreteDynamicsWorld(
//					iCollisionDispatcher, 
//					iPairCache,	  
//					iConstraintSolver,
//					collisionConfiguration
//					); 
//
//#ifdef USE_PARALLEL_DISPATCHER
//				iDynamicsWorld->getDispatchInfo().m_enableSPU=true;
//#endif //USE_PARALLEL_DISPATCHER
			}
			else
			{
				//deserializtion�� ���� ���߿� �����Ѵ�.
				iDynamicsWorld = NULL;
			}
		}

		//------------------------------------------------------------------------------
		//! Ctor 
		//! Base initialization for softworld
		//		CBulletWorldAnimator::CBulletWorldAnimator(CBulletAnimatorManager* bulletMgr,int bSoft,irr::core::vector3df m_WorldAxiesMin,irr::core::vector3df m_WorldAxiesMax)
		//		{
		//#ifdef _DEBUG
		//			setDebugName("CBulletWorldAnimator For soft");
		//#endif
		//
		//#ifdef USE_PARALLEL_DISPATCHER
		//			iCollisionDispatcher = new	SpuGatheringCollisionDispatcher();
		//#else
		//
		//			iCollisionDispatcher=0;
		//			///register some softbody collision algorithms on top of the default btDefaultCollisionConfiguration
		//			btDefaultCollisionConfiguration *collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
		//#endif//USE_PARALLEL_DISPATCHER
		//
		//			iCollisionDispatcher = new	btCollisionDispatcher(collisionConfiguration);
		//			m_softBodyWorldInfo.m_dispatcher = iCollisionDispatcher;
		//
		//			btVector3 worldAabbMin;
		//			btVector3 worldAabbMax;
		//
		//			Irrlicht2Bullet(WorldAxiesMin,aWorldMin);
		//			Irrlicht2Bullet(WorldAxiesMax,aWorldMax);
		//
		//			iPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax);
		//
		//			m_softBodyWorldInfo.m_broadphase = iPairCache;
		//
		//			btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
		//
		//			iConstraintSolver = solver;
		//
		//			btDiscreteDynamicsWorld* world = new btSoftRigidDynamicsWorld(
		//				iCollisionDispatcher,
		//				iPairCache,
		//				iConstraintSolver,
		//				collisionConfiguration
		//				);
		//
		//			iDynamicsWorld = world;
		//			//iDynamicsWorld->getDispatchInfo().m_enableSPU = true;
		//			//iDynamicsWorld->setGravity(btVector3(0,-10,0));
		//			//m_softBodyWorldInfo.m_gravity.setValue(0,-10,0);
		//			//m_softBodyWorldInfo.m_sparsesdf.Initialize();
		//
		//#ifdef USE_PARALLEL_DISPATCHER
		//			iDynamicsWorld->getDispatchInfo().m_enableSPU=true;
		//#endif //USE_PARALLEL_DISPATCHER
		//		}

		//------------------------------------------------------------------------------
		//! Dtor
		//! 
		CBulletWorldAnimator::~CBulletWorldAnimator()
		{
			iBulletManager->removeWorld(this);

			delete iDynamicsWorld;
			delete iConstraintSolver;
			delete iPairCache;
			delete iCollisionDispatcher;
			delete iCollisionConfiguration;
		}

		//------------------------------------------------------------------------------
		//! getType
		//! Returns type of the scene node animator
		ESCENE_NODE_ANIMATOR_TYPE CBulletWorldAnimator::getType() const 
		{ 
			return (ESCENE_NODE_ANIMATOR_TYPE)ESNAT_BULLET_WORLD; 
		}


		//------------------------------------------------------------------------------
		//! setGravity
		//! World's gravity
		void CBulletWorldAnimator::setGravity(const core::vector3df& gravity)
		{
			btVector3 iGravity;
			iGravity.setValue(gravity.X, gravity.Y, gravity.Z);
			iDynamicsWorld->setGravity(iGravity);

			m_softBodyWorldInfo.m_gravity = iGravity;
			
			m_worldParam.gravity = gravity;
		}

		//------------------------------------------------------------------------------
		//! getGravity
		//! World's gravity
		core::vector3df CBulletWorldAnimator::getGravity() const
		{
			//return core::vector3df(iGravity.x(), iGravity.y(), iGravity.z());
			return m_worldParam.gravity;
		}


		//------------------------------------------------------------------------------
		//! Custom per tick update func
		//! 
		void CBulletWorldAnimator::OnUpdate(u32 timeMs)
		{
			if (iDynamicsWorld)
			{
				//remember, Bullet uses a fixed internal timestep and interpolates
				// if the delta time is different. Damn useful.

				/*
				fixedTimeStep(fts)�� ���������� ȣ��ɶ� ����Ǿ��ϴ� �ð������̴�.

				fps = 1/60
				maxsubstep = 3; �̸� 20�����ӱ��� ������ ���Ѽ� �ùķ��̼� �϶�� ���̴�.

				*/
				//iDynamicsWorld->stepSimulation(timeMs/1000000.f, 3, 1.f/60.f);
				//������������ 60�����ӿ� ���� 20�����ӱ��� ������ ���� �ùķ��̼���
				//20�����Ӽӵ��̸� �ִ� 3������ �ݺ� �ùķ��̼��� ������ (20*3 = 60)
				iDynamicsWorld->stepSimulation(timeMs/1000000.f, 3, 1.f/60.f);
				//iDynamicsWorld->stepSimulation((1.f/120.f), 3, 1.f/60.f);
				//iDynamicsWorld->stepSimulation(btScalar(timeMs),3);
				//static float ftick =0;
				//ftick += (1/60.f);
				// float dt = float(getDeltaTimeMicroseconds()) * 0.000001f;
				//iDynamicsWorld->stepSimulation(dt);
				//iDynamicsWorld->stepSimulation(btScalar(timeMs),1,btScalar(timeMs)/btScalar(1000000.0));

				// apply the new positions and rotations to the scenenodes
				iDynamicsWorld->updateAabbs();
			}
		}

		//-------------------------------------------------------------------------------------
		//!����ó�� �Լ�
		//!timeMs ���е��� ����ũ�� ������ ������
		/*
		�������� ���������� ���� Ÿ�Խ����� ����Ѵ�. 
		���� Ÿ�Խ����� ����Ÿ�Խ��ܰ� ���Ͽ� �����ؼ� ���ȴ�.
		fixedTimeStep(fts)�� ���������� ȣ��ɶ� ����Ǿ��ϴ� �ð������̴�.

		fps = 1/60
		maxsubstep = 3; �̸� 20�����ӱ��� ������ ���Ѽ� �ùķ��̼� �϶�� ���̴�.

		������� timeStep���� 1/20�̵����� 3�� singleStepSimulation�� �ݺ�ó���Ѵ�.
		1/120 �̸� �ѹ����� �ѹ�ó���Ѵ�.

		*/
		void CBulletWorldAnimator::OnUpdate(btScalar timeStep,int maxSubStep,btScalar fixedTimeStep)
		{
			if (iDynamicsWorld)
			{	
				//iDynamicsWorld->stepSimulation(timeMs/1000000.f, maxSubStep, fixedTimeStep);				
				iDynamicsWorld->stepSimulation(timeStep, maxSubStep, fixedTimeStep);				

				// apply the new positions and rotations to the scenenodes
				iDynamicsWorld->updateAabbs();
			}
		}

		//------------------------------------------------------------------------------
		//! RegisterBody
		//! 
		void CBulletWorldAnimator::RegisterBody(btRigidBody* rigidBody)
		{
			iDynamicsWorld->addRigidBody(rigidBody);
		}

		//------------------------------------------------------------------------------
		//! UnregisterBody
		//! 
		void CBulletWorldAnimator::UnregisterBody(btRigidBody* rigidBody)
		{
			iDynamicsWorld->removeRigidBody(rigidBody);
		}



		//------------------------------------------------------------------------------
		//! animateNode
		//! animates a scene node
		void CBulletWorldAnimator::animateNode(ISceneNode* node, u32 timeMs)
		{			
		}

		//------------------------------------------------------------------------------
		//! serializeAttributes
		//! Writes attributes of the scene node animator.
		void CBulletWorldAnimator::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options/*=0*/) const
		{
			// store world's id
			out->addInt("id", iID);
			//out->addString("name",

			btVector3 aamin,aamax;
			iPairCache->getBroadphaseAabb(aamin,aamax);

			//out->addInt("worldType",m_worldParam.worldType);
			out->addEnum("worldType",m_worldParam.worldType,WorldTypeList);
			out->addVector3d("minAxies",m_worldParam.m_AabbWorld.MinEdge);
			out->addVector3d("maxAxies",m_worldParam.m_AabbWorld.MaxEdge);

			if(m_worldParam.worldType == EWT_SOFT)
			{
				out->addFloat("air_density",m_softBodyWorldInfo.air_density);
				out->addFloat("water_density",m_softBodyWorldInfo.water_density);
				out->addFloat("water_offset",m_softBodyWorldInfo.water_offset);

				
				out->addVector3d("water_normal",
					irr::core::vector3df(m_softBodyWorldInfo.water_normal.getX(),
					m_softBodyWorldInfo.water_normal.getY(),
					m_softBodyWorldInfo.water_normal.getZ()
					)
				);
			}

			core::vector3df ip;
			btVector3 bp;
			btQuaternion bq;

			// save global gravity
			out->addVector3d("gravity",getGravity() );
		}

		//------------------------------------------------------------------------------
		//! deserializeAttributes
		//! Reads attributes of the scene node animator.
		void CBulletWorldAnimator::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options/*=0*/)
		{
			// set world's id
			iID = in->getAttributeAsInt("id");

			//core::vector3df ip;

			btVector3 iGravity;
			//btVector3 bp;
			//btQuaternion bq;

			// save global gravity
			m_worldParam.gravity = in->getAttributeAsVector3d("gravity");

			m_worldParam.m_AabbWorld.MinEdge = in->getAttributeAsVector3d("minAxies");
			m_worldParam.m_AabbWorld.MaxEdge = in->getAttributeAsVector3d("maxAxies");
			m_worldParam.worldType = in->getAttributeAsEnumeration("worldType",WorldTypeList);
			
			if(InitDynamicsWorld(&m_worldParam) == false)
			{
			}
			else
			{
				iGravity = btVector3(m_worldParam.gravity.X,m_worldParam.gravity.Y,m_worldParam.gravity.Z);
				iDynamicsWorld->setGravity(iGravity);
			}
		}

		//------------------------------------------------------------------------------
		//! ���忡 ��ϵ� ��� ������ �����Ѵ�.
		//! 
		void CBulletWorldAnimator::removeAllConstraints()
		{
			int i;
			//removed/delete constraints
			for (i=iDynamicsWorld->getNumConstraints()-1; i>=0 ;i--)
			{
				btTypedConstraint* constraint = iDynamicsWorld->getConstraint(i);
				iDynamicsWorld->removeConstraint(constraint);			
				delete constraint;
			}
		}

		//---------------------------------------------------------------
		//!���� �ʱ�ȭ
		bool CBulletWorldAnimator::InitDynamicsWorld(Params *param)
		{
			if(!iDynamicsWorld && param)
			{
				/*irr::core::vector3df vAxiesMin;
				irr::core::vector3df vAxiesMax;

				vAxiesMin = m_worldParam.m_AabbWorld.MinEdge;
				vAxiesMax = m_worldParam.m_AabbWorld.MaxEdge;*/

				m_worldParam = *param;

				{
#ifdef USE_PARALLEL_DISPATCHER
					iCollisionDispatcher = new	SpuGatheringCollisionDispatcher();
#else

					btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
					iCollisionDispatcher = new	btCollisionDispatcher(collisionConfiguration);
					iCollisionConfiguration = collisionConfiguration;
#endif//USE_PARALLEL_DISPATCHER

					//register algorithm
					btGImpactCollisionAlgorithm::registerAlgorithm(iCollisionDispatcher);

					btVector3 aWorldMin;
					btVector3 aWorldMax;

					Irrlicht2Bullet(m_worldParam.m_AabbWorld.MinEdge,aWorldMin);
					Irrlicht2Bullet(m_worldParam.m_AabbWorld.MaxEdge,aWorldMax);

					iPairCache = new btAxisSweep3(aWorldMin,aWorldMax);					
					iConstraintSolver = new btSequentialImpulseConstraintSolver();
					iDynamicsWorld = new btDiscreteDynamicsWorld(
						iCollisionDispatcher, 
						iPairCache,	  
						iConstraintSolver,
						collisionConfiguration
						); 

#ifdef USE_PARALLEL_DISPATCHER
					iDynamicsWorld->getDispatchInfo().m_enableSPU=true;
#endif //USE_PARALLEL_DISPATCHER
				}
				return true;

			}
			return false;
		}

	} // end namespace scene
} // end namespace irr
