#include "IrrXMLReader.h"

CIrrXMLReader::CIrrXMLReader(void)
{
}

CIrrXMLReader::~CIrrXMLReader(void)
{
}

bool CIrrXMLReader::LoadToFloatArrayDataFromXMLFile(const char* fileName, irr::core::array<irr::f32> &fData, char* fDataNodeName)
{
	IrrXMLReader* xml = createIrrXMLReader(fileName);

	if(!xml)
	{
		return false;
	}

	irr::core::stringc fDataName(fDataNodeName);
	
	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				irr::u32 count = xml->getAttributeCount();

				if( fDataName == irr::core::stringc(xml->getNodeName()) )
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						fData.push_back(xml->getAttributeValueAsFloat(i));
					}
				}
			}
			break;
		}
	}

	// delete the xml parser after usage
	delete xml;

	return true;
}

bool CIrrXMLReader::LoadToStringArrayDataFromXMLFile(const char* fileName, irr::core::array<irr::core::stringc> &cData, char* cDataNodeName)
{
	IrrXMLReader* xml = createIrrXMLReader(fileName);

	if(!xml)
	{
		return false;
	}

	irr::core::stringc cDataName(cDataNodeName);

	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				irr::u32 count = xml->getAttributeCount();

				if( cDataName == irr::core::stringc(xml->getNodeName()) )
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						cData.push_back(xml->getAttributeValue(i));
					}
				}
			}
			break;
		}
	}

	// delete the xml parser after usage
	delete xml;

	return true;
}

bool CIrrXMLReader::LoadToStringArrayDataFromXMLFileNotNodeName(const char* fileName, irr::core::array<irr::core::stringc> &cData, irr::u32 &nodeCount, irr::u32 &attributeCount)
{
	IrrXMLReader* xml = createIrrXMLReader(fileName);

	if(!xml)
	{
		return false;
	}

	irr::u32 count;

	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				count = xml->getAttributeCount();

				if (count > 0)
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						cData.push_back( xml->getAttributeValue(i) );
					}
					nodeCount++;
				}
			}
			break;
		}
	}

	attributeCount = count;

	// delete the xml parser after usage
	delete xml;

	return true;
}

bool CIrrXMLReader::LoadToFloatStringArrayDataFromXMLFile(const char* fileName, irr::core::array<irr::f32> &fData, irr::core::array<irr::core::stringc> &cData)
{
	IrrXMLReader* xml = createIrrXMLReader(fileName);

	if(!xml)
	{
		return false;
	}

	irr::core::stringc fDataName("fParam");
	irr::core::stringc cDataName("cParam");

	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				irr::u32 count = xml->getAttributeCount();

				if( fDataName == irr::core::stringc(xml->getNodeName()) )
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						fData.push_back(xml->getAttributeValueAsFloat(i));
					}
				}

				if( cDataName == irr::core::stringc(xml->getNodeName()) )
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						cData.push_back( xml->getAttributeValue(i) );
					}
				}
			}
			break;
		}
	}

	// delete the xml parser after usage
	delete xml;

	return true;
}

bool CIrrXMLReader::LoadToFloatStringArrayDataFromXMLFile(
	const char* fileName, 
	irr::core::array<irr::f32> &fData, char* fDataNodeName,
	irr::core::array<irr::core::stringc> &cData, char* cDataNodeName
	)
{
	IrrXMLReader* xml = createIrrXMLReader(fileName);

	if(!xml)
	{
		return false;
	}

	irr::core::stringc fDataName(fDataNodeName);
	irr::core::stringc cDataName(cDataNodeName);

	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				irr::u32 count = xml->getAttributeCount();

				if( fDataName == irr::core::stringc(xml->getNodeName()) )
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						fData.push_back(xml->getAttributeValueAsFloat(i));
					}
				}

				if( cDataName == irr::core::stringc(xml->getNodeName()) )
				{
					irr::u32 i;
					for(i = 0 ; i < count ; i++)
					{
						cData.push_back( xml->getAttributeValue(i) );
					}
				}
			}
			break;
		}
	}

	// delete the xml parser after usage
	delete xml;

	return true;
}