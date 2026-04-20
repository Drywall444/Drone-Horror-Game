#include "SPRITE.h"

void SPRITE_MANAGER::spriteCREATE(textureATLAS sheetNUM)
{

	entt::entity newSPRITE = spriteREGISTER.create();
	spriteOBJECT newSPRITE_OBJ;
	ROTATION newROT = { 1.0, 0.0 };
	LOCATION newLOC = { {0.0, 0.0}, newROT };
	newSPRITE_OBJ.spriteLOCATION = newLOC;
	newSPRITE_OBJ.textureSHEET_NUM = sheetNUM;
	spriteREGISTER.emplace<spriteOBJECT>(newSPRITE, newSPRITE_OBJ);
}