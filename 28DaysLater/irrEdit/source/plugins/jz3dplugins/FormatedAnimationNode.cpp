#include "FormatedAnimationNode.h"

/*

�� ������ ���� �����ִ°͵鿡 ���� �����Ѿ��..

1. ������ �����غ� �޴�ó��
�� �����ڴ� �ǵ� �ߴ����� �𸣰�����
�׻󼺱���� enum ���� �̿��ؼ� �޴��� �����ؼ� �ϸ���Ʈ�������� �߰� ��ɿ� ���� �������� ���ɼ��� ���ؼ� ������ �غô�.

cmd��� �������׸��� ���� ��� ������ �߰��ߴ�.
joint ����Ʈ��� �����ֱ�
addAni ���ο� �ִϸ��̼� �߰��ϱ�

�ʱ���´� ---���ִٰ� ��Ӵٿ�޴��� �������ָ� desierialze�Լ��� ȣ��Ǹ鼭
cmd ������ üũ�ؼ� �ش��ϴ� ������ �����ϰ� cmd������ �ٽ� ---���� �س����� ��ġ �ش��ư�� 
Ŭ��������(�Ǵ� ��Ӵٿ� �޴����� �޾����� ó�� �ϴ� �Ͱ� ������ ������ �����Ҽ��־���.

2.���� ������ Ȯ�� �Ǵ� ���� ����



*/

namespace irr 
{
	namespace scene
	{
		namespace jz3d 
		{
			const char *CFormatedAnimationNode::Name = "FormatedAnimationNode";

			const c8* CFormatedAnimationNode::CmdListNames[] =			
			{
				"---",
				"joint",
				"addAni",
				"delAni",
				0
			};	

			CFormatedAnimationNode::CFormatedAnimationNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
				: ISceneNode(parent, mgr, id),
				m_CurAnimation(""),
				m_TargetNodeName("body"),
				m_CurCmd("---"),
				m_strCmdParam("")				
			{
#ifdef _DEBUG
				setDebugName("CFormatedAnimationNode");
#endif
				setAutomaticCulling(scene::EAC_OFF);
			}


			//! pre render event
			void CFormatedAnimationNode::OnRegisterSceneNode()
			{
				if (IsVisible)
					SceneManager->registerNodeForRendering(this);

				ISceneNode::OnRegisterSceneNode();
			}


			//! render
			void CFormatedAnimationNode::render()
			{
				// do nothing
			}


			//! returns the axis aligned bounding box of this node
			const core::aabbox3d<f32>& CFormatedAnimationNode::getBoundingBox() const
			{
				return Box;
			}


			//! Creates a clone of this scene node and its children.
			ISceneNode* CFormatedAnimationNode::clone(ISceneNode* newParent, ISceneManager* newManager)
			{
				if (!newParent) newParent = Parent;
				if (!newManager) newManager = SceneManager;

				CFormatedAnimationNode* nb = new CFormatedAnimationNode(newParent, 
					newManager, ID);

				nb->cloneMembers(this, newManager);
				nb->Box = Box;
				nb->m_CurAnimation = m_CurAnimation;
				nb->m_mapActionDesc = m_mapActionDesc;
				nb->m_TargetNodeName = m_TargetNodeName;

				nb->m_strActionList = m_strActionList;
				nb->m_strwActionList = m_strwActionList;
				nb->m_ActionList =m_ActionList;

				nb->m_CurCmd = m_CurCmd;
				nb->m_strCmdParam = m_strCmdParam;


				nb->drop();
				return nb;
			}


			void CFormatedAnimationNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
			{
				ISceneNode::serializeAttributes(out, options);				

				out->addArray("actionlist",m_strwActionList);
				
				out->addEnum("cmd",m_CurCmd.c_str(),CFormatedAnimationNode::CmdListNames);
				out->addString("cmd Param1",m_strCmdParam.c_str());
				if(m_ActionList.size() > 1)
				{					
					out->addEnum("animation",m_CurAnimation.c_str(),(char **)&m_ActionList[0]);
				}

				out->addString("target",m_TargetNodeName.c_str());

				std::map<irr::core::stringc,irr::core::vector3df>::const_iterator it;

				for(it = m_mapActionDesc.begin();it !=m_mapActionDesc.end();it++)
				{  
					out->addVector3d(it->first.c_str(),it->second);
				}			
				
			}

