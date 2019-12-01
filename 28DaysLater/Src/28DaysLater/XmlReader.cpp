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

	// XML���� �ε�
	if(!doc.LoadFile())
	{
		return false;
	}
	else
	{
		// �ӽ÷� ������ ���� ����
		std::string strTemp;
		ggf::parser::VectorInt vi;
		ggf::parser::VectorFloat vf;
		ggf::parser::VectorString vs;

		// ������Ʈ Ž���� ���� ���� ����
		TiXmlNode *pRoot;	
		TiXmlElement *pElement;

		// ��Ʈ������Ʈ ������
		pRoot = doc.RootElement();

		//----------------
		// �ý��� ��弳��
		//----------------
		{
			// ������Ʈ ������
			pElement = pRoot->FirstChildElement("mode");

			// �����ϸ� �Ұ��ΰ�?
			systemData.onlyCompile = returnTrueOrNot(pElement->Attribute("onlyCompile"));

			// �����ϸ���̸� false��ȯ
			if ( systemData.onlyCompile )
			{
				systemData.gameInit = false;
				return false;
			}

			// ������ �ʱ�ȭ�� �Ұ��ΰ�?
			systemData.gameInit = returnTrueOrNot(pElement->Attribute("gameInit"));

			// �ܼ� ��ɾ �̿��� ���ΰ�?
			systemData.consoleCMD = returnTrueOrNot(pElement->Attribute("consoleCMD"));
		}


		//-----------------------
		// �ϸ���Ʈ ����̽� ����
		//-----------------------
		{
			// ������Ʈ ������
			pElement = pElement->NextSiblingElement();

			// ����̹� Ÿ�� ����
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

			// ��ũ�� ������ ����
			ggf::parser::GetAttrib(pElement->Attribute("screenSize"), vi);
			systemData.screen_size[0] = vi[0];
			systemData.screen_size[1] = vi[1];

			// ��Ʈ��
			systemData.bits = atoi(pElement->Attribute("bits"));

			// Ǯ��ũ����� ����
			systemData.full_screen = returnTrueOrNot(pElement->Attribute("fullScreen"));

			// ���ٽ� ����
			systemData.stencilBuffer = returnTrueOrNot(pElement->Attribute("stencilBuffer"));

			// ����������
			systemData.vsync = returnTrueOrNot(pElement->Attribute("vsync"));

			// ���콺 Ŀ�� ���̱�
			systemData.mouse_visible = returnTrueOrNot(pElement->Attribute("mouseVisible"));

			// �۾� ���丮 ����
			systemData.working_directory = pElement->Attribute("workingDirectory");
		}


		//----------
		// irrklang
		//----------
		{
			// ������Ʈ ������
			pElement = pElement->NextSiblingElement();

			// 3DSoundMinDistance
			systemData.soundMinDistance = atof(pElement->Attribute("soundMinDistance"));
		}


		//-----------
		// XML files
		//-----------
		{
			// ������Ʈ ������
			pElement = pElement->NextSiblingElement();

			// ĳ���� XML������ ���ϸ�
			systemData.heroXML = pElement->Attribute("herofileName");

			// ���� XML������ ���ϸ�
			systemData.zombieXML = pElement->Attribute("zombiefileName");

			// HL2���� XML������ ���ϸ�
			systemData.HL2zombieXML = pElement->Attribute("HL2zombiefileName");

			// �ö�������� XML������ ���ϸ�
			systemData.plasmarifleXML = pElement->Attribute("plasmariflefileName");

			// ��ƼŬ XML������ ���ϸ�
			systemData.particleXML = pElement->Attribute("particlefileName");

			// �������� XML������ ���ϸ�
			systemData.stageXML = pElement->Attribute("stagefileName");

			// ���� XML������ ���ϸ�
			systemData.soundfileXML = pElement->Attribute("soundfileName");

			// �̹��� XML������ ���ϸ�
			systemData.imagefileXML = pElement->Attribute("imagefileName");
		}

		//--------------------
		// �ϸ���Ʈ�� ���ϸ� 
		//--------------------
		{
			// ������Ʈ ������
			pElement = pElement->NextSiblingElement();

			// ĳ���� �� ���ϸ�
			systemData.hero_fileName = pElement->Attribute("heroSceneFileName");
			// ĳ���� �ų�� �̸�
			systemData.hero_SceneName = pElement->Attribute("heroSceneName");
			
			// �������� �� ���ϸ�
			systemData.stage_fileName = pElement->Attribute("stageSceneFileName");
			// �������� �ų�� �̸�
			systemData.stage_SceneName = pElement->Attribute("stageSceneName");
			// �������� �������� �̸�
			systemData.stage_physicName = pElement->Attribute("stagePhysicName");
			
			// ���� �� ���ϸ�
			systemData.zombie_fileName = pElement->Attribute("zombieSceneFileName");
			// ���� �ų�� �̸�
			systemData.zombie_SceneName = pElement->Attribute("zombieSceneName");

			// HL2���� �� ���ϸ�
			systemData.HL2zombie_fileName = pElement->Attribute("HL2zombieSceneFileName");
			// HL2���� �ų�� �̸�
			systemData.HL2zombie_SceneName = pElement->Attribute("HL2zombieSceneName");

			// �ö�� ������ �� ���� ��
			systemData.plasmarifle_fileName = pElement->Attribute("plasmarifleSceneFileName");
			// �ö�� ������ �ų�� �̸�
			systemData.plasmarifle_SceneName = pElement->Attribute("plasmarifleSceneName");
		}

		//----------------
		// ���� ������
		//----------------
		{
			// ������Ʈ ������
			pElement = pElement->NextSiblingElement();

			// ������Ʈ�� ������ �ǽ�
			if (pElement)
			{
				// ù��° attribute ������
				TiXmlAttribute* attribute = pElement->FirstAttribute();

				// attribute�� NULL�� �ƴҶ� ���� ����spec�� ����
				while (attribute != NULL)
				{			
					irr::s32 value = atoi(attribute->Value());
					systemData.data.push_back(value);
					attribute = attribute->Next();
				}
			}
			// ������ ����
			else
				return true;
		}

	}

	return true;
}

