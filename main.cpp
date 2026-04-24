#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

//GAME
#include "RENDER.h"
#include "SPRITE.h"
#include "INPUT.h"

SPRITE_MANAGER SM; //SM FIRST
INPUT I(SM);
RENDER R(I);

double DT = 0;

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    I.initializeINPUT(1280, 720);
    R.initializeRENDER();

    ROTATION testROT = { 0.0, 1.0 };

    SM.createSOLDIER({ 0.0, 100.0 }, testROT, false);
    SM.createSOLDIER({ 0.0, 150.0 }, testROT, false);
    SM.createSOLDIER({ 0.0, 170.0 }, testROT, false);

    SM.createSOLDIER({ 250.0, 2000.0 }, testROT, true);
    SM.createSOLDIER({ 100.0, 2000.0 }, testROT, true);

    R.createMAP(SM);


    while (I.RUN)
    {

        // F.companyADVANCE(firstCOMPANY, 0.1);
        Uint64 frameSTART = SDL_GetPerformanceCounter();
        SDL_RenderClear(R.REND);
        I.handleINPUT(DT);//GET INPUT POLL
        //GROUP ALL PLAYER INPUT INTO ONE FUNCTION
        I.cameraMOVEMENT();//HANDLE ANY INPUT
        SM.updateGAME();


        R.renderSPRITES_ON_SCREEN(SM.spriteREGISTER, I.C);
        SDL_RenderPresent(R.REND);
        Uint64 frameEND = SDL_GetPerformanceCounter();
        DT = double(frameEND - frameSTART) / SDL_GetPerformanceFrequency();
        SM.updateDT(DT);
        //std::cout << 1.0 / DT << std::endl;
        frameSTART = frameEND;
    }

    // Clean
    SDL_DestroyRenderer(R.REND);
    SDL_DestroyWindow(R.window);
    SDL_Quit();

    return 0;
}