#pragma once
#include "Modules/Base/Module.h"
#include <SDL_scancode.h>
#include "ExternalTools/SDL2/include/SDL_mouse.h"

#define SDL_MAX_KEYS 231 // SDL has 512 values of SDL_Scancode but we only want to have 231 as maximum

class m1Input :
	public Module
{
	enum class KeyState {
		IDLE = 0, DOWN, REPEAT, UP
	};

public:
	m1Input(bool start_enabled = true);
	~m1Input();

public:
	UpdateStatus PreUpdate() override;

	void HandleKeyboard();

	bool IsKeyDown(SDL_Scancode scancode);
	bool IsKeyRepeating(SDL_Scancode scancode);
	bool IsKeyUp(SDL_Scancode scancode);

	bool IsKeyPressed(SDL_Scancode scancode);

	bool IsMouseButtonDown(const int& button);
	bool IsMouseButtonRepeating(const int& button);
	bool IsMouseButtonUp(const int& button);

	bool IsMouseButtonPressed(const int& button);

	int GetMouseX();
	int GetMouseY();
	int GetMouseZ();

	void GetMousePosition(int* x, int* y);

private:
	KeyState* keyboard = nullptr;
	KeyState  mouse[3] = {KeyState::IDLE};
	int mouseX = 0;
	int mouseY = 0;
	int mouseZ = 0;
};
