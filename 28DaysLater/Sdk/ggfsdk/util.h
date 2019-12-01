#pragma once


namespace ggf {
	namespace util {


		inline void RecursiveEvent(irr::scene::ISceneNode *pNode, const irr::SEvent &event)
		{			
			irr::core::list<irr::scene::ISceneNodeAnimator*> Animators = pNode->getAnimators();

			irr::core::list<irr::scene::ISceneNodeAnimator*>::Iterator ait = Animators.begin();
			for (; ait != Animators.end(); ++ait)
			{
				if((*ait)->isEventReceiverEnabled())
				{
					(*ait)->OnEvent(event);
				}
			}

			irr::core::list<irr::scene::ISceneNode*> Children = pNode->getChildren();

			irr::core::list<irr::scene::ISceneNode*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)	
			{
				RecursiveEvent((*it),event);				
			}
		}



		// 인자로 넘겨준 애니메이터를 만들수있는 씬노드 애니메이터펙토리를 찾는다.
		inline irr::scene::ISceneNodeAnimatorFactory *findAnimatorFactory(scene::ISceneManager *SceneManager, irr::s32 typeID)
		{
			u32 i;				

			for(i=0;i<SceneManager->getRegisteredSceneNodeAnimatorFactoryCount();i++)
			{
				irr::scene::ISceneNodeAnimatorFactory *factory = SceneManager->getSceneNodeAnimatorFactory(i);

				u32 j;
				for(j=0;j< factory->getCreatableSceneNodeAnimatorTypeCount();j++)
				{
					if(factory->getCreateableSceneNodeAnimatorType(i) == typeID)
					{
						return factory;						
					}
				}
			}
			return NULL;
		}	

	}
}
