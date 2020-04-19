#pragma once

#include <vector>

enum class UpdateStatus;

class Module;
class m1Window;
class m1Render3D;
class m1Input;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

public:
	m1Window*	window = nullptr;
	m1Render3D* render = nullptr;
	m1Input*	input  = nullptr;

private:
	std::vector<Module*> modules;
};

extern Application* App;