bool CXmlReader::readObjectData(const char *fileName, ObjectData &objectData)
{
	TiXmlDocument doc(fileName);
	
	// XML���� �ε�
	if(!doc.LoadFile())
	{
		//std::cout << "�ε� ���� : XML���� ������ �ٽ��ѹ� Ȯ���ϼ���." << std::endl;
		return false;
	}
	else
	{
		//std::cout << "XML���� �ε� ����" << std::endl;
	}


	// �ӽ÷� ������ ���� ����
	std::string strTemp;
	ggf::parser::VectorInt vi;
	ggf::parser::VectorFloat vf;
	ggf::parser::VectorString vs;

	// ������Ʈ Ž���� ���� ���� ����
	TiXmlNode *pRoot;	
	TiXmlElement *pElement;

	// ��Ʈ������Ʈ ������
	pRoot = doc.RootElement();


	//----------------
	// ������Ʈ�� ����
	//----------------
	{
		// ������Ʈ ������
		pElement = pRoot->FirstChildElement("spec");

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			// ù��° attribute ������
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute�� NULL�� �ƴҶ� ���� ����spec�� ����
			while (attribute != NULL)
			{			
				irr::f32 value = atof(attribute->Value());
				objectData.spec.push_back(value);
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}

	//------------------------
	// ������Ʈ�� �����Ķ����
	//------------------------
	{
		
		// ������Ʈ ������
		pElement = pElement->NextSiblingElement();

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute�� NULL�� �ƴҶ� ���� ����physics�� ����
			while (attribute != NULL)
			{			
				irr::f32 value = atof(attribute->Value());
				objectData.physics.push_back(value);
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}

	//--------------------------
	// ������Ʈ�� ���ڿ��Ķ����
	//--------------------------
	{
		// ������Ʈ ������
		pElement = pElement->NextSiblingElement();

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute�� NULL�� �ƴҶ� ���� ����str�� ����
			while (attribute != NULL)
			{			
				std::string value = attribute->Value();
				objectData.str.push_back(value);
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}

	//---------------------------
	// ������Ʈ�� boolean�Ķ����
	//---------------------------
	{
		// ������Ʈ ������
		pElement = pElement->NextSiblingElement();

		// ������Ʈ�� ������ �ǽ�
		if ( pElement)
		{
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// attribute�� NULL�� �ƴҶ� ���� ����str�� ����
			while (attribute != NULL)
			{			
				bool value = returnTrueOrNot(attribute->Value());
				objectData.boolean.push_back(value);
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}

	return true;
}

bool CXmlReader::readSoundFileDATA(const char* fileName, SoundFileListDATA &soundData)
{
	TiXmlDocument doc(fileName);
	
	// XML���� �ε�
	if(!doc.LoadFile())
	{
		//std::cout << "�ε� ���� : XML���� ������ �ٽ��ѹ� Ȯ���ϼ���." << std::endl;
		return false;
	}
	else
	{
		//std::cout << "XML���� �ε� ����" << std::endl;
	}


	// �ӽ÷� ������ ���� ����
	std::string strTemp;
	ggf::parser::VectorInt vi;
	ggf::parser::VectorFloat vf;
	ggf::parser::VectorString vs;

	// ������Ʈ Ž���� ���� ���� ����
	TiXmlNode *pRoot;	
	TiXmlElement *pElement;

	// ��Ʈ������Ʈ ������
	pRoot = doc.RootElement();


	//--------------------
	// ���ΰ� ���帮��Ʈ
	//--------------------
	{
		// ������Ʈ ������
		pElement = pRoot->FirstChildElement("hero");

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			// ù��° attribute ������
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// ù��°�� ���丮���� �ǹ���.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute�� NULL�� �ƴҶ� ���� ����spec�� ����
			while (attribute != NULL)
			{
				// ���ϸ� ������
				std::string file(attribute->Value());
				
				// (���丮 + ���ϸ�)�� ����.
				soundData.heroList.push_back(directory + file);

				// ���� attribute
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}


	//----------------------------
	// �ö�������� ���帮��Ʈ
	//----------------------------
	{
		// ������Ʈ ������
		pElement = pElement->NextSiblingElement();

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			// ù��° attribute ������
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// ù��°�� ���丮���� �ǹ���.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute�� NULL�� �ƴҶ� ���� ����spec�� ����
			while (attribute != NULL)
			{
				// ���ϸ� ������
				std::string file(attribute->Value());
				
				// (���丮 + ���ϸ�)�� ����.
				soundData.plasmarifleList.push_back(directory + file);

				// ���� attribute
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}


	//--------------------
	// ���� ���帮��Ʈ
	//--------------------
	{
		// ������Ʈ ������
		pElement = pElement->NextSiblingElement();

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			// ù��° attribute ������
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// ù��°�� ���丮���� �ǹ���.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute�� NULL�� �ƴҶ� ���� ����spec�� ����
			while (attribute != NULL)
			{
				// ���ϸ� ������
				std::string file(attribute->Value());
				
				// (���丮 + ���ϸ�)�� ����.
				soundData.zombieList.push_back(directory + file);

				// ���� attribute
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}

	//--------------------
	// ���� ���帮��Ʈ
	//--------------------
	{
		// ������Ʈ ������
		pElement = pElement->NextSiblingElement();

		// ������Ʈ�� ������ �ǽ�
		if (pElement)
		{
			// ù��° attribute ������
			TiXmlAttribute* attribute = pElement->FirstAttribute();

			// ù��°�� ���丮���� �ǹ���.
			std::string directory(attribute->Value());
			attribute = attribute->Next();

			// attribute�� NULL�� �ƴҶ� ���� ����spec�� ����
			while (attribute != NULL)
			{
				// ���ϸ� ������
				std::string file(attribute->Value());

				// (���丮 + ���ϸ�)�� ����.
				soundData.stageList.push_back(directory + file);

				// ���� attribute
				attribute = attribute->Next();
			}
		}
		// ������ ����
		else
			return true;
	}

	return true;
}