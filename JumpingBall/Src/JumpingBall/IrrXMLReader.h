#ifndef IRR_XML_READER_H
#define IRR_XML_READER_H

#include "irrlicht.h"

using namespace irr;
using namespace io;

class CIrrXMLReader
{
public:
	CIrrXMLReader(void);
	~CIrrXMLReader(void);

	bool LoadToFloatArrayDataFromXMLFile(const char* fileName, irr::core::array<irr::f32> &fData, char* fDataNodeName);
	bool LoadToStringArrayDataFromXMLFile(const char* fileName, irr::core::array<irr::core::stringc> &cData, char* cDataNodeName);
	bool LoadToStringArrayDataFromXMLFileNotNodeName(const char* fileName, irr::core::array<irr::core::stringc> &cData, irr::u32 &nodeCount, irr::u32 &attributeCount);
	bool LoadToFloatStringArrayDataFromXMLFile(const char* fileName, irr::core::array<irr::f32> &fData, irr::core::array<irr::core::stringc> &cData);
	bool LoadToFloatStringArrayDataFromXMLFile(
		const char* fileName, 
		irr::core::array<irr::f32> &fData, char* fDataNodeName,
		irr::core::array<irr::core::stringc> &cData, char* cDataNodeName
		);
};

#endif