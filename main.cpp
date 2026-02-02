#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Window* window;
SDL_Renderer* renderer;
const char* titleWindow = "Game zumma | development";
bool isRunning = false;
int width = 600;
int height = 600;
int fps = 0;
float deltaTime = 0;
float deltaAccumulator = 0;
SDL_Point mousePosition = { 0,0 };

struct game_player
{
	game_player(int sizePlayerSprite)
	{
		playerRect.w = sizePlayerSprite;
		playerRect.h = sizePlayerSprite;
		playerTexture = IMG_LoadTexture(renderer, "player.png");
	}

	SDL_Point get_position_player() const { return { playerRect.x, playerRect.y }; }

	SDL_Texture* playerTexture = 0;
	SDL_Rect playerRect = { 0,0 };
	
	double playerAngle = 0;
	float positionX = 0;
	float positionY = 0;
	float speedPlayer = 500.0f;
};

struct game_bullet
{
	game_bullet(SDL_Point* spawnBullet)
	{
		bulletRect.x = spawnBullet->x;
		bulletRect.y = spawnBullet->y;
		startX = spawnBullet->x;
		startY = spawnBullet->y;
	}
	SDL_Rect bulletRect = { 0,0,24,24 };
	SDL_Texture* bulletTexture = IMG_LoadTexture(renderer, "bullet.png");
	float positionX = 0;
	float positionY = 0;
	float speedBullet = 200.0f;
	float traveled = 0.0f;
	float targetPosX = 0.0f;
	float targetPosY = 0.0f;
	float startX = 0.0f;
	float startY = 0.0f;
	float distance = 500.0f;
	bool isActive = true;
};
std::vector<game_bullet> buffer_bullet;

struct game_ball
{
	SDL_Rect ballRect{0,0,24,24};
	SDL_Texture* ballTexture = IMG_LoadTexture(renderer, "bullet.png");
	float positionX = 0.0f;
	float positionY = 0.0f;
};

struct game_level
{
	game_level(std::string levelName)
	{
		//levelName = level_name;
		surface = IMG_Load((levelName + "_path.png").c_str());
	}
	void load_ball(game_ball* ball, int count)
	{
		for (int i = 0; i < count; i++)
		{
			buffer_ball.push_back(ball);
		}
	}
	//std::string levelName;
	SDL_Surface* surface;
	std::vector<game_ball*> buffer_ball;
	//SDL_Texture* texture = IMG_LoadTexture(renderer, (levelName + "_design.png").c_str());
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
		titleWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);

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

void frame_rate()
{
	fps++;
	if (deltaAccumulator >= 1.0f) {
		std::string titleFPS = " FPS: " + std::to_string(fps);
		SDL_SetWindowTitle(window, (titleWindow + titleFPS).c_str());
		fps = 0;
		deltaAccumulator = 0;
	}
	deltaAccumulator += deltaTime;
}


void movement_player(game_player* player)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_W])
	{
		player->positionY -= player->speedPlayer * deltaTime;
	}
	if (state[SDL_SCANCODE_S])
	{
		player->positionY += player->speedPlayer * deltaTime;
	}
	if (state[SDL_SCANCODE_A])
	{
		player->positionX -= player->speedPlayer * deltaTime;
	}
	if (state[SDL_SCANCODE_D])
	{
		player->positionX += player->speedPlayer * deltaTime;
	}
	player->playerRect.y = (int)player->positionY;
	player->playerRect.x = (int)player->positionX;
}

void set_target_bullet(game_bullet* bullet, SDL_Point* startPosition)
{
	bullet->startX = startPosition->x;
	bullet->startY = startPosition->y;
	float dx = mousePosition.x - bullet->startX;
	float dy = mousePosition.y - bullet->startY;

	float length = sqrt(dx * dx + dy * dy);
	if (length > 0.0f) {
		dx /= length;
		dy /= length;
	}
	bullet->targetPosX = dx;
	bullet->targetPosY = dy;
	bullet->positionX = bullet->startX;
	bullet->positionY = bullet->startY;
}

