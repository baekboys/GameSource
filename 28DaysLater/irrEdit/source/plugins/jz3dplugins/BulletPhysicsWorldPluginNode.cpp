#include "jz3d.h"

#include "BulletPhysicsObjPluginNode.h"
#include "BulletPhysicsWorldPluginNode.h"

/*
���� �ùķ��̼� �ϴ¹�
���� ���带 �����.(CBulletPhysicsWorldPluginNode)
���带 ���۽�Ű���� start ����� ������.

������� ���� ��忡 ���� ������Ʈ���� �߰� ��Ų��.(CBulletPhysicsObjPluginNode)
���� ������Ʈ�� ���۽�Ű���� start ����� ������.




*/

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const c8* CBulletPhysicsWorldPluginNode::CmdLsit[] =			
			{
				"---",
				"start",
				"stop",
				"start_children", //���峻�� ��� ������Ʈ ��������
				"stop_children", //������� ��� ������Ʈ ���� ����				
				0
			};	

			const char *CBulletPhysicsWorldPluginNode::Name  = "CBulletPhysicsWorldPluginNode";

			CBulletPhysicsWorldPluginNode::CBulletPhysicsWorldPluginNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent,mgr,id),
				m_fixedTimeStep(1/60.f),
				m_maxSubStep(3),
				m_WorldAxiesMax(10000,10000,10000),
				m_WorldAxiesMin(-10000,-10000,-10000),
				m_pWorldAnimator(NULL),
				m_strCmd(CmdLsit[0])
			{
#ifdef _DEBUG
				setDebugName(CBulletPhysicsWorldPluginNode::Name);
#endif
				setAutomaticCulling(scene::EAC_OFF);	

				m_uLastTick = 0;
			}			

			//! pre render event
			void CBulletPhysicsWorldPluginNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);

				ISceneNode::OnRegisterSceneNode();
			}


			void CBulletPhysicsWorldPluginNode::OnAnimate(irr::u32 timeMs)
			{
				if (IsVisible)
				{
					irr::f32 fDelta = ((float)(timeMs - m_uLastTick)) / 1000.f; //��Ÿ�� ���ϱ�
					m_uLastTick = timeMs;	

					if(m_pWorldAnimator == NULL)
					{
						//�ִϸ����� ���ϱ�
						irr::core::list<irr::scene::ISceneNodeAnimator *>::ConstIterator it = getAnimators().begin();

						while(it !=  getAnimators().end())
						{
							if((*it)->getType() == ESNAT_BULLET_WORLD)
							{
								m_pWorldAnimator = (CBulletWorldAnimator *)(*it);
								break;
							}
						}
					}
					else
					{	
						m_pWorldAnimator->OnUpdate(fDelta,m_maxSubStep,m_fixedTimeStep);
					}				

					// animate this node with all animators

					/*core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
					for (; ait != Animators.end(); ++ait)
						(*ait)->animateNode(this, timeMs);*/

					// update absolute position
					//updateAbsolutePosition();

					// perform the post render process on all children

					core::list<ISceneNode*>::Iterator it = Children.begin();
					for (; it != Children.end(); ++it)
						(*it)->OnAnimate(timeMs);
				}
			}

			//! render
			void CBulletPhysicsWorldPluginNode::render()
			{
				// do nothing
			}			


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CBulletPhysicsWorldPluginNode::getBoundingBox() const
			{
				return Box;
			}

			//! Creates a clone of this scene node and its children.
			ISceneNode* CBulletPhysicsWorldPluginNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CBulletPhysicsWorldPluginNode* nb = new CBulletPhysicsWorldPluginNode(newParent, 
					newManager, ID);

				nb->cloneMembers(this, newManager);
				nb->Box = Box;

				nb->drop();
				return nb;
			}		


			void CBulletPhysicsWorldPluginNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);		
				
				//������ ���� �ڵ��߰�
				//...

				out->addEnum("command",m_strCmd.c_str(),CBulletPhysicsWorldPluginNode::CmdLsit);

				if(m_pWorldAnimator)
					out->addInt("worldID",m_pWorldAnimator->getID());

				out->addVector3d("worldMax",m_WorldAxiesMax);
				out->addVector3d("worldMin",m_WorldAxiesMin);
				//out->addBool("IsSoft",m_WorldParam.bSoft);
				out->addVector3d("gravity",m_WorldParam.gravity);
				
				out->addFloat("FixedTimeStep",m_fixedTimeStep);
				out->addInt("MaxSubStep",m_maxSubStep);



			}

			//! Reads attributes of the scene node.
			void CBulletPhysicsWorldPluginNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				//������ �б� �ڵ��߰�
				//...

				ISceneNode::deserializeAttributes(in, options);				

				m_strCmd = in->getAttributeAsEnumeration("command");

				//m_WorldAxiesMax = in->getAttributeAsVector3d("worldMax");
				//m_WorldAxiesMin = in->getAttributeAsVector3d("worldMin");
				//m_WorldParam.bSoft = in->getAttributeAsBool("IsSoft");
				m_WorldParam.gravity = in->getAttributeAsVector3d("gravity");
				m_WorldParam.m_AabbWorld.MinEdge = in->getAttributeAsVector3d("worldMin");
				m_WorldParam.m_AabbWorld.MaxEdge = in->getAttributeAsVector3d("worldMax");

				Box = m_WorldParam.m_AabbWorld;


				m_fixedTimeStep = in->getAttributeAsFloat("FixedTimeStep");
				m_maxSubStep = in->getAttributeAsInt("MaxSubStep");


