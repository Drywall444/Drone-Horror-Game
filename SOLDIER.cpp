#include "sprite.h"



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
			spawnGRENADE_THROW(curG, 550);
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
	entt::entity grenade = createSPRITE(thrownGRENADE.throwPOS, thrownGRENADE.throwDIR, 20, 20, 1);
	auto& newGRENADE_STRUCT = spriteREGISTER.get<spriteOBJECT>(grenade);
	newGRENADE_STRUCT.TYPE = VFX_GRENADE;
	spriteREGISTER.emplace<grenadeOBJECT>(grenade);
	MOVING newMOVE = newMOVEMENT(forceOF_THROW, thrownGRENADE.throwDIR, randPOINT_FROM(thrownGRENADE.targetPOS, 120));
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
	entt::entity newVFX = createVFX(grenade.spriteLOCATION.POS, randROTATION(), VFX_GRENADE_EXPLOSION_FRAME1, 128, 128, 1);
	std::vector<UV_REGION> grenadeFRAMES = { VFX_GRENADE_EXPLOSION_FRAME1, VFX_GRENADE_EXPLOSION_FRAME2 };
	spriteREGISTER.emplace<tempSPRITE>(newVFX, 0.35f, 0.0f, grenadeFRAMES);
	spriteREGISTER.destroy(explodingSPRITE); //delete

}
