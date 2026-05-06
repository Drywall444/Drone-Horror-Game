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
RENDER R(I, SM);

double DT = 0;

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    I.initializeINPUT(1280, 720);
    R.initializeRENDER();

    ROTATION testROT = { -1.0, 0.0 };

    SM.createSOLDIER({ 0.0, 100.0 }, testROT, true);
    SM.createSOLDIER({ 100.0, 150.0 }, testROT, true);
    //SM.createSOLDIER({ 0.0, 170.0 }, testROT, false);

    for (int i = 0; i < 15;i++)
    {
        SM.createSOLDIER({ (100.0f * float(i)) + 100.0f, 2000.0f }, testROT, false);
    }

    SM.createBUILDING({ 60.0, 60.0 }, testROT, SM.FOXHOLE);
    SM.createBUILDING({ 450.0, 68.0 }, testROT, SM.FOXHOLE);
    SM.createBUILDING({ 600.0, 68.0 }, testROT, SM.DUGOUT);

    R.createMAP(SM);


    while (I.RUN)
    {

        Uint64 frameSTART = SDL_GetPerformanceCounter();

        SDL_RenderClear(R.REND);
        I.handleINPUT(DT);
        I.cameraMOVEMENT();
        SM.updateGAME();
        R.renderSPRITES_ON_SCREEN(SM.spriteREGISTER, I.C);
        SDL_RenderPresent(R.REND); // blocks here if vsync on

        Uint64 frameEND = SDL_GetPerformanceCounter();
        SM.updateDT(DT);
        DT = (double)(frameEND - frameSTART) / SDL_GetPerformanceFrequency();
        std::cout << "FPS: " << (int)(1.0 / DT) << "\n";
    }

    // Clean
    SDL_DestroyRenderer(R.REND);
    SDL_DestroyWindow(R.window);
    SDL_Quit();

    return 0;
}