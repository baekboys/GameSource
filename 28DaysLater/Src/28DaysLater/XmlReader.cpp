#include "XmlReader.h"

CXmlReader::CXmlReader(void)
{
}

CXmlReader::~CXmlReader(void)
{
}

bool CXmlReader::returnTrueOrNot(std::string value)
{
	if(value == "true")
		return true;
	
	return false;
}

bool CXmlReader::readStageDATA(const char* filename, StageDATA &stageData)
{
	TiXmlDocument doc(filename);

	return true;
}

bool CXmlReader::readSystemDATA(const char* filename, SystemDATA &systemData)
{
	TiXmlDocument doc(filename);

	// XML파일 로딩
	if(!doc.LoadFile())
	{
		return false;
	}
	else
	{
		// 임시로 저장할 변수 선언
		std::string strTemp;
		ggf::parser::VectorInt vi;
		ggf::parser::VectorFloat vf;
		ggf::parser::VectorString vs;

		// 엘리먼트 탐색을 위한 변수 선언
		TiXmlNode *pRoot;	
		TiXmlElement *pElement;

		// 루트엘리먼트 얻어오기
		pRoot = doc.RootElement();

		//----------------
		// 시스템 모드설정
		//----------------
		{
			// 엘리먼트 얻어오기
			pElement = pRoot->FirstChildElement("mode");

			// 컴파일만 할것인가?
			systemData.onlyCompile = returnTrueOrNot(pElement->Attribute("onlyCompile"));

			// 컴파일모드이면 false반환
			if ( systemData.onlyCompile )
			{
				systemData.gameInit = false;
				return false;
			}

			// 겜임을 초기화를 할것인가?
			systemData.gameInit = returnTrueOrNot(pElement->Attribute("gameInit"));

			// 콘솔 명령어를 이용할 것인가?
			systemData.consoleCMD = returnTrueOrNot(pElement->Attribute("consoleCMD"));
		}


		//-----------------------
		// 일리히트 디바이스 설정
		//-----------------------
		{
			// 엘리먼트 얻어오기
			pElement = pElement->NextSiblingElement();

			// 드라이버 타입 설정
			const std::string type(pElement->Attribute("driverType"));

			if  (type == "EDT_SOFTWARE")
				systemData.deviceType = irr::video::EDT_SOFTWARE;
			else if  (type == "EDT_BURNINGSVIDEO")
				systemData.deviceType = irr::video::EDT_BURNINGSVIDEO;
			else if  (type == "EDT_DIRECT3D8")
				systemData.deviceType = irr::video::EDT_DIRECT3D8;
			else if  (type == "EDT_DIRECT3D9")
				systemData.deviceType = irr::video::EDT_DIRECT3D9;
			else if  (type == "EDT_OPENGL")
				systemData.deviceType = irr::video::EDT_OPENGL;
			else
				systemData.deviceType = irr::video::EDT_NULL;

			// 스크린 사이즈 설정
			ggf::parser::GetAttrib(pElement->Attribute("screenSize"), vi);
			systemData.screen_size[0] = vi[0];
			systemData.screen_size[1] = vi[1];

			// 비트수
			systemData.bits = atoi(pElement->Attribute("bits"));

			// 풀스크린모드 설정
			systemData.full_screen = returnTrueOrNot(pElement->Attribute("fullScreen"));

			// 스텐실 버퍼
			systemData.stencilBuffer = returnTrueOrNot(pElement->Attribute("stencilBuffer"));

			// 고정프레임
			systemData.vsync = returnTrueOrNot(pElement->Attribute("vsync"));

			// 마우스 커서 보이기
			systemData.mouse_visible = returnTrueOrNot(pElement->Attribute("mouseVisible"));

			// 작업 디렉토리 설정
			systemData.working_directory = pElement->Attribute("workingDirectory");
		}


		//----------
		// irrklang
		//----------
		{
			// 엘리먼트 얻어오기
			pElement = pElement->NextSiblingElement();

			// 3DSoundMinDistance
			systemData.soundMinDistance = atof(pElement->Attribute("soundMinDistance"));
		}


		//-----------
		// XML files
		//-----------
		{
			// 엘리먼트 얻어오기
			pElement = pElement->NextSiblingElement();

			// 캐릭터 XML데이터 파일명
			systemData.heroXML = pElement->Attribute("herofileName");

			// 좀비 XML데이터 파일명
			systemData.zombieXML = pElement->Attribute("zombiefileName");

			// HL2좀비 XML데이터 파일명
			systemData.HL2zombieXML = pElement->Attribute("HL2zombiefileName");

			// 플라즈마라이플 XML데이터 파일명
			systemData.plasmarifleXML = pElement->Attribute("plasmariflefileName");

			// 파티클 XML데이터 파일명
			systemData.particleXML = pElement->Attribute("particlefileName");

			// 스테이지 XML데이터 파일명
			systemData.stageXML = pElement->Attribute("stagefileName");

			// 사운드 XML데이터 파일명
			systemData.soundfileXML = pElement->Attribute("soundfileName");

			// 이미지 XML데이터 파일명
			systemData.imagefileXML = pElement->Attribute("imagefileName");
		}

		//--------------------
		// 일리히트씬 파일명 
		//--------------------
		{
			// 엘리먼트 얻어오기
			pElement = pElement->NextSiblingElement();

			// 캐릭터 신 파일명
			systemData.hero_fileName = pElement->Attribute("heroSceneFileName");
			// 캐릭터 신노드 이름
			systemData.hero_SceneName = pElement->Attribute("heroSceneName");
			
			// 스테이지 신 파일명
			systemData.stage_fileName = pElement->Attribute("stageSceneFileName");
			// 스테이지 신노드 이름
			systemData.stage_SceneName = pElement->Attribute("stageSceneName");
			// 스테이지 물리지형 이름
			systemData.stage_physicName = pElement->Attribute("stagePhysicName");
			
			// 좀비 신 파일명
			systemData.zombie_fileName = pElement->Attribute("zombieSceneFileName");
			// 좀비 신노드 이름
			systemData.zombie_SceneName = pElement->Attribute("zombieSceneName");

			// HL2좀비 신 파일명
			systemData.HL2zombie_fileName = pElement->Attribute("HL2zombieSceneFileName");
			// HL2좀비 신노드 이름
			systemData.HL2zombie_SceneName = pElement->Attribute("HL2zombieSceneName");

			// 플라즈마 라이플 신 파일 명
			systemData.plasmarifle_fileName = pElement->Attribute("plasmarifleSceneFileName");
			// 플라즈마 라이플 신노드 이름
			systemData.plasmarifle_SceneName = pElement->Attribute("plasmarifleSceneName");
		}

		//----------------
		// 게임 데이터
		//----------------
		{
			// 엘리먼트 얻어오기
			pElement = pElement->NextSiblingElement();

			// 엘리먼트가 있으면 실시
			if (pElement)
			{
				// 첫번째 attribute 얻어오기
				TiXmlAttribute* attribute = pElement->FirstAttribute();

				// attribute가 NULL이 아닐때 까지 벡터spec에 저장
				while (attribute != NULL)
				{			
					irr::s32 value = atoi(attribute->Value());
					systemData.data.push_back(value);
					attribute = attribute->Next();
				}
			}
			// 없으면 종료
			else
				return true;
		}

	}

	return true;
}

