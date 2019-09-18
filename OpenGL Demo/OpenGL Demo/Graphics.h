#pragma once
#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>
#include <fmod.hpp>
#include <iostream>
#include <time.h>
#include <vector>
#include <windows.h>
#include <cstdlib>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "ConsoleController.h"

class WindowInfo {
public:
	const glm::vec2 screenSize = {1280, 720};
};

void Initalize(int argc, char** argv);
