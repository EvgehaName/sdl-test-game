#include <iostream>
#include <string>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Window* window;
SDL_Renderer* renderer;
bool isRunning = false;
int width = 600;
int height = 600;
int fps = 0;
float deltaAccumulator = 0;

struct game_player
{
    SDL_Texture* playerTexture;
    SDL_Rect playerRect;
    double playerAngle;
    float positionX = 0;
    float positionY = 0;
    float speedPlayer = 500.0f;
};

bool init_sdl()
{
    // Output count drivers and elem driver
    int num_drivers_video = SDL_GetNumVideoDrivers();
    std::cout << "Count drivers: " << num_drivers_video << std::endl;

    for (int i = 0; i < num_drivers_video; ++i) {
        std::cout << "Driver " << i << ": " << SDL_GetVideoDriver(i) << std::endl;
    }

    // SDL init video and events
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    const char* currentDrivers = SDL_GetCurrentVideoDriver();
    std::cout << "Selected driver: " << currentDrivers << std::endl;
    
    // SDL create window (create window context to flags - SDL_WINDOW_OPENGL or SDL_WINDOW_VULKAN. 0 - default)
    window = SDL_CreateWindow(
        "Game zumma | development", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

    SDL_SetWindowResizable(window, SDL_TRUE);

    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create SDL window: %s", SDL_GetError());
        return 0;
    }
    return 1;
}

bool render_sdl()
{
    // SDK render to window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC); // SDL_RENDERER_PRESENTVSYNC - enable vsync limit frame rate. 0 - default

    if (renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to renderer SDL window: %s", SDL_GetError());
        return 0;
    }
    return 1;
}

float get_delta(Uint32& lastTime)
{
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    return deltaTime;
}

void frame_rate(float& deltaTime)
{
    fps++;
    if (deltaAccumulator >= 1.0f) {
        std::string titleFPS = "FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(window, titleFPS.c_str());
        fps = 0;
        deltaAccumulator = 0;
    }
    deltaAccumulator += deltaTime;
}


int main(int argc, char* argv[])
{
    if (init_sdl())
    {
        if (!render_sdl()) return 0;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

        game_player player;
        player.playerTexture = IMG_LoadTexture(renderer, "player.png");
        player.playerRect.x = 0;
        player.playerRect.y = 0;
        player.playerRect.h = 64;
        player.playerRect.w = 64;

        Uint32 lastTime = SDL_GetTicks();
        while (!isRunning)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    isRunning = true;
                    break;
                case SDL_MOUSEMOTION:
                    //std::cout << "mouse X: " << event.motion.x << "mouse Y: " << event.motion.y << std::endl;
                    //std::cout << "angle: " << angle << std::endl;
                    double dy = event.motion.y - player.playerRect.y;
                    double dx = event.motion.x - player.playerRect.x;
                    player.playerAngle = atan2(dy, dx) * (180.0 / M_PI);
                    break;
                }
            }
            float deltaTime = get_delta(lastTime);
            frame_rate(deltaTime);

            const Uint8* state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_W])
            {
                player.positionY -= player.speedPlayer * deltaTime;
            }
            if (state[SDL_SCANCODE_S])
            {
                player.positionY += player.speedPlayer * deltaTime;
            }
            if (state[SDL_SCANCODE_A])
            {
                player.positionX -= player.speedPlayer * deltaTime;
            }
            if (state[SDL_SCANCODE_D])
            {
                player.positionX += player.speedPlayer * deltaTime;
            }
            player.playerRect.y = (int)player.positionY;
            player.playerRect.x = (int)player.positionX;

            SDL_RenderClear(renderer);
            SDL_RenderCopyEx(renderer, player.playerTexture, nullptr, &player.playerRect, player.playerAngle, nullptr, SDL_FLIP_NONE);
            SDL_RenderPresent(renderer);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}