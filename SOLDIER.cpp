#include "sprite.h"

//Seperate movement


//LOS

void SPRITE_MANAGER::checkLOS(entt::entity soldier, bool friendly)
{

	float losRANGE = 2000.0;

	auto allSOLDIER = spriteREGISTER.view<soldierOBJECT>();

	auto& curSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	auto& curSOLDIER_INFO = spriteREGISTER.get<soldierOBJECT>(soldier);

	bool hasTARGET_BOOL = false;
	bool removeTARGET = false;
	if (spriteREGISTER.all_of<hasTARGET>(soldier))
	{
		hasTARGET_BOOL = true;

		auto& targetINFO = spriteREGISTER.get<hasTARGET>(soldier);
		if (!spriteREGISTER.valid(targetINFO.enemySOLDIER)) //if target has died since last check
		{
			spriteREGISTER.remove<hasTARGET>(soldier);
			hasTARGET_BOOL = false;
		}
	}

	for (auto& enemy : allSOLDIER)
	{
		auto& curENEMY = spriteREGISTER.get<spriteOBJECT>(enemy);
		auto& curENEMY_INFO = spriteREGISTER.get<soldierOBJECT>(enemy);
		//Skip friendly checks
		if (friendly == true && curENEMY_INFO.friendly == true) { continue; }
		if (friendly == false && curENEMY_INFO.friendly == false) { continue; }


		float dis = distanceTO_POINT(curSOLDIER.spriteLOCATION.POS, curENEMY.spriteLOCATION.POS);

		if (dis > losRANGE) //Skip if to far
		{
			if (hasTARGET_BOOL)
			{
				// only remove if THIS enemy IS our current target
				auto& currentTARGET = spriteREGISTER.get<hasTARGET>(soldier);
				if (currentTARGET.enemySOLDIER == enemy)
				{
					removeTARGET = true;
					hasTARGET_BOOL = false;
				}
			}
			continue;
		}
		else if (hasTARGET_BOOL)
		{
			auto& targetINFO = spriteREGISTER.get<hasTARGET>(soldier);
			float curTARGET_DIS = distanceTO_POINT(curSOLDIER.spriteLOCATION.POS, spriteREGISTER.get<spriteOBJECT>(targetINFO.enemySOLDIER).spriteLOCATION.POS);
			//if new target closer than current target switch
			if (curTARGET_DIS > dis)
			{
				removeTARGET = false;
				hasTARGET newTARGET = { enemy, false };
				spriteREGISTER.emplace_or_replace<hasTARGET>(soldier, newTARGET);

			}
		}
		else {
			removeTARGET = false;
			hasTARGET newTARGET = { enemy, false };
			spriteREGISTER.emplace_or_replace<hasTARGET>(soldier, newTARGET);
		}
	}

	if (removeTARGET) { spriteREGISTER.remove<hasTARGET>(soldier); }
}

//DAMAGE

void SPRITE_MANAGER::soldierTAKE_DAMAGE(entt::entity soldier, float damage)
{
	spriteREGISTER.get<soldierOBJECT>(soldier).HP -= damage;
	auto& curSOLDIER = spriteREGISTER.get<spriteOBJECT>(soldier);
	float randX = randBETWEEN(5, 25);
	float randY = randBETWEEN(5, 25);
	if (randBETWEEN(0.0, 1.0) > 0.5) { randX = -randX; }
	if (randBETWEEN(0.0, 1.0) > 0.5) { randY = -randY; }
	SDL_FPoint bloodPOINT = { curSOLDIER.spriteLOCATION.POS.x + randX, curSOLDIER.spriteLOCATION.POS.y + randY };

	UV_REGION bloodTEX_TYPE;
	float randBETWEEN4 = randBETWEEN(0.0, 4.0);
	if (randBETWEEN4 > 3.0) { bloodTEX_TYPE = VFX_BLOOD_1; }
	else if (randBETWEEN4 > 2.0) { bloodTEX_TYPE = VFX_BLOOD_2; }
	else if (randBETWEEN4 > 1.0) { bloodTEX_TYPE = VFX_BLOOD_3; }
	else { bloodTEX_TYPE = VFX_BLOOD_4; }

	createVFX(bloodPOINT, randROTATION(), bloodTEX_TYPE, 32, 32, -1);

}

