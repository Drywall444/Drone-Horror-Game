#include "SPRITE.h"

//DAMAGE

void SPRITE_MANAGER::soldierTAKE_DAMAGE(entt::entity soldier, float damage)
{
	spriteREGISTER.get<soldierOBJECT>(soldier).HP -= damage;
	auto& curSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	SDL_FPoint bloodPOINT = { curSOLDIER.spriteLOCATION.POS.x, curSOLDIER.spriteLOCATION.POS.y };
	int numOFBLOOD = damage / 50;
	if (numOFBLOOD == 0)
	{
		numOFBLOOD = 1;
	}

	for (int i = 0;i < numOFBLOOD; i++)
	{
		UV_REGION bloodTEX_TYPE;
		float randBETWEEN4 = randBETWEEN(0.0, 4.0);
		if (randBETWEEN4 > 3.0) { bloodTEX_TYPE = VFX_BLOOD_1; }
		else if (randBETWEEN4 > 2.0) { bloodTEX_TYPE = VFX_BLOOD_2; }
		else if (randBETWEEN4 > 1.0) { bloodTEX_TYPE = VFX_BLOOD_3; }
		else { bloodTEX_TYPE = VFX_BLOOD_4; }

		createVFX(randPOINT_FROM(bloodPOINT, 64), randROTATION(), bloodTEX_TYPE, 32, 32, -1);
	}


}

void SPRITE_MANAGER::soldierSHOOT_AT_TARGET(entt::entity soldier)
{
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	if (soldierINFO.outOF_AMMO) { return; }
	if (spriteREGISTER.all_of<throwingGRENADE>(soldier)) { return; } //throwing grenade

	auto& soldiersSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);
	float curDISTANCE = returnDIST_TO_TARGET(soldier, soldiersSHOOTING);

	if (curDISTANCE < 1000 && !spriteREGISTER.all_of<throwingGRENADE>(soldier)) //If close throw a grenade
	{
		float rand = randBETWEEN(0.0f, 100.0f);
		if (rand > 99.97f)
		{
			auto& enemySOLDIER_INFO = spriteREGISTER.get<spriteOBJECT>(soldiersSHOOTING.enemySOLDIER);
			soldierTHROW_GRENADE_AT_POS(soldier, enemySOLDIER_INFO.spriteLOCATION.POS);
			return;
		}
	}

	// Idle and not firing yet - set up FIRING
	if (!spriteREGISTER.all_of<FIRING>(soldier))
	{
		FIRING newFIRE;
		newFIRE.secPER_BULLET = (1.0f / (soldierINFO.weapon.weaponRPM / 60.0f));
		newFIRE.TIME_SINCE_LAST_SHOT = newFIRE.secPER_BULLET;
		newFIRE.aimTIME = curDISTANCE / 1000.0;
		spriteREGISTER.emplace<FIRING>(soldier, newFIRE);
	}

	auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& enemySPRITE = spriteREGISTER.get<spriteOBJECT>(soldiersSHOOTING.enemySOLDIER);
	soldierSPRITE_INFO.spriteLOCATION.ROT = rotationTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, enemySPRITE.spriteLOCATION.POS);
	fireWEAPON(soldier, soldiersSHOOTING);
}

