#pragma once
#ifndef SPRITE_H
#define SPRITE_H
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "entt.hpp"
#include "GAME_MATH.h"


struct UV_REGION
{
	float uMIN;
	float uMAX;
	float vMIN;
	float vMAX;
};

enum natureTYPE_TILE
{
	TYPE_GRASS1,
	TYPE_GRASS2,
	TYPE_WOODS1,
	TYPE_WOODS2,
};

struct isTREE 
{
	bool treeTOP = true; //False = treeSTUMP
};

struct GUN
{
	std::string name = "AK-74";
	float weaponEFFECTIVE_RANGE = 500;
	float weaponRPM = 900;
	float weaponDMG = 55.0;
	int magSIZE = 30;
	int curMAG_SIZE = 31; //start at a full mag
	float reloadTIME = 5.0;
	float curRELOAD_TIME = 5.0; //hits zero when reloaded
	bool reloading = false;
};

struct spriteOBJECT//SHIT THAT CAN MOVE
{
	LOCATION spriteLOCATION;
	UV_REGION TYPE; //what is drawn
	int texW = 64;
	int texH = 64;
	bool hidden = false;
};

struct TILE
{
	SDL_FPoint pos;
	UV_REGION UV_TYPE;
	natureTYPE_TILE TYPE;
	std::vector<entt::entity> spritesWITHIN;
};

struct coverPOS
{
	entt::entity soldierIN_POS;
	SDL_FPoint globalPOS;
	ROTATION facing = {1.0f, 0.0f}; 
	float maxROTATION = -1.0f;  //-1 means free 360 rotation
};

struct BUILDING
{
	std::vector<coverPOS> soldierINSIDE; //nothing inside yet
	int numOF_SOLDIERS_INSIDE = 0;
	float coverVALUE = 0.0; // 0-100% 
	bool topCOVERED = false;
	float w = 0.0f, h = 0.0f;

	bool isEMPTY() { 
		return numOF_SOLDIERS_INSIDE <= 0;
	}

	bool isDUGOUT;
};

struct COVER
{
	coverPOS cover;
};

struct soldierOBJECT 
{
	float HP = 100.0f;
	float speed = 120.0f;
	float coverVALUE = 0.0f;
	float concealment = 0.0f;
	entt::entity curBUILDING = entt::null;

	//ORDERS
	entt::entity curORDER = entt::null;

	//WEAPON INFO
	GUN weapon;
	int totalMAGS = 12;
	int curMAGS = 12;
	bool outOF_AMMO = false;

	//STATS
	float soldierSKILL = 0.75; //default 0.8 increase by 0.1 per day survived
};

enum TEAMS {OPFOR, BLUFOR};

struct TEAM { TEAMS soldierTEAM = OPFOR; };

struct grenadeOBJECT
{
	float fuseLENGTH = 3.5f;
	bool fuseENDED(float DT);
};

//ORDERS
struct ORDER_TO_BUILDING { entt::entity building; };
struct ORDER_TO_POINT 
{
	std::vector<SDL_FPoint> waypoints;
};

//ACTIONS
struct FIRING
{
	float aimTIME = 0.0;
	float TIME_SINCE_LAST_SHOT = 0.0;
	float secPER_BULLET = 0.0;
	bool firingBURST = false; //ignore aimtime and fire multiple bullets in seccesion
};

struct throwingGRENADE 
{
	ROTATION throwDIR;
	SDL_FPoint throwPOS;
	SDL_FPoint targetPOS;
	float throwTIME = 2.5f;
	bool throwing(float DT);
};

struct hasTARGET
{
	entt::entity enemySOLDIER;
	bool targetDEAD = false; //quit firing
};

struct IDLE{};
struct inCOVER {};
struct inDUGOUT{};

//ENGINE
struct MOVING
{
	float dX, dY;
	SDL_FPoint waypoint; 
	float movementSPEED = 150.0;
	bool destroyAT_TARGET = false;
};

struct tempSPRITE { float orginalTIME = 0.0; float curTIME = 0.0; std::vector<UV_REGION> frames; };
struct isSTATIC {};
struct hasCOLLISION { int curINDEX = -1; };


struct selectedNOTHING {};


