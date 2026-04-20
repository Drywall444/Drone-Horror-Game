#include "PLAYER.h"

void PLAYER::createPLAYER()//CREATE PLAYER BEFORE ANY OTHER SPRITE
{
	gameSPRITES.spriteCREATE(HUMAN, TYPE_PLAYER);
	auto view = gameSPRITES.spriteREGISTER.view<playerOBJECT>();
	thePLAYER = *view.begin();
}

void PLAYER::playerROTATE_TO_MOUSE()
{
	SDL_FPoint playerPOS = gameSPRITES.spriteREGISTER.get<spriteOBJECT>(thePLAYER).spriteLOCATION.POS;
	SDL_FPoint playerCENTER = { playerPOS.x + (playerTEX_W / 2), playerPOS.y + (playerTEX_H / 2) };

	SDL_FPoint worldPOS = IN.screenTO_WORLD_POS(IN.M.screenMOUSE_POS);

	ROTATION newROT = rotationTO_POINT(playerCENTER, worldPOS);
	gameSPRITES.spriteREGISTER.get<spriteOBJECT>(thePLAYER).spriteLOCATION.ROT = newROT;
}

