#pragma once

#include <irrlicht.h>

namespace irr
{
namespace scene
{
namespace jz3d
{
	class CylinderSceneNode : public ISceneNode
	{
		irr::core::aabbox3df m_Box;

		// 원의 정점갯수
		int m_DivideNumber;
	public :
		static const int SN_ID = MAKE_IRR_ID('c','y','n','d');
		static char *Name;
		CylinderSceneNode(ISceneNode *parent, ISceneManager *mgr,s32 id)
			: ISceneNode(parent,mgr,id),
			m_DivideNumber(20)
		{
			// 초기화코드

			// 바운딩박스 초기화
			m_Box.MaxEdge = irr::core::vector3df(5,5,5);
			m_Box.MinEdge = irr::core::vector3df(-5,-5,-5);
			
			// 매트리얼 설정
			irr::video::SMaterial m;
			m.Lighting = false;
			m.BackfaceCulling = false;

			irr::video::SMaterial mt;

			// 0번 매트리얼은 m, 나머지는 mt로 초기화
			m_Matrial.push_back(m);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);
			m_Matrial.push_back(mt);					
		}

		inline void OnRegisterSceneNode()
		{
			if(IsVisible)
			{
				SceneManager->registerNodeForRendering(this);
			}

			ISceneNode::OnRegisterSceneNode();

		}

		inline void render()
		{
			irr::video::IVideoDriver *driver = SceneManager->getVideoDriver();

			// 매트리얼을 0번으로 사용
			driver->setMaterial(m_Matrial[0]);

			driver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);

			// 원의 정점 갯수
			int count = m_DivideNumber;

			// 버텍스의 갯수 : 원의 정점갯수 * 2개(위-아래) + 2개(0, 1번 버텍스와 동일)
			irr::video::S3DVertex *Vertex = new irr::video::S3DVertex[count*2+2];


			// 원의 정점 갯수만큼 반복
			for(int i=0;i<count ; i++)
			{
				// 각도 구하기
				float theta = (2*irr::core::PI * i) / (count);

				// 상단 원의 버텍스
				Vertex[2*i+0] = irr::video::S3DVertex(m_Box.MaxEdge.X*cosf(theta),m_Box.MinEdge.Y,m_Box.MaxEdge.Z*sinf(theta),0,0,-1,video::SColor(255,255,255,255), (f32)i/ (f32)count+1, 1);

				// 하단 원의 버텍스
				Vertex[2*i+1] = irr::video::S3DVertex(m_Box.MaxEdge.X*cosf(theta),m_Box.MaxEdge.Y,m_Box.MaxEdge.Z*sinf(theta),0,0,-1,video::SColor(255,255,255,255), (f32)i/ (f32)count+1, 0);
			}


			// 마지막 2개의 버텍스는 0,1번의 버텍스와 위치좌표가 동일
			Vertex[count*2] = Vertex[0];
			Vertex[count*2+1] = Vertex[1];

			// 마지막 2개의 버텍스는 U,V좌표는 다르기 때문에 직접 설정
			Vertex[count*2].TCoords = irr::core::vector2df((f32)count /(f32) count+1, 1);
			Vertex[count*2+1].TCoords = irr::core::vector2df((f32)count/(f32) count+1, 0);


			// 인덱스 생성 : 원의 정점 갯수 * 2개(상하단) + 2개(0,1번과 동일한 버텍스)
			u16 *indices = new u16[count*2+2];

			for(int i=0 ; i<count * 2 ; i++)
			{
				indices[i] = i;
			}
			indices[count*2] = count*2;
			indices[count*2+1] = count*2+1;

			// 인덱스를 이용하여 그리기
			driver->drawVertexPrimitiveList(Vertex,count*2+2,indices,count*2,irr::video::EVT_STANDARD,irr::scene::EPT_TRIANGLE_STRIP,irr::video::EIT_16BIT);

			delete Vertex;
			delete indices;
		}