class SPRITE_MANAGER
{
	public:
		//SPRITE INFO
		UV_REGION GRASS_1 = { 0.0f,  0.25f, 0.0, 1.0 };
		UV_REGION GRASS_2 = { 0.25f,  0.50f, 0.0, 1.0 };
		UV_REGION WOODS_1 = { 0.50f,  0.75f, 0.0, 1.0 };
		UV_REGION WOODS_2 = { 0.75f,  1.0f,  0.0, 1.0 };

		UV_REGION ENEMY_SOLDIER_STANDING = { 0.0f, 0.20f, 0.0f, 0.2f };
		UV_REGION ENEMY_SOLDIER_SHOOTING = { 0.20f, 0.40f, 0.0f, 0.2f };
		UV_REGION ENEMY_SOLDIER_AIM_GRENADE = { 0.40f, 0.60f, 0.0f, 0.2f };
		UV_REGION ENEMY_SOLDIER_THROW_GRENADE = { 0.60f, 0.80f, 0.0f, 0.2f };
		UV_REGION ENEMY_SOLDIER_RELOADING = { 0.20f, 0.40f, 0.2f, 0.4f };

		UV_REGION ENEMY_SOLDIER_DEAD_1 = { 0.60f, 0.80f, 0.0f, 0.4f };
		UV_REGION ENEMY_SOLDIER_DEAD_2 = { 0.80f, 1.0f, 0.0f, 0.4f };

		UV_REGION SOLDIER_STANDING = { 0.0f, 0.20f, 0.4f, 0.6f };
		UV_REGION SOLDIER_SHOOTING = { 0.20f, 0.40f, 0.40f, 0.6f };
		UV_REGION SOLDIER_AIM_GRENADE = { 0.40f, 0.60f, 0.40f, 0.6f };
		UV_REGION SOLDIER_THROW_GRENADE = { 0.40f, 0.60f, 0.40f, 0.6f };
		UV_REGION SOLDIER_RELOADING = { 0.20f, 0.40f, 0.60f, 0.8f };
		UV_REGION SOLDIER_DEAD_1 = { 0.60f,  0.80f, 0.4f, 0.8f };
		UV_REGION SOLDIER_DEAD_2 = { 0.80f, 1.0f, 0.4f, 0.8f };

		//VFX ROW 1
		UV_REGION VFX_BULLET = { 0.15f, 0.2f, 0.8f, 0.85f };
		UV_REGION VFX_MUZZ_FLASH_3 = { 0.10f, 0.15f, 0.8f, 0.85f };
		UV_REGION VFX_MUZZ_FLASH_2 = { 0.05f, 0.10f, 0.8f, 0.85f };
		UV_REGION VFX_MUZZ_FLASH_1 = { 0.0f, 0.05f, 0.8f, 0.85f };
		//VFX ROW 2
		UV_REGION VFX_BLOOD_1 = { 0.15f, 0.2f, 0.85f, 0.90f };
		UV_REGION VFX_BLOOD_2 = { 0.10f, 0.15f, 0.85f, 0.90f };
		UV_REGION VFX_BLOOD_3 = { 0.05f, 0.10f, 0.85f, 0.90f };
		UV_REGION VFX_BLOOD_4 = { 0.0f, 0.05f, 0.85f, 0.90f };
		//VFX ROW 3
		UV_REGION VFX_GRENADE_EXPLOSION_FRAME1 = { 0.0f, 0.05f, 0.90f, 0.95f };
		UV_REGION VFX_MAG = { 0.05f, 0.10f, 0.90f, 0.95f };
		UV_REGION VFX_GRENADE = { 0.10f, 0.15f, 0.90f, 0.95f };
		UV_REGION TREE_TRUNK = { 0.15f, 0.20f, 0.90f, 0.95f };

		//VFX ROW 4
		UV_REGION VFX_GRENADE_EXPLOSION_FRAME2 = { 0.0f, 0.05f, 0.95f, 1.0f };

		UV_REGION FOXHOLE = { 0.2f, 0.4f, 0.80f, 1.0f };
		UV_REGION DUGOUT = { 0.4f, 0.6f, 0.80f, 1.0f };
		UV_REGION TREE_TOP = { 0.6f, 0.8f, 0.80f, 1.0f };
		UV_REGION DUGOUT_BOTTOM = { 0.8f, 1.0f, 0.80f, 1.0f };

