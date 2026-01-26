#include <iostream>
#include <string>
#include <chrono>
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
        "SDL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, 0);
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    if(renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to renderer SDL window: %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_Surface *surface = IMG_Load("player.png");
    playerTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);


    
    //Uint32 lastTime = SDL_GetTicks();
    //Uint32 fpsTimer = SDL_GetTicks();
    std::chrono::time_point lastTime = std::chrono::system_clock::now();
    //Uint32 fpsTimer = SDL_GetTicks();
    std::chrono::time_point fpsTimer = std::chrono::system_clock::now();
    float limitFPS = 60.0f;
    float targetLimit = limitFPS / 1000;
    int frameCount = 0;
    int fps = 0;
    float deltaTime = 0;
    float speedPlayer = 500.0f;
    float posX = 0.0f;
    float posY = 0.0f;

    double angle = 0;
    while (!done) {
        //Uint32 currentTime = SDL_GetTicks();
        std::chrono::time_point currentTime = std::chrono::system_clock::now();
        std::chrono::duration<float> tim = (currentTime - lastTime);
        deltaTime = tim.count();
        lastTime = currentTime;
        std::cout << "DELTA: " << deltaTime << std::endl;


        frameCount++;
        std::chrono::duration<float> dd = currentTime - fpsTimer;
        if (dd.count() >= 1.0f) {
            fps = frameCount;
            std::string titleFPS = "FPS: " + std::to_string(fps);
            SDL_SetWindowTitle(window, titleFPS.c_str());
            frameCount = 0;
            fpsTimer = currentTime;
        }
        
        SDL_Event event;

        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            done = true;
            break;
        case SDL_MOUSEMOTION:
            //std::cout << "mouse X: " << event.motion.x << "mouse Y: " << event.motion.y << std::endl;
            //std::cout << "angle: " << angle << std::endl;
            double dy = event.motion.y - dRectPlayer.y;
            double dx = event.motion.x - dRectPlayer.x;
            angle = atan2(dy, dx) * (180.0 / M_PI);
            break;
        }
        dRectPlayer.h = 64;
        dRectPlayer.w = 64;
       
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W])
        {
            posY -= speedPlayer * deltaTime;
        }
        if (state[SDL_SCANCODE_S])
        {
            posY += speedPlayer * deltaTime;
        }
        if (state[SDL_SCANCODE_A])
        {
             posX -= speedPlayer * deltaTime;
        }
        if (state[SDL_SCANCODE_D])
        {
            posX += speedPlayer * deltaTime;
        }
        dRectPlayer.y = (int)posY;
        dRectPlayer.x = (int)posX;

        SDL_RenderClear(renderer);
        SDL_RenderCopyEx(renderer, playerTexture, nullptr, &dRectPlayer, angle, nullptr, SDL_FLIP_NONE);
        SDL_RenderPresent(renderer);
        //std::cout << "X: " << dRectPlayer.x << " "  << "Y: " << dRectPlayer.y <<std::endl;
        SDL_Delay(3);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}