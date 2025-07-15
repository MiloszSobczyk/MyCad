#include "MainSystem.h"

#include <iostream>

MainSystem::MainSystem()
	: scene(), cameraSystem(&scene)
{
}

void MainSystem::Update()
{
	std::cout << "MainSystem Update\n";

	cameraSystem.Update();
}