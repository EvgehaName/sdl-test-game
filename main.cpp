#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main(int argc, char* argv[])
{
    int numDrivers = SDL_GetNumVideoDrivers();
    std::cout << "Available video drivers: " << numDrivers << std::endl;

    for (int i = 0; i < numDrivers; ++i) {
        std::cout << "Driver " << i << ": " << SDL_GetVideoDriver(i) << std::endl;
    }
    SDL_Window *window;
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    const char* currentDrivers = SDL_GetCurrentVideoDriver();
    std::cout << "Selected driver: " << currentDrivers << std::endl;
    window = SDL_CreateWindow(
        "SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_OPENGL);
    SDL_SetWindowResizable(window, SDL_TRUE);
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer;
    SDL_Texture *playerTexture;
    SDL_Rect dRectPlayer;
    
    dRectPlayer.x = 0;
    dRectPlayer.y = 0;

    renderer = SDL_CreateRenderer(window, -1, 0);

    if(renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to renderer SDL window: %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_Surface *surface = IMG_Load("player.png");
    playerTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    int FPS = 60;
    int frameDelay = 1000 / FPS; 
    Uint32 frameStart;
    int frameTime;
    double angle = 0;
    while (!done) {
        frameStart = SDL_GetTicks();
        SDL_Event event;
        SDL_PumpEvents();
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            done = true;
            break;
        case SDL_MOUSEMOTION:
            std::cout << "mouse X: " << event.motion.x << "mouse Y: " << event.motion.y << std::endl;
            std::cout << "angle: " << angle << std::endl;
            double dy = event.motion.y - dRectPlayer.y;
            double dx = event.motion.x - dRectPlayer.x;
            angle = atan2(dy, dx) * (180.0 / M_PI);
            break;
        }
        dRectPlayer.h = 64;
        dRectPlayer.w = 64;
        dRectPlayer.x = 500 / 2;
        dRectPlayer.y = 500 / 2;

        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, playerTexture, nullptr, &dRectPlayer, angle, nullptr, SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
        std::cout << "X: " << dRectPlayer.x << " "  << "Y: " << dRectPlayer.y <<std::endl;
        frameTime = SDL_GetTicks() - frameStart;
        if(frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}