void SPRITE_MANAGER::soldierSHOOT_AT_TARGET(entt::entity soldier)
{
	auto& soldierINFO = spriteREGISTER.get<soldierOBJECT>(soldier);
	if (soldierINFO.outOF_AMMO) { return; }
	if (spriteREGISTER.all_of<throwingGRENADE>(soldier)) { return; } //throwing grenade

	auto& soldiersSHOOTING = spriteREGISTER.get<hasTARGET>(soldier);
	float curDISTANCE = returnDIST_TO_TARGET(soldier, soldiersSHOOTING);

	if (curDISTANCE < 800 && !spriteREGISTER.all_of<throwingGRENADE>(soldier)) //If close throw a grenade
	{
		float rand = randBETWEEN(0.0f, 100.0f);
		if (rand < 95.0f)
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

void SPRITE_MANAGER::fireWEAPON(entt::entity soldier, hasTARGET target) //CLEANUP NEXT 5/3/26
{
	float baseMISS = 80.0;

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
		if (soldierINFO.friendly) { spriteINFO.TYPE = SOLDIER_RELOADING; } //set animation to reloading basic as hell will change later
		else { spriteINFO.TYPE = ENEMY_SOLDIER_RELOADING; }

		if (soldierINFO.weapon.curRELOAD_TIME < 0.0)
		{
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
			float soldierSKILL = soldierINFO.soldierSKILL;

			float rangeFACTOR = dist / soldierINFO.weapon.weaponEFFECTIVE_RANGE;
			float baseHIT = 0.95f - (rangeFACTOR * rangeFACTOR) * 0.85f;
			float finalHIT = baseHIT * soldierINFO.soldierSKILL;
			finalHIT = std::clamp(finalHIT, 0.01f, 0.99f);
			//If miss randomize direction

			float randNUMBER = randBETWEEN(0.0f, 1.0f);

			if (randNUMBER < (finalHIT - (finalHIT * enemySPRITE.coverVALUE)) * soldierSKILL)
			{
				//we hit
				//soldierTAKE_DAMAGE(target.enemySOLDIER, soldierINFO.weapon.weaponDMG);
				//apply damage after bullet hits


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
				float randY = randBETWEEN(enemySPRITE_INFO.texH / 2, 90.0);
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

//grenade

void SPRITE_MANAGER::soldiersAIM_GRENADE()
{
	//grenades throwing action

	auto allGRENADES = spriteREGISTER.view<throwingGRENADE>();
	for (auto& soldier : allGRENADES)
	{
		auto& curG = spriteREGISTER.get<throwingGRENADE>(soldier);
		if (curG.throwing(DT))
		{
			//spawn grenade here
			spawnGRENADE_THROW(curG, 450);
			spriteREGISTER.remove<throwingGRENADE>(soldier);
			continue;
		}
	}
}

void SPRITE_MANAGER::checkEXPLOSIONS()
{
	auto allGRENADES = spriteREGISTER.view<grenadeOBJECT>();
	for (auto& grenade : allGRENADES)
	{
		auto& curG = spriteREGISTER.get<grenadeOBJECT>(grenade);
		if (curG.fuseENDED(DT))
		{
			explode(grenade);
			//explode
		}
	}
}

void SPRITE_MANAGER::soldierTHROW_GRENADE_AT_POS(entt::entity soldier, SDL_FPoint targetPOS)
{
	auto& spriteSOLDIER_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	ROTATION dir = directionTO_POINT(spriteSOLDIER_INFO.spriteLOCATION.POS, targetPOS);
	throwingGRENADE newGRENADE_THROW;
	newGRENADE_THROW.targetPOS = targetPOS;
	newGRENADE_THROW.throwDIR = dir;
	newGRENADE_THROW.throwPOS = spriteSOLDIER_INFO.spriteLOCATION.POS;
	spriteREGISTER.emplace<throwingGRENADE>(soldier, newGRENADE_THROW);

}

bool throwingGRENADE::throwing(float DT)
{
	throwTIME -= DT;
	return throwTIME < 0.0f;
}

bool grenadeOBJECT::fuseENDED(float DT)
{
	fuseLENGTH -= DT;
	return fuseLENGTH < 0.0f;
}

entt::entity SPRITE_MANAGER::spawnGRENADE_THROW(throwingGRENADE thrownGRENADE, float forceOF_THROW)
{
	std::cout << "Grenade Spawned!\n";
	entt::entity grenade = createSPRITE(thrownGRENADE.throwPOS, thrownGRENADE.throwDIR, 16, 16, 1);
	auto& newGRENADE_STRUCT = spriteREGISTER.get<spriteOBJECT>(grenade);
	newGRENADE_STRUCT.TYPE = VFX_GRENADE;
	spriteREGISTER.emplace<grenadeOBJECT>(grenade);
	MOVING newMOVE = newMOVEMENT(forceOF_THROW, thrownGRENADE.throwDIR, thrownGRENADE.targetPOS);
	spriteREGISTER.emplace<MOVING>(grenade, newMOVE);

	return grenade;
}


void SPRITE_MANAGER::explode(entt::entity explodingSPRITE)
{
	//add VFX LATER
	std::cout << "Grenade exploded!\n";
	auto allSOLDIERS_NOT_IN_COVER = spriteREGISTER.view<soldierOBJECT>(entt::exclude<inCOVER>);
	auto& grenade = spriteREGISTER.get<spriteOBJECT>(explodingSPRITE);
	for (auto& soldier : allSOLDIERS_NOT_IN_COVER)
	{
		auto& curSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
		float dist = distanceTO_POINT(grenade.spriteLOCATION.POS, curSPRITE_INFO.spriteLOCATION.POS);
		if (dist < 300)
		{
			float dmg = ((300 - dist) / 300) * 150; //baased on distance from explosion
			soldierTAKE_DAMAGE(soldier, 150.0f);
		}
	}
	entt::entity newVFX = createVFX(grenade.spriteLOCATION.POS, randROTATION(), VFX_MUZZ_FLASH_3, 80, 80, 1);
	spriteREGISTER.emplace<tempSPRITE>(newVFX, 0.2f);
	spriteREGISTER.destroy(explodingSPRITE); //delete

}


//TO ADD: soldierTHROW_GRENADE_AT_POS(soldier, targetPOS), grenadeEXPLODE(grenade) -> checkPOS and then damage all within range regardless of friend or foe, soldierCHECK_GRENADE_RANGE(soldier) -> plae within firing function, if within range randomly decide to lob a grenade. 
