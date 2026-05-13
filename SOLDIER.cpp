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
			explode(grenade, 250, 1000, 5);
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


void SPRITE_MANAGER::explode(entt::entity explodingSPRITE, float blastRADIUS, int shrapnel, float dmg_PER_SHRAPNEL)
{
	//add VFX LATER
	std::cout << "Grenade exploded!\n";
	auto allSOLDIERS = spriteREGISTER.view<soldierOBJECT>();
	auto& grenade = spriteREGISTER.get<spriteOBJECT>(explodingSPRITE);

	//

	struct soldierWITHIN
	{
		entt::entity soldier;
		float dist;
	};

	std::vector<soldierWITHIN> allSOLDIERS_WITHIN_RADIUS;

	for (auto& soldier : allSOLDIERS) //collect all soldiers within radius
	{
		auto& curSPRITE_INFO = spriteREGISTER.get<spriteOBJECT>(soldier);
		float dist = distanceTO_POINT(grenade.spriteLOCATION.POS, curSPRITE_INFO.spriteLOCATION.POS);
		if (dist < blastRADIUS)
		{
			soldierWITHIN newSOLDIER_WITHIN;
			newSOLDIER_WITHIN.soldier = soldier;
			newSOLDIER_WITHIN.dist = dist;

			allSOLDIERS_WITHIN_RADIUS.push_back(newSOLDIER_WITHIN);
		}
	}

	static constexpr float FALLOFF = 2.5f;

	for (auto& explodedSOLDIER : allSOLDIERS_WITHIN_RADIUS)
	{
		float t = 1.0f - (explodedSOLDIER.dist / blastRADIUS);
		float hitPROB = std::pow(t, FALLOFF);
		float mu = shrapnel * hitPROB;
		float sigma = std::sqrt(mu * (1.0f - hitPROB));

		float z = std::sqrt(-2.0f * std::log(randBETWEEN(1e-9f, 1.0f)))
			* std::cos(2.0f * PI * randBETWEEN(0.0f, 1.0f));

		int hitsLANDED = std::clamp(static_cast<int>(std::round(mu + z * sigma)), 0, shrapnel);
		if (spriteREGISTER.all_of<inCOVER>(explodedSOLDIER.soldier))
		{

		}

		soldierTAKE_DAMAGE(explodedSOLDIER.soldier, hitsLANDED * dmg_PER_SHRAPNEL);

	}

	entt::entity newVFX = createVFX(grenade.spriteLOCATION.POS, randROTATION(), VFX_GRENADE_EXPLOSION_FRAME1, 128, 128, 1);
	std::vector<UV_REGION> grenadeFRAMES = { VFX_GRENADE_EXPLOSION_FRAME1, VFX_GRENADE_EXPLOSION_FRAME2 };
	spriteREGISTER.emplace<tempSPRITE>(newVFX, 0.35f, 0.0f, grenadeFRAMES);
	spriteREGISTER.destroy(explodingSPRITE); //delete

}