bool CXmlReader::readObjectData(const char *fileName, ObjectData &objectData)
{
	TiXmlDocument doc(fileName);
	
	// XML파일 로딩
	if(!doc.LoadFile())
	{
		//std::cout << "로딩 실패 : XML파일 형식을 다시한번 확인하세요." << std::endl;
		return false;
	}
	else
	{
		//std::cout << "XML파일 로딩 성공" << std::endl;
	}


	// 임시로 저장할 변수 선언
	std::string strTemp;
	ggf::parser::VectorInt vi;
	ggf::parser::VectorFloat vf;
	ggf::parser::VectorString vs;

	// 엘리먼트 탐색을 위한 변수 선언
	TiXmlNode *pRoot;	
	TiXmlElement *pElement;

	// 루트엘리먼트 얻어오기
	pRoot = doc.RootElement();


	//----------------
	// 오브젝트의 스펙
	//----------------
	{
		// 엘리먼트 얻어오기
		pElement = pRoot->FirstChildElement("spec");

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			// 첫번째 attribute 얻어오기
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute가 NULL이 아닐때 까지 벡터spec에 저장
			while (attribute != NULL)
			{			
				irr::f32 value = atof(attribute->Value());
				objectData.spec.push_back(value);
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}

	//------------------------
	// 오브젝트의 물리파라미터
	//------------------------
	{
		
		// 엘리먼트 얻어오기
		pElement = pElement->NextSiblingElement();

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute가 NULL이 아닐때 까지 벡터physics에 저장
			while (attribute != NULL)
			{			
				irr::f32 value = atof(attribute->Value());
				objectData.physics.push_back(value);
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}

	//--------------------------
	// 오브젝트의 문자열파라미터
	//--------------------------
	{
		// 엘리먼트 얻어오기
		pElement = pElement->NextSiblingElement();

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute가 NULL이 아닐때 까지 벡터str에 저장
			while (attribute != NULL)
			{			
				std::string value = attribute->Value();
				objectData.str.push_back(value);
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}

	//---------------------------
	// 오브젝트의 boolean파라미터
	//---------------------------
	{
		// 엘리먼트 얻어오기
		pElement = pElement->NextSiblingElement();

		// 엘리먼트가 있으면 실시
		if ( pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute가 NULL이 아닐때 까지 벡터str에 저장
			while (attribute != NULL)
			{			
				bool value = returnTrueOrNot(attribute->Value());
				objectData.boolean.push_back(value);
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}

	return true;
}

bool CXmlReader::readSoundFileDATA(const char* fileName, SoundFileListDATA &soundData)
{
	TiXmlDocument doc(fileName);
	
	// XML파일 로딩
	if(!doc.LoadFile())
	{
		//std::cout << "로딩 실패 : XML파일 형식을 다시한번 확인하세요." << std::endl;
		return false;
	}
	else
	{
		//std::cout << "XML파일 로딩 성공" << std::endl;
	}


	// 임시로 저장할 변수 선언
	std::string strTemp;
	ggf::parser::VectorInt vi;
	ggf::parser::VectorFloat vf;
	ggf::parser::VectorString vs;

	// 엘리먼트 탐색을 위한 변수 선언
	TiXmlNode *pRoot;	
	TiXmlElement *pElement;

	// 루트엘리먼트 얻어오기
	pRoot = doc.RootElement();


	//--------------------
	// 주인공 사운드리스트
	//--------------------
	{
		// 엘리먼트 얻어오기
		pElement = pRoot->FirstChildElement("hero");

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			// 첫번째 attribute 얻어오기
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// 첫번째는 디렉토리명을 의미함.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute가 NULL이 아닐때 까지 벡터spec에 저장
			while (attribute != NULL)
			{
				// 파일명 얻어오기
				std::string file(attribute->Value());
				
				// (디렉토리 + 파일명)을 저장.
				soundData.heroList.push_back(directory + file);

				// 다음 attribute
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}


	//----------------------------
	// 플라즈마라이플 사운드리스트
	//----------------------------
	{
		// 엘리먼트 얻어오기
		pElement = pElement->NextSiblingElement();

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			// 첫번째 attribute 얻어오기
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// 첫번째는 디렉토리명을 의미함.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute가 NULL이 아닐때 까지 벡터spec에 저장
			while (attribute != NULL)
			{
				// 파일명 얻어오기
				std::string file(attribute->Value());
				
				// (디렉토리 + 파일명)을 저장.
				soundData.plasmarifleList.push_back(directory + file);

				// 다음 attribute
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}


	//--------------------
	// 좀비 사운드리스트
	//--------------------
	{
		// 엘리먼트 얻어오기
		pElement = pElement->NextSiblingElement();

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			// 첫번째 attribute 얻어오기
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// 첫번째는 디렉토리명을 의미함.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute가 NULL이 아닐때 까지 벡터spec에 저장
			while (attribute != NULL)
			{
				// 파일명 얻어오기
				std::string file(attribute->Value());
				
				// (디렉토리 + 파일명)을 저장.
				soundData.zombieList.push_back(directory + file);

				// 다음 attribute
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}

	//--------------------
	// 좀비 사운드리스트
	//--------------------
	{
		// 엘리먼트 얻어오기
		pElement = pElement->NextSiblingElement();

		// 엘리먼트가 있으면 실시
		if (pElement)
		{
			// 첫번째 attribute 얻어오기
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// 첫번째는 디렉토리명을 의미함.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute가 NULL이 아닐때 까지 벡터spec에 저장
			while (attribute != NULL)
			{
				// 파일명 얻어오기
				std::string file(attribute->Value());

				// (디렉토리 + 파일명)을 저장.
				soundData.stageList.push_back(directory + file);

				// 다음 attribute
				attribute = attribute->Next();
			}
		}
		// 없으면 종료
		else
			return true;
	}

	return true;
}