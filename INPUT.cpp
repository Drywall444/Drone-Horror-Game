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
				checkLEFT_CLICK_RETURN_SPRITE(screenTO_WORLD_POS(M.screenMOUSE_POS));
				std::cout << "Left Click detetcted... do sum\n";

				//Player checkSELECTION
			}
			if (input.button.button == SDL_BUTTON_RIGHT)
			{
				if (seclectedSOLDIER)
				{
					checkRIGHT_CLICK(screenTO_WORLD_POS(M.screenMOUSE_POS));
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
			C.zoom = std::clamp(C.zoom + (scrollWHEEL_SENS * input.wheel.y), 0.09, 10.0); //clamps to min and max zoom
			getCAM_OFFSET();
		}
		if (input.type == SDL_EVENT_KEY_DOWN)
		{
			if (input.key.key == SDLK_ESCAPE) {
				std::cout << "hit escape\n";
				RUN = false;
			}
			if (input.key.scancode == SDL_SCANCODE_T)
			{
				std::cout << "Toggle Tree\n";
				gameSPRITES.toggleTREE_TOPS();
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

void INPUT::checkLEFT_CLICK_RETURN_SPRITE(SDL_FPoint globalPOS)
{
	auto spriteVIEW = gameSPRITES.spriteREGISTER.view<spriteOBJECT>();
	seclectedSOLDIER = false;

	for (auto& sprite : spriteVIEW)
	{
		auto& soldier = gameSPRITES.spriteREGISTER.get<spriteOBJECT>(sprite);
		if (isPOINT_WITHIN_BOUNDS(globalPOS, soldier.spriteLOCATION.POS, soldier.spriteLOCATION.ROT, soldier.texW, soldier.texH))
		{
			if (gameSPRITES.spriteREGISTER.all_of<soldierOBJECT>(sprite)) //if this is a soldier
			{
				curSELECTED_SOLDIER = sprite;
				seclectedSOLDIER = true;
				//gameSPRITES.spriteREGISTER.destroy(soldier);
			}
		}
	}
		
	if (!seclectedSOLDIER)//If nothing found we select nothing
	{
		curSELECTED_SOLDIER = nothing;
	}
}

void INPUT::checkRIGHT_CLICK(SDL_FPoint globalPOS)
{
	if (!gameSPRITES.spriteREGISTER.valid(curSELECTED_SOLDIER))
	{
		//not valid 
		curSELECTED_SOLDIER = entt::null;
		std::cout << "Selected Soldier Died\n";
		return;
	}

	auto spriteVIEW = gameSPRITES.spriteREGISTER.view<spriteOBJECT>();
	auto& curSOLDIER_INFO = gameSPRITES.spriteREGISTER.get<soldierOBJECT>(curSELECTED_SOLDIER);
	bool clickBUILDING = false;
	entt::entity clickedBUILDING_ENTITY = entt::null;

	for (auto& sprite : spriteVIEW) //Check if we right clicked a building
	{
		auto& curSPRITE = gameSPRITES.spriteREGISTER.get<spriteOBJECT>(sprite);
		if (isPOINT_WITHIN_BOUNDS(globalPOS, curSPRITE.spriteLOCATION.POS, curSPRITE.spriteLOCATION.ROT, curSPRITE.texW, curSPRITE.texH))
		{
			if (gameSPRITES.spriteREGISTER.all_of<BUILDING>(sprite)) //if this is a building
			{
				std::cout << "Clicked on building\n";
				clickBUILDING = true;
				auto& clickedON_BUILDING = gameSPRITES.spriteREGISTER.get<BUILDING>(sprite);
				clickedBUILDING_ENTITY = sprite;
				if (!clickedON_BUILDING.isOCCUPIED() && seclectedSOLDIER == true) //if not in a building and we click on a building move selc soldier if not occupied
				{
					if (curSOLDIER_INFO.curBUILDING == entt::null)
					{
						gameSPRITES.soldierMOVE_TO_BUILDING(curSELECTED_SOLDIER, sprite); //If we right click, have a selcSOLDIER and the building is unonccupied we move a soldier there
					}
					else {
						gameSPRITES.soldierMOVE_OUT_BUILDING(curSOLDIER_INFO.curBUILDING, globalPOS, clickBUILDING, clickedBUILDING_ENTITY);
					}
				}
			}
		}
	}

	if (curSOLDIER_INFO.curBUILDING != entt::null) //if inside a building
	{
		gameSPRITES.soldierMOVE_OUT_BUILDING(curSOLDIER_INFO.curBUILDING, globalPOS, clickBUILDING, clickedBUILDING_ENTITY);
	}
	else if (!clickBUILDING)
	{
		if (seclectedSOLDIER) { gameSPRITES.ORDER_soldierMOVE_TO_POINT(curSELECTED_SOLDIER, globalPOS); }
	}

}
