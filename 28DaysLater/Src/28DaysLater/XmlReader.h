#pragma once

#include "StdLibrary.h"
#include "AppDataStructure.h"

#include "tinyxml.h"
#include "tokenizer.h"
#include "ggf_parser.h"

class CXmlReader
{
public:
	CXmlReader(void);
	~CXmlReader(void);

	bool returnTrueOrNot(std::string value);

	bool readSystemDATA(const char* filename, SystemDATA &systemData);
	bool readStageDATA(const char* filename, StageDATA &stageData);
	bool readObjectData(const char* fileName, ObjectData &objectData);
	bool readSoundFileDATA(const char* fileName, SoundFileListDATA &soundData);
};
