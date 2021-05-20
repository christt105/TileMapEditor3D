#pragma once

struct SDL_Window;
typedef void* SDL_GLContext;

class App
{
public:
	App();
	~App();

	void Init();
	void CleanUp();

	void Update();

	bool quit = false;

private:
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	unsigned int VAO = 0u;
	unsigned int VBO = 0u;
};

