#include "INPUT.h"
#include <algorithm>
#include "PLAYER.h"

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
				std::cout << "Left Click detetcted... do sum\n";
			}
		}
		if (input.type == SDL_EVENT_MOUSE_WHEEL)
		{
			C.zoom = std::clamp(C.zoom + (0.07 * input.wheel.y), 0.25, 9.0);
		}
	}

	SDL_GetMouseState(&M.screenMOUSE_POS.x, &M.screenMOUSE_POS.y);

	KEYS = SDL_GetKeyboardState(nullptr);

}

//CAMERA

void INPUT::getCAM_OFFSET()
{
	C.offSET.x = WINDOW_CENTER_X - C.camPOS.x * C.zoom;
	C.offSET.y = WINDOW_CENTER_Y - C.camPOS.y * C.zoom;
}