			//! Reads attributes of the scene node.
			void CFormatedAnimationNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
			{	
				m_strwActionList = in->getAttributeAsArray("actionlist");

				if(m_strActionList.size() < m_strwActionList.size()) //���� �ε��Ȼ���
				{
					m_strActionList.clear();
					m_mapActionDesc.clear();
					m_ActionList.clear();

					u32 i=0;

					for(i=0;i<m_strwActionList.size();i++)
					{
						irr::core::stringc strc = irr::core::stringc(m_strwActionList[i].c_str());
						m_strActionList.push_back(strc);						
						m_mapActionDesc[strc] = irr::core::vector3df(0,0,0);
					}

					//������ �ΰ��� ������ ������ �����̳ĸ�...
					//����� ���� �ƴ϶� �ܼ� �����Ͱ��̹Ƿ� ������������ �ȵȴ�.
					for(i=0;i<m_strActionList.size();i++)
					{
						m_ActionList.push_back((char *const)m_strActionList[i].c_str());
					}
					m_ActionList.push_back(NULL); //���� ī��

				}

				//�÷����ο� �޴�ó��
				m_CurCmd = in->getAttributeAsEnumeration("cmd"); //Ŀ��Ʈ �б�
				m_strCmdParam = in->getAttributeAsString("cmd Param1");

#ifdef JZ3DPLUGINS_EXPORTS
				{
					//�̺κ���
					//�������α׷��󿡼��� �����ι������� ������ ������ �ϴ°�����.

					if(m_CurCmd == "---")
					{
					}
					else if(m_CurCmd == "joint")
					{
						irr::scene::IAnimatedMeshSceneNode *pTarNode = 
							(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
							m_TargetNodeName.c_str(),this);

						if(pTarNode)
						{
							u32 i;

							for(i=0;i<pTarNode->getJointCount();i++)
							{
								irr::scene::IBoneSceneNode *pBone =
									pTarNode->getJointNode(i);
							}
						}

						m_CurCmd = "---"; //�޴��� ó���ϰ� �����·� �ǵ������´�.(������ Ŭ�����)
					}
					else if(m_CurCmd == "addAni")
					{
						if(m_mapActionDesc.count(m_strCmdParam.c_str()) == 0)
						{
							m_mapActionDesc[m_strCmdParam] = irr::core::vector3df(0,0,0);
							m_strActionList.push_back(m_strCmdParam);
							m_strwActionList.push_back(irr::core::stringw(m_strCmdParam.c_str()));

							m_ActionList.clear();

							u32 i;
							for(i=0;i<m_strActionList.size();i++)
							{
								m_ActionList.push_back((char *const)m_strActionList[i].c_str());
							}
							m_ActionList.push_back(NULL);
						}

						m_CurCmd = "---";
					}
				}
#endif

				/////////////////////////////////////////////////////
				//�������� ����Ÿ�ε�

				if(m_mapActionDesc.size())
				{
					m_CurAnimation = in->getAttributeAsEnumeration("animation");//,CFormatedAnimationNode::ActionNames);

					//�׼��������				
					{
						std::map<irr::core::stringc,irr::core::vector3df>::iterator it;
						for(it = m_mapActionDesc.begin();it !=m_mapActionDesc.end();it++)
						{
							it->second = in->getAttributeAsVector3d(it->first.c_str());						
						}
					}
				}

				m_TargetNodeName = in->getAttributeAsString("target");			
				changeAction(m_CurAnimation.c_str());
				ISceneNode::deserializeAttributes(in, options);				
			}

			void CFormatedAnimationNode::OnAnimate(irr::u32 timeMs)
			{
				ISceneNode::OnAnimate(timeMs);	

				/*irr::scene::IAnimatedMeshSceneNode *pAni = 
					(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
					m_TargetNodeName.c_str(),this);*/

				//pAni->animateJoints();

			}

			bool CFormatedAnimationNode::IsActionFinished()
			{
				irr::scene::IAnimatedMeshSceneNode *pAni = 
					(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
					m_TargetNodeName.c_str(),this);
				if(pAni->getEndFrame() == pAni->getFrameNr())
				{
					return true;
				}

				return false;
			}

			void CFormatedAnimationNode::changeAction(const irr::c8 *ActionName,bool bLoop)
			{
				irr::scene::IAnimatedMeshSceneNode *pAni = 
					(irr::scene::IAnimatedMeshSceneNode *)SceneManager->getSceneNodeFromName(
					m_TargetNodeName.c_str(),this);			

				
				if(pAni)
				{
					pAni->setLoopMode(bLoop);
					pAni->setAnimationSpeed(m_mapActionDesc[ActionName].Z);
					pAni->setFrameLoop(
						(irr::s32)m_mapActionDesc[ActionName].X,
						(irr::s32)m_mapActionDesc[ActionName].Y
						);
				}					
			}
		}
	}
}
