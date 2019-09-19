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
#include "ShaderLoader.h"
#include "Camera.h"

class WindowInfo {
public:
	const glm::vec2 screenSize = {1280, 720};
};

class ExampleCube {
public:

	GLuint EBO;
	GLuint VBO;
	GLuint VAO;
	GLuint program;
	
	void Init();

	void Render();
};

void Initalize(int argc, char** argv);
