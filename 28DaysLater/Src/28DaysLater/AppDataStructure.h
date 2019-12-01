#pragma once

#include "IrrCommon.h"
#include "BtEngine.h"
#include "CollisionManager.h"
#include "FSMObject.h"

//////////////////////////////////////////////
//											//
//	������ ������ �Ķ���� ������ ����ü	//
//											//
//////////////////////////////////////////////
struct SystemDATA
{
	//------------
	// �ý��� ���
	//------------

	// �����ϸ� �ϰ� ������ ���� ������ true, ������� �Ϸ��� false
	bool onlyCompile;

	// ������ �ʱ�ȭ �Ϸ��� true, �ʱ�ȭ ���� �������� false
	bool gameInit;

	// �ָܼ�ɾ� ����Ϸ��� true, ������� �������� false
	bool consoleCMD;

	//--------------------------
	// �ϸ���Ʈ ������ �Ķ����
	//--------------------------

	// ����̽� Ÿ��
	irr::video::E_DRIVER_TYPE deviceType;
	
	// ��ũ�� ������ ( ���� * ���� )
	int screen_size[2];
	
	// Ǯ��ũ�� ���
	bool full_screen;

	// ��Ʈ
	int bits;

	// ���ٽǹ���
	bool stencilBuffer;

	// ����������
	bool vsync;

	// ���콺 ������ ���̱�/���߱�
	bool mouse_visible;

	// �۾����丮
	irr::core::stringc working_directory;

	//-------------------
	// irrKlang �Ķ����
	//-------------------

	// �Ҹ��� �ּ� �Ÿ�
	irr::f32 soundMinDistance;


	//------------------
	// �ʿ��� XML ���ϸ�
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
	// �ų�� ���ϸ�
	//--------------

	// ��������
	irr::core::stringc stage_fileName;
	irr::core::stringc stage_SceneName;
	irr::core::stringc stage_physicName;
	
	// ���ΰ�
	irr::core::stringc hero_fileName;
	irr::core::stringc hero_SceneName;

	// ����
	irr::core::stringc zombie_fileName;
	irr::core::stringc zombie_SceneName;

	// HL2����
	irr::core::stringc HL2zombie_fileName;
	irr::core::stringc HL2zombie_SceneName;

	// �ö�� ������
	irr::core::stringc plasmarifle_fileName;
	irr::core::stringc plasmarifle_SceneName;

	// ���ӿ� �ʿ��� ������
	irr::core::array<irr::s32> data;
};


//////////////////////////////////////////////
//											//
//	���忡 �ʿ��� �Ķ���� ������ ����ü	//
//	���, �������� ������ �ʿ��� ������		//
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
//	����� ���ݽ� �������� �޴� ��ü�� �ñ׳� ó���� ���� ����ü	//
//																	//
//////////////////////////////////////////////////////////////////////
struct WeaponAttackDATA
{
	// ������
	irr::f32 damage;

	// �������� ����
	irr::f32 impulse;
	
	// �浹������
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
//	��� ������Ʈ���� �Ķ���͸� ���� ����ü	//
//												//
//////////////////////////////////////////////////
struct ObjectData
{
	// ���� �Ķ����
	std::vector<irr::f32> spec;
	
	// ���� �Ķ����
	std::vector<irr::f32> physics;
	
	// ���ڿ� �Ķ����
	std::vector<std::string> str;
	
	// boolean�� �Ķ����
	std::vector<bool> boolean;
};


// ������Ʈ�Ŵ���(��)�� ���� Ű���� �����͸� ��Ƶ� ����ü
struct objectMgrData
{
	btCollisionObject* colObj;
	SP_IFSMObject pObj;
};


// ���� ���� ����ü
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