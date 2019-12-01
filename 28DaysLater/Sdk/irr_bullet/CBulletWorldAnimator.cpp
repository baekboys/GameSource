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

//2008,10.7 메모리릭수정, 소프트월드 생성추가

#include "CBulletAnimatorManager.h"

namespace irr
{
	namespace scene
	{

		//------------------------------------------------------------------------------
		//! createBulletPhysicsAnimator
		//! Create CBulletWorldAnimatorAnimator
		CBulletWorldAnimator* CBulletWorldAnimator::createInstance(
			ISceneManager* scenemanager,
			ISceneNode* sceneNode,
			CBulletAnimatorManager* bulletMgr,
			CBulletWorldAnimatorParams* params)
		{
			CBulletWorldAnimator* animator;
			if(params->bSoft)
			{
				animator = new CBulletWorldAnimator(bulletMgr,true);
			}
			else
			{
				animator = new CBulletWorldAnimator(bulletMgr);
			}

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
			CBulletAnimatorManager* bulletMgr)
		{
			CBulletWorldAnimator* animator = new CBulletWorldAnimator(bulletMgr);

			return animator;
		}


		//------------------------------------------------------------------------------
		//! Ctor
		//! Base initialization
		CBulletWorldAnimator::CBulletWorldAnimator(CBulletAnimatorManager* bulletMgr)
			: iBulletManager(bulletMgr), iID(-1)
		{
#ifdef _DEBUG
			setDebugName("CBulletWorldAnimator");
#endif

#ifdef USE_PARALLEL_DISPATCHER
			iCollisionDispatcher = new	SpuGatheringCollisionDispatcher();
#else

			btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
			iCollisionDispatcher = new	btCollisionDispatcher(collisionConfiguration);
			iCollisionConfiguration = collisionConfiguration;

#endif//USE_PARALLEL_DISPATCHER

			//register algorithm
			btGImpactCollisionAlgorithm::registerAlgorithm(iCollisionDispatcher);


			btVector3 aWorldMin(-10000,-10000,-10000);
			btVector3 aWorldMax(10000,10000,10000);
			iPairCache = new btAxisSweep3(aWorldMin,aWorldMax);
			//btOverlappingPairCache* aPairCache = new btAxisSweep3(aWorldMin,aWorldMax);

			//iPairCache = new btSimpleBroadphase(); 

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

		//------------------------------------------------------------------------------
		//! Ctor 
		//! Base initialization for softworld
		CBulletWorldAnimator::CBulletWorldAnimator(CBulletAnimatorManager* bulletMgr,int bSoft)
		{
#ifdef _DEBUG
			setDebugName("CBulletWorldAnimator For soft");
#endif

#ifdef USE_PARALLEL_DISPATCHER
			iCollisionDispatcher = new	SpuGatheringCollisionDispatcher();
#else

			iCollisionDispatcher=0;
			///register some softbody collision algorithms on top of the default btDefaultCollisionConfiguration
			btDefaultCollisionConfiguration *collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
#endif//USE_PARALLEL_DISPATCHER

			iCollisionDispatcher = new	btCollisionDispatcher(collisionConfiguration);
			m_softBodyWorldInfo.m_dispatcher = iCollisionDispatcher;

			btVector3 worldAabbMin(-1000,-1000,-1000);
			btVector3 worldAabbMax(1000,1000,1000);

			iPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax);

			m_softBodyWorldInfo.m_broadphase = iPairCache;

			btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

			iConstraintSolver = solver;

			btDiscreteDynamicsWorld* world = new btSoftRigidDynamicsWorld(
				iCollisionDispatcher,
				iPairCache,
				iConstraintSolver,
				collisionConfiguration
				);

			iDynamicsWorld = world;
			//iDynamicsWorld->getDispatchInfo().m_enableSPU = true;
			//iDynamicsWorld->setGravity(btVector3(0,-10,0));
			//m_softBodyWorldInfo.m_gravity.setValue(0,-10,0);
			//m_softBodyWorldInfo.m_sparsesdf.Initialize();

#ifdef USE_PARALLEL_DISPATCHER
			iDynamicsWorld->getDispatchInfo().m_enableSPU=true;
#endif //USE_PARALLEL_DISPATCHER
		}

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
			iGravity.setValue(gravity.X, gravity.Y, gravity.Z);
			iDynamicsWorld->setGravity(iGravity);
			m_softBodyWorldInfo.m_gravity = iGravity;
		}

		//------------------------------------------------------------------------------
		//! getGravity
		//! World's gravity
		core::vector3df CBulletWorldAnimator::getGravity() const
		{
			return core::vector3df(iGravity.x(), iGravity.y(), iGravity.z());
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
				fixedTimeStep(fts)는 서프스탭이 호출될때 보장되야하는 시간단위이다.
				
				fps = 1/60
				maxsubstep = 3; 이면 20프레임까지 보정을 시켜서 시뮬레이션 하라는 뜻이다.

				*/
				//iDynamicsWorld->stepSimulation(timeMs/1000000.f, 3, 1.f/60.f);
				//고정프레임을 60프레임에 맞춰 20프레임까지 보정을 시켜 시뮬레이션함
				//20프레임속도이면 최대 3번연속 반복 시뮬레이션을 수행함 (20*3 = 60)
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
		//!물리처리 함수
		//!timeMs 정밀도는 마이크로 세컨드 단위임
		/*
		블릿엔진은 내부적으로 고정 타입스텝을 사용한다. 
		실제 타입스탭은 고정타입스텝과 비교하여 보간해서 사용된다.
		fixedTimeStep(fts)는 서프스탭이 호출될때 보장되야하는 시간단위이다.

		fps = 1/60
		maxsubstep = 3; 이면 20프레임까지 보정을 시켜서 시뮬레이션 하라는 뜻이다.

		예를들어 timeStep값이 1/20이들어오면 3번 singleStepSimulation을 반복처리한다.
		1/120 이면 한번쉬고 한번처리한다.

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
		void CBulletWorldAnimator::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options/*=0*/)
		{
			// store world's id
			out->addInt("id", iID);

			core::vector3df ip;
			btVector3 bp;
			btQuaternion bq;

			// save global gravity
			out->addVector3d("gravity", core::vector3df(iGravity.getX(), iGravity.getY(), iGravity.getZ()));
		}

		//------------------------------------------------------------------------------
		//! deserializeAttributes
		//! Reads attributes of the scene node animator.
		void CBulletWorldAnimator::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options/*=0*/)
		{
			// set world's id
			iID = in->getAttributeAsInt("id");

			core::vector3df ip;
			btVector3 bp;
			btQuaternion bq;

			// save global gravity
			ip = in->getAttributeAsVector3d("gravity");
			iGravity = btVector3(ip.X, ip.Y, ip.Z);
			iDynamicsWorld->setGravity(iGravity);
		}

		//------------------------------------------------------------------------------
		//! 월드에 등록된 모든 구속을 제거한다.
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

	} // end namespace scene
} // end namespace irr