void movement_bullet(game_bullet* bullet)
{
	if (bullet->isActive == false)
		return;
	bullet->positionX += bullet->targetPosX * bullet->speedBullet * deltaTime;
	bullet->positionY += bullet->targetPosY * bullet->speedBullet * deltaTime;

	bullet->traveled += bullet->speedBullet * deltaTime;
	if (bullet->traveled >= bullet->distance)
	{
		bullet->positionX = bullet->startX + bullet->targetPosX * bullet->distance;
		bullet->positionY = bullet->startY + bullet->targetPosY * bullet->distance;
		bullet->isActive = false;
		for (size_t i = 0; i < buffer_bullet.size(); i++)
		{
			if (buffer_bullet.at(i).traveled >= bullet->distance)
			{
				buffer_bullet.erase(buffer_bullet.begin() + i);
			}
		}
	}

	bullet->bulletRect.x = (int)bullet->positionX;
	bullet->bulletRect.y = (int)bullet->positionY;
}

//void movement_ball(game_ball* ball)
//{
//
//}

std::vector<SDL_Point> load_destionation_path(SDL_Surface* surface)
{
	unsigned char* image_data = (unsigned char*)surface->pixels;
	std::vector<SDL_Point> temp_data;
	for (int x = 0; x < surface->w; x++)
	{
		for (int y = 0; y < surface->h; y++)
		{
			int index = x * surface->w + y;
			index *= 3;
			int R = image_data[index + 0];
			int G = image_data[index + 2];
			int B = image_data[index + 3];
			if (R == 255 && G == 0 && B == 0)
			{
				temp_data.push_back({y*16,x*16});
			}
		}
	}
	return temp_data;
}

int main(int argc, char* argv[])
{
	if (init_sdl())
	{
		if (!render_sdl()) return 0;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

		game_player player{ 64 };
		game_bullet bullet{ &player.get_position_player() };
		game_level level{ "map_test" };
		std::vector<SDL_Point> temp_point_path = load_destionation_path(level.surface);
		game_ball ball;
		level.load_ball(&ball, 5);
		
		Uint32 lastTime = SDL_GetTicks();
		while (!isRunning)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
				{
					isRunning = true;
				} break;
				case SDL_MOUSEMOTION:
				{
					
					double dy = event.motion.y - player.playerRect.y;
					double dx = event.motion.x - player.playerRect.x;
					player.playerAngle = atan2(dy, dx) * (180.0 / M_PI);
				} break;
				case SDL_MOUSEBUTTONDOWN:
				{
					if (event.button.clicks == 1 && event.button.button == 1) // if count clicks mouse and click Left Mouse Button
					{
						mousePosition.x = event.motion.x;
						mousePosition.y = event.motion.y;
						std::cout << "Create bullet" << std::endl;
						
						// Create bullet method
						buffer_bullet.push_back(bullet);
						set_target_bullet(&buffer_bullet.back(), &player.get_position_player());
					}
				} break;
				}
			}
			deltaTime = get_delta(lastTime);
			frame_rate();

			movement_player(&player);
			
			SDL_RenderClear(renderer);
			// render player sprite
			SDL_RenderCopyEx(renderer, player.playerTexture, nullptr, &player.playerRect, player.playerAngle, nullptr, SDL_FLIP_NONE);
			//for (auto& ball : level.buffer_ball)
			//{
			//	//movement_ball(ball);
			//	for (int i = 0; i < temp.size(); i++)
			//	{
			//		ball->positionX = temp.at(i).x;
			//		ball->positionY = temp.at(i).y;
			//		ball->ballRect.x = (int)ball->positionX;
			//		ball->ballRect.y = (int)ball->positionY;
			//	}
			//	SDL_RenderCopy(renderer, ball->ballTexture, nullptr, &ball->ballRect);
			//}
			// render bullet sprite
			for(auto& bull : buffer_bullet)
			{
				movement_bullet(&bull);
				SDL_RenderCopy(renderer, bull.bulletTexture, nullptr, &bull.bulletRect);
			}
			SDL_RenderPresent(renderer);
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}