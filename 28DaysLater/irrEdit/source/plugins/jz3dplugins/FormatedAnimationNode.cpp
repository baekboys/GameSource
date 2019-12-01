#include "FormatedAnimationNode.h"

/*

이 예제를 통해 배울수있는것들에 대한 간략한언급..

1. 억지로 구현해본 메뉴처리
원 제작자는 의도 했는지는 모르겠지만
항상성기능중 enum 형을 이용해서 메뉴를 구현해서 일리히트에디터의 추가 기능에 대한 유아이의 가능성에 대해서 실험을 해봤다.

cmd라는 열거형항목을 만들어서 몇가지 동작을 추가했다.
joint 조인트노드 보여주기
addAni 새로운 애니메이션 추가하기

초기상태는 ---로있다가 드롭다운메뉴를 선택해주면 desierialze함수가 호출되면서
cmd 변수를 체크해서 해당하는 동작을 수행하고 cmd내용은 다시 ---으로 해놓으면 마치 해당버튼을 
클릭했을때(또는 드롭다운 메뉴선택 메씨지를 처리 하는 것과 유사한 동작을 구현할수있었다.

2.동적 데이터 확장 또는 형식 변경



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

				if(m_strActionList.size() < m_strwActionList.size()) //새로 로딩된상태
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

					//루프를 두개로 나눠논 이유는 무엇이냐면...
					//복사된 값이 아니라 단순 포인터값이므로 지역변수쓰면 안된다.
					for(i=0;i<m_strActionList.size();i++)
					{
						m_ActionList.push_back((char *const)m_strActionList[i].c_str());
					}
					m_ActionList.push_back(NULL); //종료 카드

				}

				//플러그인용 메뉴처리
				m_CurCmd = in->getAttributeAsEnumeration("cmd"); //커멘트 읽기
				m_strCmdParam = in->getAttributeAsString("cmd Param1");

#ifdef JZ3DPLUGINS_EXPORTS
				{
					//이부분은
					//게임프로그램상에서는 디파인문등으로 빼놓고 컴파일 하는게좋다.

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

						m_CurCmd = "---"; //메뉴를 처리하고 원상태로 되돌려놓는다.(일종에 클릭기능)
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
				//본격적인 데이타로딩

				if(m_mapActionDesc.size())
				{
					m_CurAnimation = in->getAttributeAsEnumeration("animation");//,CFormatedAnimationNode::ActionNames);

					//액션정보얻기				
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
