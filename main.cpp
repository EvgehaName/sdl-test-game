#include <iostream>
#include <SDL3/SDL.h>


int main()
{
    int numDrivers = SDL_GetNumVideoDrivers();
    std::cout << "Available video drivers: " << numDrivers << std::endl;

    for (int i = 0; i < numDrivers; ++i) {
        std::cout << "Driver " << i << ": " << SDL_GetVideoDriver(i) << std::endl;
    }
    SDL_Window *window;
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);
    const char* currentDrivers = SDL_GetCurrentVideoDriver();
    std::cout << "Selected driver: " << currentDrivers << std::endl;
    window = SDL_CreateWindow(
        "SDL Window", 500, 500, SDL_WINDOW_OPENGL);

    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}