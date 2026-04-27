#include "INPUT.h"
#include <algorithm>

SDL_FPoint INPUT::screenTO_WORLD_POS(SDL_FPoint screenPOINT)
{
	//(screenPos - camOff) / zoom - Formula, fuck math
	SDL_FPoint final = { (screenPOINT.x - C.offSET.x) / C.zoom, (screenPOINT.y - C.offSET.y) / C.zoom };
	return final;
}


void INPUT::initializeINPUT(int windowW, int windowH)
{
	 WINDOW_W = windowW;
	 WINDOW_H = windowH;
	 WINDOW_CENTER_X = windowW / 2;
	 WINDOW_CENTER_Y = windowH / 2;

	 curSELECTED_SOLDIER = gameSPRITES.spriteREGISTER.create();
	 gameSPRITES.spriteREGISTER.emplace<selectedNOTHING>(curSELECTED_SOLDIER);
	 nothing = curSELECTED_SOLDIER;
}

void INPUT::handleINPUT(float dt)
{
	DT = dt;
	SDL_Event input;
	while (SDL_PollEvent(&input))
	{
		if (input.type == SDL_EVENT_QUIT)
		{
			RUN = false;
		}

		if (input.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (input.button.button == SDL_BUTTON_LEFT)
			{
				frameSTART = SDL_GetPerformanceCounter(); // detect time between clicks and if left click held for certain time we have held left click
				M.isHELD_DOWN = true;
				checkLEFTCLICK_RETURN_SPRITE(screenTO_WORLD_POS(M.screenMOUSE_POS));
				std::cout << "Left Click detetcted... do sum\n";

				//Player checkSELECTION
			}
			if (input.button.button == SDL_BUTTON_RIGHT)
			{
				if (seclectedSOLDIER)
				{
					gameSPRITES.ORDER_soldierMOVE_TO_POINT(curSELECTED_SOLDIER, screenTO_WORLD_POS(M.screenMOUSE_POS));
				}

			}
		}
		if (input.type == SDL_EVENT_MOUSE_BUTTON_UP)


		{
			if (input.button.button == SDL_BUTTON_LEFT)
			{
				M.isHELD_DOWN = false;
				std::cout << "Left Click Let GO\n";
			}
		}
		if (input.type == SDL_EVENT_MOUSE_WHEEL)
		{
			C.zoom = std::clamp(C.zoom + (scrollWHEEL_SENS * input.wheel.y), 0.50, 20.0); //clamps to min and max zoom
			getCAM_OFFSET();
		}
		if (input.type == SDL_EVENT_KEY_DOWN)
		{
			if (input.key.key == SDLK_ESCAPE) {
				std::cout << "hit escape\n";
				RUN = false;
			}
		}
	}
	frameEND = SDL_GetPerformanceCounter();
	timeINSEC = double(frameEND - frameSTART) / SDL_GetPerformanceFrequency();
	//std::cout << timeINSEC << std::endl;

	if (timeINSEC > 0.25 && M.isHELD_DOWN)
	{
		std::cout << "Mouse Was held down\n";
		//Draw box from first held mouse pos to current mouse pos and then get all sprites within bounds. 
	}

	SDL_GetMouseState(&M.screenMOUSE_POS.x, &M.screenMOUSE_POS.y);

	KEYS = SDL_GetKeyboardState(nullptr);
	//frameSTART = frameEND;
}

//CAMERA

void INPUT::getCAM_OFFSET()
{
	C.offSET.x = WINDOW_CENTER_X - C.camPOS.x * C.zoom;
	C.offSET.y = WINDOW_CENTER_Y - C.camPOS.y * C.zoom;
}

void INPUT::getWINDOWSIZE(SDL_Window* window)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	WINDOW_W = w;
	WINDOW_H = h;
	WINDOW_CENTER_X = w / 2.0f;
	WINDOW_CENTER_Y = h / 2.0f;

}

//PLAYER INTERACT
void INPUT::cameraMOVEMENT()
{

	if (KEYS[SDL_SCANCODE_W])
	{
		C.camPOS.y -= (cameraSPEED * DT);
	}
	if (KEYS[SDL_SCANCODE_S])
	{
		C.camPOS.y += (cameraSPEED * DT);
	}
	if (KEYS[SDL_SCANCODE_A])
	{
		C.camPOS.x -= (cameraSPEED * DT);
	}
	if (KEYS[SDL_SCANCODE_D])
	{
		C.camPOS.x += (cameraSPEED * DT);
	}
}

void INPUT::checkLEFTCLICK_RETURN_SPRITE(SDL_FPoint globalPOS)
{
	auto soldierVIEW = gameSPRITES.spriteREGISTER.view<soldierOBJECT>();
	seclectedSOLDIER = false;

	for (auto& soldier : soldierVIEW)
	{
		auto& soldierLOC = gameSPRITES.spriteREGISTER.get<spriteOBJECT>(soldier).spriteLOCATION;
		if (isPOINT_WITHIN_BOUNDS(globalPOS, soldierLOC.POS, soldierLOC.ROT, 64, 128))
		{
			curSELECTED_SOLDIER = soldier;
			seclectedSOLDIER = true;
			//gameSPRITES.spriteREGISTER.destroy(soldier);
		}
	}
		
	if (!seclectedSOLDIER)//If nothing found we select nothing
	{
		curSELECTED_SOLDIER = nothing;
	}
}