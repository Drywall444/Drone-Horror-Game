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
	TYPE_GRASS,
	TYPE_GRASS1,
	TYPE_WOODS,
	TYPE_WOODS1,
};

struct isTREE 
{
	bool treeTOP = true; //False = treeSTUMP
};

struct GUN
{
	std::string name = "AK-74";
	float weaponEFFECTIVE_RANGE = 1900.0;
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
};

struct TILE//SHIT THAT CAN MOVE
{
	SDL_FPoint pos;
	UV_REGION TYPE;
};

struct BUILDING
{
	std::vector<entt::entity> soldierINSIDE; //nothing inside yet
	int buildingMAX_SIZE = 1;
	float coverVALUE = 0.0; // 0-100% 
	bool topCOVERED = false;
	bool isOCCUPIED() { return soldierINSIDE.size() > 0; }
};

struct soldierOBJECT 
{
	bool friendly = false;
	float HP = 100.0;
	float speed = 150.0;
	float coverVALUE = 0.0;
	entt::entity curBUILDING = entt::null;

	//WEAPON INFO
	GUN weapon;
	int totalMAGS = 15;
	int curMAGS = 15;
	bool outOF_AMMO = false;

	//STATS
	float soldierSKILL = 0.5; //default 0.8 increase by 0.1 per day survived
};

struct grenadeOBJECT
{
	float fuseLENGTH = 3.5f;
	bool fuseENDED(float DT);
};

//STATES
struct MOVING
{
	float dX, dY;
	std::vector<LOCATION> waypoints; //Keep waypoint function
	float movementSPEED = 150.0;
	bool destroyAT_TARGET = false;
	float waitTIME_AT_WAYPOINT = 4.0f;
	float cur_waitTIME_AT_WAYPOINT = 4.0f;
	bool atFINAL_POINT = false;
	bool stopped = false;
};

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
	float throwTIME = 2.0f;
	bool throwing(float DT);
};

struct hasTARGET
{
	entt::entity enemySOLDIER;
	bool targetDEAD = false; //quit firing
};

struct IDLE{};
struct inCOVER {};
struct HIDDEN {};
struct ORDER_TO_BUILDING { entt::entity building; };
struct ORDER_TO_POINT {};

struct tempSPRITE{ float timeLEFT; };


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
		//VFX ROW #
		UV_REGION VFX_MAG = { 0.05f, 0.10f, 0.90f, 0.95f };
		UV_REGION VFX_GRENADE = { 0.10f, 0.15f, 0.90f, 0.95f };
		UV_REGION TREE_TRUNK = { 0.15f, 0.20f, 0.90f, 0.95f };

		UV_REGION FOXHOLE = { 0.2f, 0.4f, 0.80f, 1.0f };
		UV_REGION DUGOUT = { 0.4f, 0.6f, 0.80f, 1.0f };
		UV_REGION TREE_TOP = { 0.6f, 0.8f, 0.80f, 1.0f };

		//GAME LOOP
		float DT = 0.0;
		float LOS_DELAY = 0.5;
		float curLOS_DELAY = 0.5;
		void updateDT(float newDT);
		void updateGAME();
		void moveSPRITES();
		void checkEXPLOSIONS();
		std::vector<entt::entity> toDESTROY; //sprites to destroy at end of loop

		entt::registry spriteREGISTER;
		entt::entity createSPRITE(SDL_FPoint pos, ROTATION ROT, int texW, int texH, float z);
		void tileCREATE(UV_REGION type, SDL_FPoint pos);

		//SOLDIER MANAGER
		void createSOLDIER(SDL_FPoint pos, ROTATION rot, bool isFRIEND);
		entt::entity createCORPSE(SDL_FPoint pos, ROTATION rot, bool isFRIEND);
		entt::entity createCORPSE_IN_COVER(SDL_FPoint pos, ROTATION rot, bool isFRIEND, entt::entity buidling);
		MOVING newMOVEMENT(float speed, ROTATION dirTO, SDL_FPoint targetLOC);
		bool hasARRIVED_AT_POINT(SDL_FPoint spriteCUR_POS, MOVING& soldierMOVING_INFO);

		//SOLDIER ACTIONS
		void checkLOS(entt::entity soldier, bool checkENEMYS);
		void soldierSHOOT_AT_TARGET(entt::entity soldier);
		void shootAND_MOVE(entt::entity soldier);
		void fireWEAPON(entt::entity solder, hasTARGET target);
		void soldierTAKE_DAMAGE(entt::entity soldier, float damage);
		void ORDER_soldierMOVE_TO_POINT(entt::entity soldier, SDL_FPoint globalPOS);
		//GRENADE
		void soldierTHROW_GRENADE_AT_POS(entt::entity soldier, SDL_FPoint targetPOS);
		void soldiersAIM_GRENADE();
		entt::entity spawnGRENADE_THROW(throwingGRENADE thrownGRENADE, float forceOF_THROW);
		void explode(entt::entity explodingSPRITE);

		//Building
		entt::entity createBUILDING(SDL_FPoint pos, ROTATION rot, UV_REGION BUILDING_TEX_TYPE);
		void soldierMOVE_TO_BUILDING(entt::entity soldier, entt::entity building);
		void soldierMOVE_OUT_BUILDING(entt::entity building, SDL_FPoint globalPOS, bool movingIN_ANOTHER_BUILDING, entt::entity newBUIDLING = entt::null);
		void soldierENTERED_BUILDING(entt::entity soldier, entt::entity building);

		//VFX - Include sound here
		void spawnBULLET(entt::entity soldier, SDL_FPoint target);
		entt::entity createVFX(SDL_FPoint pos, ROTATION rot, UV_REGION MAG_TEX_TYPE, int w, int h, int z);

		//TREE SHIT
		void toggleTREE_TOPS();

private:
	float returnDIST_TO_TARGET(entt::entity soldier, hasTARGET targetINFO);
	void countDOWN_TEMP_SPRITES();
	void soldierHEALTH(entt::entity soldier);

};

#endif // !SPRITE_H