void SPRITE_MANAGER::fireWEAPON(entt::entity soldier, hasTARGET target) //CLEANUP NEXT 5/3/26, BUG: soldiers keep firing even when has no target
{

	auto& fireINFO = spriteREGISTER.get<FIRING>(soldier);
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	auto& spriteINFO = spriteREGISTER.get<spriteOBJECT>(soldier);


	if (soldierINFO.weapon.curMAG_SIZE <= 0 && soldierINFO.weapon.reloading == false) //If our mag is on empty and we havent started reloading, reload
	{
		soldierINFO.weapon.reloading = true;
		soldierINFO.weapon.curRELOAD_TIME = soldierINFO.weapon.reloadTIME; //set to full reload time
	}

	if (soldierINFO.weapon.reloading == true)
	{
		soldierINFO.weapon.curRELOAD_TIME -= DT;
		auto& soldierTEAM_INFO = spriteREGISTER.get<TEAM>(soldier);

		if (soldierTEAM_INFO.soldierTEAM == BLUFOR) { spriteINFO.TYPE = SOLDIER_RELOADING; } //set animation to reloading basic as hell will change later
		else { spriteINFO.TYPE = ENEMY_SOLDIER_RELOADING; }

		if (soldierINFO.weapon.curRELOAD_TIME < 0.0)
		{
			//finished reloading
			float randY = randBETWEEN(-15.0, 15.0);
			SDL_FPoint magOFF_SET = { 30.0, randY };
			SDL_FPoint magPOS = rotatePOINT_AND_APPLY_OFFSET(spriteINFO.spriteLOCATION.POS, spriteINFO.spriteLOCATION.ROT, magOFF_SET);
			createVFX(magPOS, randROTATION(), VFX_MAG, 16, 16, -1);
			soldierINFO.weapon.curMAG_SIZE = soldierINFO.weapon.magSIZE;
			soldierINFO.weapon.reloading = false; //When done reloading set false and then return
			soldierINFO.curMAGS -= 1;
			if (soldierINFO.curMAGS <= 0)
			{
				soldierINFO.outOF_AMMO = true;
			}
		}
		return;
	}

	if (fireINFO.aimTIME < 0.0 || fireINFO.firingBURST && soldierINFO.weapon.reloading == false)
	{
		if (fireINFO.TIME_SINCE_LAST_SHOT > fireINFO.secPER_BULLET)
		{
			if (fireINFO.firingBURST == false)
			{
				fireINFO.firingBURST = true;
			}
			else {
				float rand = randBETWEEN(0.0, 5.0);
				if (rand > 4.0)
				{
					fireINFO.firingBURST = false;
				}
			}

			auto& soldierSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
			auto& enemySPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(target.enemySOLDIER);
			auto& enemySPRITE = spriteREGISTER.get<soldierOBJECT>(target.enemySOLDIER);

			float dist = distanceTO_POINT(soldierSPRITE_INFO.spriteLOCATION.POS, enemySPRITE_INFO.spriteLOCATION.POS);
			fireINFO.aimTIME = dist / 1000;

			if (calculateHIT(dist, soldierINFO.soldierSKILL, soldierINFO.weapon.weaponEFFECTIVE_RANGE))
			{
				float coverRAND = randBETWEEN(0.0f, 1.0f);
				float rangeFACTOR = dist / soldierINFO.weapon.weaponEFFECTIVE_RANGE;
				float rangeFACTOR_PROBABILITY = (1.0f - enemySPRITE.coverVALUE) / rangeFACTOR;
				if (coverRAND < rangeFACTOR_PROBABILITY)
				{
					//we hit
					soldierTAKE_DAMAGE(target.enemySOLDIER, soldierINFO.weapon.weaponDMG);
				}
				else {
					//cover blocked it
					//hit vfx
				}

				//VFX
				spawnBULLET(soldier, enemySPRITE_INFO.spriteLOCATION.POS);
				if (enemySPRITE.HP < 0.0)
				{
					//stop firing when dead, nolonger have target
					spriteREGISTER.get<hasTARGET>(soldier).targetDEAD = true;
					spriteREGISTER.remove<FIRING>(soldier);

					return;
				}
			}
			else {
				float randX = randBETWEEN(enemySPRITE_INFO.texW / 2, 90.0);
				float randY = randBETWEEN(enemySPRITE_INFO.texH / 2, 300.0);
				if (randBETWEEN(0.0, 1.0) > 0.5) { randX = -randX; }
				if (randBETWEEN(0.0, 1.0) > 0.5) { randY = -randY; }
				SDL_FPoint missPOINT = { enemySPRITE_INFO.spriteLOCATION.POS.x + randX, enemySPRITE_INFO.spriteLOCATION.POS.y + randY };
				spawnBULLET(soldier, missPOINT);
			}
			soldierINFO.weapon.curMAG_SIZE -= 1;
			fireINFO.TIME_SINCE_LAST_SHOT = 0.0;
		}
	}


	fireINFO.TIME_SINCE_LAST_SHOT += DT; //increase by secs
	if (!fireINFO.firingBURST) //If not firing burst reduce aim time while we steady
	{
		fireINFO.aimTIME -= DT; //reduce steady time
	}

}


bool SPRITE_MANAGER::calculateHIT(float distance, float shooterSKILL, float weaponEFFECTIVE_RANGE)
{
	float rangeFACTOR = distance / weaponEFFECTIVE_RANGE;
	float finalHIT = 0.10f / rangeFACTOR;
	finalHIT = std::clamp(finalHIT, 0.001f, 0.75f);

	float randNUMBER = randBETWEEN(0.0f, 1.0f);

	if (randNUMBER < finalHIT) { return true; }
	else { return false; }
}