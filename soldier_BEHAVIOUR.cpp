#include "SPRITE.h"

void SPRITE_MANAGER::assignBEHAVIOUR()
{
	auto allSoldiers = spriteREGISTER.view<soldierOBJECT>(); //STATE CHECK
	for (auto& S : allSoldiers)
	{
		auto& curS = spriteREGISTER.get<soldierOBJECT>(S);
		soldierHEALTH(S); //Health Check
		assignCOVER(S);

		if (curLOS_DELAY < 0.0)
		{
			auto& curTEAM_INFO = spriteREGISTER.get<TEAM>(S);
			//Check LOS
			checkLOS(S, curTEAM_INFO);
		}
		//flags
		bool movingAND_IDLE = false;
		bool moving = false;

		if (spriteREGISTER.all_of<MOVING>(S)) //if moving
		{
			auto& moveINFO = spriteREGISTER.get<MOVING>(S);
			if (moveINFO.stopped) { spriteREGISTER.emplace_or_replace<IDLE>(S); movingAND_IDLE = true; }//we idle baby and set flag
			else if (spriteREGISTER.all_of<IDLE>(S)) { spriteREGISTER.remove<IDLE>(S); } //if not stopped and still idle remove that hoe!
			moving = true;
		}


		auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(S);
		auto& teamINFO = spriteREGISTER.get<TEAM>(S); //clean this hoe up

		if (spriteREGISTER.all_of<throwingGRENADE>(S)) {
			spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_AIM_GRENADE : ENEMY_SOLDIER_AIM_GRENADE;
		}
		else if (moving) //we are moving 
		{
			if (spriteREGISTER.all_of<FIRING>(S) && movingAND_IDLE) { //If Moving, but we are waiting we 
				spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_SHOOTING : ENEMY_SOLDIER_SHOOTING;
				continue;
			}
			spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
			//can add moving animation here
		}
		else {
			//otherwise we idle
			if (spriteREGISTER.all_of<FIRING>(S)) {
				spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_SHOOTING : ENEMY_SOLDIER_SHOOTING;
			}
			else {
				spriteINFO.TYPE = (teamINFO.soldierTEAM == BLUFOR) ? SOLDIER_STANDING : ENEMY_SOLDIER_STANDING;
			}
			spriteREGISTER.emplace_or_replace<IDLE>(S);
		}
	}
}