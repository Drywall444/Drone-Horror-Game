#include "SPRITE.h"

//VFX

void SPRITE_MANAGER::spawnBULLET(entt::entity soldier, SDL_FPoint target)
{
	float bulletSPEED = 3500.0;

	auto& soldierINFO = spriteREGISTER.get<spriteOBJECT>(soldier);
	SDL_FPoint offSET_TRACER = { 7.5f, -50.0f };

	ROTATION newROT = soldierINFO.spriteLOCATION.ROT;
	SDL_FPoint bulletPOS = rotatePOINT_AND_APPLY_OFFSET(soldierINFO.spriteLOCATION.POS, newROT, offSET_TRACER);
	entt::entity newBULLET = createVFX(bulletPOS, newROT, VFX_BULLET, 32, 64, 0);

	//EMPLACE MOVING
	ROTATION dirTOPOINT = directionTO_POINT(bulletPOS, target);
	MOVING bulletMOVEMENT = newMOVEMENT(bulletSPEED, dirTOPOINT, target);
	bulletMOVEMENT.destroyAT_TARGET = true; //destroy once at target
	spriteREGISTER.emplace<MOVING>(newBULLET, bulletMOVEMENT);

	//MUZZLE FLASH
	SDL_FPoint offSET_MUZZLE_FLASH = { 6.5f, -50.0f };
	SDL_FPoint flashPOS = rotatePOINT_AND_APPLY_OFFSET(soldierINFO.spriteLOCATION.POS, newROT, offSET_MUZZLE_FLASH);
	UV_REGION randFLASH_UV;
	float rand = randBETWEEN(0.0, 3.0);
	if (rand > 0.2) { randFLASH_UV = VFX_MUZZ_FLASH_1; }
	else if (rand > 0.1) { randFLASH_UV = VFX_MUZZ_FLASH_2; }
	else { randFLASH_UV = VFX_MUZZ_FLASH_3; }
	entt::entity newFLASH = createVFX(flashPOS, newROT, randFLASH_UV, 32, 32, 0.5);

	float randFLASH = randBETWEEN(0.02f, 0.08f);
	std::vector<UV_REGION> MUZZLE_ANIMATION = { randFLASH_UV };
	spriteREGISTER.emplace<tempSPRITE>(newFLASH, randFLASH, 0.0f, MUZZLE_ANIMATION);

}

entt::entity SPRITE_MANAGER::createVFX(SDL_FPoint pos, ROTATION rot, UV_REGION MAG_TEX_TYPE, int w, int h, int z)
{
	entt::entity newVFX = spriteREGISTER.create();
	spriteOBJECT newSPRITE_OBJ;
	ROTATION newROT = rot;
	LOCATION newLOC = { pos, newROT };
	newSPRITE_OBJ.spriteLOCATION = newLOC;
	newSPRITE_OBJ.texW = w;
	newSPRITE_OBJ.texH = h;
	newSPRITE_OBJ.TYPE = MAG_TEX_TYPE;
	newSPRITE_OBJ.spriteLOCATION.z = z;
	spriteREGISTER.emplace<spriteOBJECT>(newVFX, newSPRITE_OBJ);

	return newVFX;
}