#ifdef JZ3DPLUGINS_EXPORTS				

				////�޴�ó��
				if(m_strCmd == "stop_children")
				{
					stop_Children();
					m_strCmd = "---";
				}
				else if(m_strCmd == "start_children")
				{
					start_Children();
					m_strCmd = "---";
				}
				else if(m_strCmd == "start")
				{
					stop_Children();
					removeAnimators();

					m_pWorldAnimator = registerDynamics(this); //�ִϸ����� ���
					
					m_strCmd = "---";
				}
				else if(m_strCmd == "stop")
				{
					stop_Children();					
					removeAnimators();

					m_pWorldAnimator = NULL;

					m_strCmd = "---";
				}
#endif
			}

			
			//!���� �ִϸ����͸� ����Ѵ�.
			CBulletWorldAnimator* CBulletPhysicsWorldPluginNode::registerDynamics(irr::scene::ISceneNode *pNode)
			{
				CBulletWorldAnimator *pAnim;
				//if(m_pWorldAnimator == NULL)
				{
					irr::scene::CBulletAnimatorManager *ani_factory =
						static_cast<irr::scene::CBulletAnimatorManager *>(ggf::util::findAnimatorFactory(
						SceneManager,ESNAT_BULLET_WORLD));

					pAnim = ani_factory->createBulletWorldAnimator(
						SceneManager,
						pNode,
						&m_WorldParam
						);

					addAnimator(pAnim);
				}
				return pAnim;
			}

			//!��� �ڽĳ�� �ϰ������� �����ִϸ����� �߰�
			void CBulletPhysicsWorldPluginNode::start_Children()
			{
				
				core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
				{
					if((*it)->getType() == CBulletPhysicsObjPluginNode::TypeID )
					{
						CBulletPhysicsObjPluginNode *ptr = CBulletPhysicsObjPluginNode::upcast(*it);
						if(ptr)
						{
							//�ִϸ����Ͱ� �̹� ������ ������ �ʴ´�.
							if(ptr->getAnimators().getSize() == 0) 
							{
								ptr->RegisterPhysics(getName());
							}
						}
					}
				}
			}

			//!�ڽĳ���� ��� �ִϸ����� ����
			void CBulletPhysicsWorldPluginNode::stop_Children()
			{
				
				core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
				{
					(*it)->removeAnimators();							
				}
			}
		}
	}
}