		//GAME LOOP
		int XY_TO_tileNUM(int x, int y);
		float LOS_DELAY = 0.8;
		float curLOS_DELAY = 0.8;
		float DT = 0.0;
		void updateDT(float newDT);
		void updateGAME();
		void checkEXPLOSIONS();
		void assignCOVER(entt::entity soldier);
		std::vector<entt::entity> toDESTROY; //sprites to destroy at end of loop

		//Collision Detection
		std::vector<entt::entity> worldTILES;
		void assignCOLLISION();
		void checkCOLLISIONS();
		void removeCOLLISION_SPRITE_FROM_TILE(entt::entity sprite, entt::entity tile);

		entt::registry spriteREGISTER;
		entt::entity createSPRITE(SDL_FPoint pos, ROTATION ROT, int texW, int texH, float z);

		//SOLDIER MANAGER
		void createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isFRIEND);
		entt::entity createCORPSE(SDL_FPoint pos, ROTATION rot, TEAM teaminfo);
		entt::entity createCORPSE_IN_COVER(SDL_FPoint pos, ROTATION rot, TEAM teaminfo, entt::entity buidling);

		//MOVEMENT
		void moveSPRITES();
		MOVING newMOVEMENT(float speed, ROTATION dirTO, SDL_FPoint targetLOC);
		bool hasARRIVED_AT_POINT(SDL_FPoint spriteCUR_POS, MOVING& soldierMOVING_INFO);

		void ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS);
		void soldiersMOVE_TOWARD_POINT();

		//SOLDIER ACTIONS
		void decideSOLDIER_ACTIONS();

		//LOS
		void checkLOS(entt::entity soldier, TEAM teaminfo);
		std::vector<int> bresenLINE(int startTILE, int endTILE);

		//WEAPON
		void soldierSHOOT_AT_TARGET(entt::entity soldier);
		bool calculateHIT(float distance, float shooterSKILL, float weaponEFFECTIVE_RANGE);
		void fireWEAPON(entt::entity solder, hasTARGET target);
		void soldierTAKE_DAMAGE(entt::entity soldier, float damage);
		//GRENADE
		void soldierTHROW_GRENADE_AT_POS(entt::entity soldier, SDL_FPoint targetPOS);
		void soldiersAIM_GRENADE();
		entt::entity spawnGRENADE_THROW(throwingGRENADE thrownGRENADE, float forceOF_THROW);
		void explode(entt::entity explodingSPRITE, float blastRADIUS, int shrapnel, float dmg_PER_SHRAPNEL);

		//Building
		entt::entity createFOXHOLE(SDL_FPoint pos, ROTATION rot);
		entt::entity createDUGOUT(SDL_FPoint pos, ROTATION rot);
		entt::entity createBUILDING(SDL_FPoint pos, ROTATION rot, float w, float h, float z, UV_REGION BUILDING_TEX_TYPE, std::vector<coverPOS> firingPOS, float coverVALUE);
		void soldierMOVE_OUT_BUILDING(entt::entity soldier, entt::entity building);
		void soldierENTERED_BUILDING(entt::entity soldier, entt::entity building);
		void checkBUILDING_INSIDES();

		//VFX - Include sound here
		void spawnBULLET(entt::entity soldier, SDL_FPoint target);
		entt::entity createVFX(SDL_FPoint pos, ROTATION rot, UV_REGION MAG_TEX_TYPE, int w, int h, int z);

		//WORLD MAP
		entt::entity  tileCREATE(UV_REGION uv_type, SDL_FPoint pos, natureTYPE_TILE type);
		float natureTEX_W = 128, natureTEX_H = 128;
		int MAP_W = 150;
		int MAP_H = 300;
		void createMAP(SPRITE_MANAGER& sprites);
		entt::entity spawnTREE(SDL_FPoint pos);
		void toggleTREE_TOPS();

private:
	float returnDIST_TO_TARGET(entt::entity soldier, hasTARGET targetINFO);
	void countDOWN_TEMP_SPRITES();
	void soldierHEALTH(entt::entity soldier);

};


#endif // !SPRITE_H

