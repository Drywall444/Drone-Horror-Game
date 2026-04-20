#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

//GAME
#include "RENDER.h"
#include "SPRITE.h"
#include "INPUT.h"

RENDER R;
INPUT I;
SPRITE_MANAGER SM;

double DT = 0;

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }


    R.initializeRENDER(1280, 720);
    SM.spriteCREATE(HUMAN);


    while (I.RUN)
    {

        // F.companyADVANCE(firstCOMPANY, 0.1);
        Uint64 frameSTART = SDL_GetPerformanceCounter();
        I.handleINPUT(DT);


        SDL_RenderClear(R.REND);

        R.renderSPRITES_ON_SCREEN(SM.spriteREGISTER, I.C);
        SDL_RenderPresent(R.REND);
        Uint64 frameEND = SDL_GetPerformanceCounter();;
        DT = double(frameEND - frameSTART) / SDL_GetPerformanceFrequency();
        std::cout << 1.0 / DT << std::endl;
        frameSTART = frameEND;
    }

    // Clean
    SDL_DestroyRenderer(R.REND);
    SDL_DestroyWindow(R.window);
    SDL_Quit();

    return 0;
}