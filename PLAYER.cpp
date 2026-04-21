#include "PLAYER.h"

void PLAYER::createPLAYER()//CREATE PLAYER BEFORE ANY OTHER SPRITE
{
	gameSPRITES.spriteCREATE(HUMAN, TYPE_PLAYER, {0.0, 0.0});
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

void PLAYER::playerMOVEMENT()
{
    auto& loc = gameSPRITES.spriteREGISTER.get<spriteOBJECT>(thePLAYER).spriteLOCATION;

    float forwardX = loc.ROT.sinR;
    float forwardY = -loc.ROT.cosR;
    float rightX = loc.ROT.cosR;
    float rightY = loc.ROT.sinR;

    float speed = playerSPEED_WALK * IN.DT;
    float moveX = 0, moveY = 0;

    if (IN.KEYS[SDL_SCANCODE_W]) 
    { 
        moveX += forwardX * speed; 
        moveY += forwardY * speed;
    }
    if (IN.KEYS[SDL_SCANCODE_S]) 
    { 
        moveX -= forwardX * speed * 0.5;
        moveY -= forwardY * speed * 0.5;
    }
    if (IN.KEYS[SDL_SCANCODE_A]) 
    { 
        moveX -= rightX * speed / 3;
        moveY -= rightY * speed / 3;
    }
    if (IN.KEYS[SDL_SCANCODE_D]) 
    { 
        moveX += rightX * speed / 3;   
        moveY += rightY * speed /3; 
    }

    loc.POS.x += moveX;
    loc.POS.y += moveY;

    // Keep camera locked to player so mouse world pos stays accurate
    //IN.C.camPOS.x = loc.POS.x + (playerTEX_W / 2);
    //IN.C.camPOS.y = loc.POS.y + (playerTEX_H / 2);

    //std::cout << loc.POS.x << " " << loc.POS.y << std::endl;
}

void PLAYER::cameraFOLLOW_PLAYER()
{
    float cameraPULL = 8.0;

    auto& loc = gameSPRITES.spriteREGISTER.get<spriteOBJECT>(thePLAYER).spriteLOCATION;
    float distance = distanceTO_POINT(IN.C.camPOS, loc.POS);

        if (distance < 0.1)
        {
            //do nothing
        }
        else {
            ROTATION rotTOPLAYER = rotationTO_POINT(IN.C.camPOS, loc.POS);

            float dX = rotTOPLAYER.sinR * ((cameraPULL * distance) * IN.DT);
            float dY = -rotTOPLAYER.cosR * ((cameraPULL * distance) * IN.DT);

            IN.C.camPOS.x += dX;
            IN.C.camPOS.y += dY;
        }
}

