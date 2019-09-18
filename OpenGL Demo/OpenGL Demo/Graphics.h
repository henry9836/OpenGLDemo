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

class WindowInfo {
public:
	const glm::vec2 screenSize = {1280, 720};
};

class ExampleCube {
public:
	/*const GLfloat vertices[48] = {
		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
		1.0f,  1.0f, -1.0f,		1.0f, 1.0f, 1.0f,
	};

	const GLuint indices[14] =
	{
		0, 1, 2, 
		3, 7, 1, 
		5, 4, 7, 
		6, 2, 4, 
		0, 1
	};*/

	

	GLuint EBO;
	GLuint VBO;
	GLuint VAO;
	GLuint program;
	
	void Init();

	void Render();
};

void Initalize(int argc, char** argv);
