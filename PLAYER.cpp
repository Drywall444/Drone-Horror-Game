#include "PLAYER.h"

void PLAYER::createPLAYER()//CREATE PLAYER BEFORE ANY OTHER SPRITE
{
	gameSPRITES.spriteCREATE(HUMAN, TYPE_PLAYER, {0.0, 0.0});
	auto view = gameSPRITES.spriteREGISTER.view<playerOBJECT>();
	thePLAYER = *view.begin();
}

void PLAYER::cameraMOVEMENT()
{

    if (IN.KEYS[SDL_SCANCODE_W]) 
    { 
        IN.C.camPOS.y -= (cameraSPEED * IN.DT);
    }
    if (IN.KEYS[SDL_SCANCODE_S]) 
    { 
        IN.C.camPOS.y += (cameraSPEED * IN.DT);
    }
    if (IN.KEYS[SDL_SCANCODE_A]) 
    { 
        IN.C.camPOS.x -= (cameraSPEED * IN.DT);
    }
    if (IN.KEYS[SDL_SCANCODE_D]) 
    { 
        IN.C.camPOS.x += (cameraSPEED * IN.DT);
    }
}