		const irr::core::aabbox3df& getBoundingBox() const
		{
			return m_Box;
		}

		ESCENE_NODE_TYPE getType() const 
		{
			return (ESCENE_NODE_TYPE)SN_ID;
		}

		inline void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions *options) const
		{					
			ISceneNode::serializeAttributes(out,options);

			out->addVector3d("BoundingBox SIZE",m_Box.getExtent());
			out->addInt("Divided-number",m_DivideNumber);

		}

		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions *options)
		{
			irr::core::vector3df box_ext = in->getAttributeAsVector3d("BoundingBox SIZE");
			m_DivideNumber = in->getAttributeAsInt("Divided-number");
			m_Box = irr::core::aabbox3df(-box_ext/2.f,box_ext/2.f);

			ISceneNode::deserializeAttributes(in,options);				
		}

		virtual ISceneNode* clone(ISceneNode *newParent=0, ISceneManager* newManager=0)
		{
			if(!newParent)
			{
				newParent = Parent;
			}
			if(!newManager)
			{
				newManager = SceneManager;
			}
			CylinderSceneNode *cloneNode = new CylinderSceneNode(newParent,
				newManager,
				ID) ;

			cloneNode->cloneMembers(this,newManager);

			cloneNode->m_Box = m_Box;
			cloneNode->m_DivideNumber = m_DivideNumber;

			cloneNode->getMaterial(0) = this->getMaterial(0);

			cloneNode->drop();

			return cloneNode;
		}

		irr::core::array<irr::video::SMaterial> m_Matrial;


		virtual video::SMaterial& getMaterial(u32 num)
		{
			return m_Matrial[num];
		}

		virtual u32 getMaterialCount() const
		{
			return m_Matrial.size();
		}
	};

	/*class CylinderSceneFactory : public irr::scene::ISceneNodeFactory
	{
		static const int SN_ID = MAKE_IRR_ID('c','y','n','d');
		static char *Name;

		irr::scene::ISceneManager *SceneManager;
	public :
		CylinderSceneFactory(irr::scene::ISceneManager *mgr)
			: SceneManager(mgr)
		{
		}

		virtual irr::scene::ISceneNode *addSceneNode(irr::scene::ESCENE_NODE_TYPE type,irr::scene::ISceneNode *parent = 0)
		{
			return 0;
		}

		virtual ISceneNode* addSceneNode(const c8* typeName, ISceneNode* parent=0)
		{
			if(parent==0)
			{
				parent = SceneManager->getRootSceneNode();
			}

			if(!strcmp(typeName,CylinderSceneNode::Name))
			{
				CylinderSceneNode *pNode = new CylinderSceneNode(
					parent,SceneManager,-1);
				pNode->drop();

				return pNode;
			}
			return 0;
		}

		virtual u32 getCreatableSceneNodeTypeCount() const
		{
			return 1;
		}

		virtual ESCENE_NODE_TYPE getCreateableSceneNodeType(u32 idx) const
		{

			return (ESCENE_NODE_TYPE)CylinderSceneNode::SN_ID;

		}

		virtual const c8* getCreateableSceneNodeTypeName(u32 idx) const
		{

			return CylinderSceneNode::Name;
		}

		virtual const c8* getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
		{
			if(type == SN_ID)
			{
				return CylinderSceneNode::Name;
			}

			return 0;
		}
	};*/
}
}
}

//#include "CylinderSceneNode.h"
//
//__declspec(dllexport) irr::irredit::IIrrEditPlugin* __stdcall createPlugin( irr::irredit::IrrEditServices* services )
//{
//
//	irr::scene::ISceneManager* mgr = services->getDevice()->getSceneManager();
//
//	services->getDevice()->getLogger()->log("CylinderPlugin\n");
//
//	irr::scene::CylinderSceneFactory *factory = new irr::scene::CylinderSceneFactory(mgr);
//	mgr->registerSceneNodeFactory(factory);
//	factory->drop();
//	return 0;
//}