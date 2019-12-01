#pragma once

#include "IrrCommon.h"
#include "BtEngine.h"
#include "CollisionManager.h"
#include "FSMObject.h"

//////////////////////////////////////////////
//											//
//	엔진을 제어할 파라미터 데이터 구조체	//
//											//
//////////////////////////////////////////////
struct SystemDATA
{
	//------------
	// 시스템 모드
	//------------

	// 컴파일만 하고 실행은 하지 않을때 true, 실행까지 하려면 false
	bool onlyCompile;

	// 게임을 초기화 하려면 true, 초기화 하지 않으려면 false
	bool gameInit;

	// 콘솔명령어 사용하려면 true, 사용하지 않으려면 false
	bool consoleCMD;

	//--------------------------
	// 일리히트 렌더링 파라미터
	//--------------------------

	// 디바이스 타입
	irr::video::E_DRIVER_TYPE deviceType;
	
	// 스크린 사이즈 ( 가로 * 세로 )
	int screen_size[2];
	
	// 풀스크린 모드
	bool full_screen;

	// 비트
	int bits;

	// 스텐실버퍼
	bool stencilBuffer;

	// 고정프레임
	bool vsync;

	// 마우스 포인터 보이기/감추기
	bool mouse_visible;

	// 작업디렉토리
	irr::core::stringc working_directory;

	//-------------------
	// irrKlang 파라미터
	//-------------------

	// 소리의 최소 거리
	irr::f32 soundMinDistance;


	//------------------
	// 필요한 XML 파일명
	//------------------
	irr::core::stringc heroXML;
	irr::core::stringc zombieXML;
	irr::core::stringc HL2zombieXML;
	irr::core::stringc stageXML;
	irr::core::stringc plasmarifleXML;
	irr::core::stringc particleXML;
	irr::core::stringc soundfileXML;
	irr::core::stringc imagefileXML;

	//--------------
	// 신노드 파일명
	//--------------

	// 스테이지
	irr::core::stringc stage_fileName;
	irr::core::stringc stage_SceneName;
	irr::core::stringc stage_physicName;
	
	// 주인공
	irr::core::stringc hero_fileName;
	irr::core::stringc hero_SceneName;

	// 좀비
	irr::core::stringc zombie_fileName;
	irr::core::stringc zombie_SceneName;

	// HL2좀비
	irr::core::stringc HL2zombie_fileName;
	irr::core::stringc HL2zombie_SceneName;

	// 플라즈마 라이플
	irr::core::stringc plasmarifle_fileName;
	irr::core::stringc plasmarifle_SceneName;

	// 게임에 필요한 데이터
	irr::core::array<irr::s32> data;
};


//////////////////////////////////////////////
//											//
//	월드에 필요한 파라미터 데이터 구조체	//
//	배경, 지형등의 생성에 필요한 데이터		//
//											//
//////////////////////////////////////////////
struct WorldDATA
{

};

struct StageDATA
{
	static const int total_stage = 2;
	irr::core::stringc stage[total_stage];
};


//////////////////////////////////////////////////////////////////////
//																	//
//	무기로 공격시 데미지를 받는 객체에 시그널 처리를 위한 구조체	//
//																	//
//////////////////////////////////////////////////////////////////////
struct WeaponAttackDATA
{
	// 데미지
	irr::f32 damage;

	// 물리엔진 힘값
	irr::f32 impulse;
	
	// 충돌데이터
	collisionData colData;
};

class PickingResultDATA
{
public :
	PickingResultDATA()
	: hasHit(false), isCharicterHit(false), pickedObj(NULL), key(NULL)
	{}
	~PickingResultDATA()
	{}

	btCollisionObject* key;
	bool hasHit;
	bool isCharicterHit;
	irr::core::vector3df hitPoint;
	irr::core::vector3df hitNormal;
	irr::core::vector3df hitDirection;
	collisionData colData;
	IFSMObject* pickedObj;
	std::vector<std::string> hitTypeName;
};


//////////////////////////////////////////////////
//												//
//	모든 오브젝트들의 파라미터를 위한 구조체	//
//												//
//////////////////////////////////////////////////
struct ObjectData
{
	// 스펙 파라미터
	std::vector<irr::f32> spec;
	
	// 물리 파라미터
	std::vector<irr::f32> physics;
	
	// 문자열 파라미터
	std::vector<std::string> str;
	
	// boolean값 파라미터
	std::vector<bool> boolean;
};


// 오브젝트매니저(맵)에 쓰일 키값과 데이터를 모아둔 구조체
struct objectMgrData
{
	btCollisionObject* colObj;
	SP_IFSMObject pObj;
};


// 사운드 관련 구조체
struct SoundSourceDATA
{
	std::vector<irrklang::ISoundSource*> hero;
	std::vector<irrklang::ISoundSource*> plasmarifle;
	std::vector<irrklang::ISoundSource*> zombie;
	std::vector<irrklang::ISoundSource*> stage;
};

struct SoundFileListDATA
{
	std::vector<std::string> heroList;
	std::vector<std::string> plasmarifleList;
	std::vector<std::string> zombieList;
	std::vector<std::string> stageList;
};