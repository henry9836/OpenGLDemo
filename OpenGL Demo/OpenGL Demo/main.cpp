#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <time.h>
#include <vector>

#include "ShaderLoader.h"
#include "Audio.h"
#include "Camera.h"
#include "GamerManager.h"
#include "ScreenInfo.h"
#include "Input.h"
#include "Model.h"
#include "ModelMesh.h"
#include "ObjectManager.h"
#include "ShaderLoader.h"
#include "TextLabel.h"
#include "Sprite.h"
#include "ConsoleController.h"
#include "3D.h"
#include "AI.h"

using namespace std;

//Classes

Camera mCam;
ScreenInfo mScreen;
AudioSystem mAudio;
TextLabel mScore;
TextLabel mWaveNum;
TextLabel gameOverText;
TextLabel mainText;
TextLabel mLivesText;
TextLabel highscoreText;
GameManager m_Game;
ObjectManager objManager;
LoadTexture textureLoader;

//3D Objects
Simple3DObject cubeModel;

GLfloat vertices[] //player
{
	-1.0f, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
	-1.0, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
	1.0, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
	1.0, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,

	-1.0, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	1.0, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,

	1.0, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	1.0, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,

	1.0, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	-1.0, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,

	-1.0, 0.0f, 0.5f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	-1.0, 0.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,

	0.0f, 0.5f, 0.0f,		0.0f, 1.0f, 1.0f,	0.5f, 0.0f,
};

GLuint indices[] =
{
	1, 4, 3,
	1, 3, 2,

	4, 12, 5,
	6, 12, 7,
	8, 12, 9,
	10, 12, 11,
};


//Sprites
Sprite backdropSprite;

//Models
Model tankModel;
Model shellModel;
Model basicCubeModel;


//Vectors
vector<Simple3DObject*> simple3DObjects;
vector<Sprite*> menuSprites;
vector<Sprite*> gameSprites;
vector<Model*> menuModels;
vector<Model*> mainModels;
vector<Model*> stencilModels;

//Cubemap
CubeMap cubeMap;

//Global Vars

float r = 1.0;
float b = 0.0;
float g = 0.0;

glm::vec3 rotZ = glm::vec3(0.0f, 0.0f, 1.0f);

float rotationAngle = 0.0f;
float currentTime = 0.0f;
float deltaTime = 0.0f;
float pasttime = 0.0f;

GLuint backIndices[] = {
	0, 1, 2,
	2, 3, 0,
};

GLfloat backVerts[] = {
	1.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,		//top right		0
	-1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 1.0f,	0.0f, 0.0f,		//top left		1
	-1.0f, -1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,		//bottom left	2
	1.0f, -1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//bottom right	3
};


//Render function displays everything on the screen
void Render() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glClearColor(r, g, b, 1.0);

	/* Tick */
	
	glm::vec3 rotationAxisZ = glm::vec3(1.0f, 0.0f, 0.0f);

	mCam.Tick(mScreen, deltaTime);

	backdropSprite.Tick(rotationAngle, rotationAxisZ, mCam);

	cubeMap.Update();



	int diff = 0;

	float rotationAngle = 0;

	/*
		===========
		[RENDERING]
		===========
	*/

	cubeMap.Render();
	
	/*
	==============
	[SCISSOR TEST]
	==============
	*/

	//glEnable(GL_SCISSOR_TEST);
	//glScissor((mScreen.SCR_WIDTH / 2) / 1.5, (mScreen.SCR_HEIGHT / 2) / 2, mScreen.SCR_HEIGHT / 2, mScreen.SCR_HEIGHT / 2); //void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)

	/*
	==============
	[DEPTH TEST]
	==============
	*/
	//glDepthFunc(GL_ALWAYS);
	
	cubeModel.Render(&mCam, &cubeMap, indices);

	// MAIN MENU SCENE

	if (!m_Game.gameover && m_Game.currentScreen == m_Game.MAIN) {

		tankModel.position = glm::vec3(1.0f, 1.0f, 1.0f);

		for (size_t i = 0; i < menuSprites.size(); i++)
		{
			menuSprites.at(i)->Render(&mCam, &cubeMap);
		}

		/*
		==============
		[STENCIL TEST]
		==============
		*/

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		for (size_t i = 0; i < mainModels.size(); i++)
		{
			mainModels.at(i)->Render();
		}

		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

		for (size_t i = 0; i < stencilModels.size(); i++)
		{
			stencilModels.at(i)->Render();
		}

		glDisable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);

		
		mCam.SwitchMode(mCam.ORBIT, tankModel.position, glm::vec3(-5.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 5.0f);
		mainText.Render();
		highscoreText.SetText("Current Highscore: " + std::to_string(m_Game.highscore));
		highscoreText.Render();
	}
	glStencilMask(0x00);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);

	glutSwapBuffers();
}


//Update Function
void Update() {
	//Set deltaTime
	currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
	deltaTime = ((currentTime - pasttime) * 0.001f);
	pasttime = currentTime;
	
	glutPostRedisplay();
	mAudio.Tick(); //Update Audio
	m_Game.CheckGeneralInput(m_Game); //Get Current Keyboard Input State
	if (m_Game.leave) { //Quit Game
		glutLeaveMainLoop();
	}
}

int main(int argc, char** argv) {
	
	try {
		Console_Initalize(); //Show Console Controller Banner

		Console_OutputLog(L"Initializing Game...", LOGINFO);

		m_Game.score = 0;
		m_Game.gameover = false;

		//glut init

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
		glutInitWindowPosition(100, 50);
		glutInitWindowSize((int)mScreen.SCR_WIDTH, (int)mScreen.SCR_HEIGHT);
		glutCreateWindow("The Dev Forgot To Name Me");

		if (glewInit() != GLEW_OK) {
			Console_OutputLog(L"Glew INIT FAILED! The program cannot recover from this error", LOGFATAL);
			system("pause");
			exit(0);
		}

		glutSetOption(GLUT_MULTISAMPLE, 16);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		glClearColor(1.0, 0.0, 0.0, 1.0);
		
		/*
			===============================================
			// CREATE, INITALISE AND ASSIGN GAME OBJECTS //
			===============================================
		*/

		/*
			=========
			[ AUDIO ]
			=========
		*/

		if (!mAudio.AudioInit()) {
			Console_OutputLog(L"Audio Inialistation Failed!", LOGWARN);
		}
		mAudio.Play(mAudio.AMBIENT);

		/*
			==============
			[ 3D OBJECTS ]
			==============
		*/

		cubeModel.Initalise(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "Resources/Textures/box.png", "Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs", indices, vertices, "Basic Cube", false);
		mainModels.push_back(new Model("Resources/Models/BasicCube/Cube.obj", &mCam, "Cube", rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), "Resources/Shaders/3DObjectColor.vs", "Resources/Shaders/3DObjectColor.fs"));
		stencilModels.push_back(new Model("Resources/Models/BasicCube/Cube.obj", &mCam, "[STENCIL] Cube", rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f)*1.1f, "Resources/Shaders/3DObjectColor.vs", "Resources/Shaders/3DObjectColorRed.fs"));

		mainModels.push_back(new Model("Resources/Models/BasicCube/Cube.obj", &mCam, "Fog Cube", rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), "Resources/Shaders/3DObjectFog.vs", "Resources/Shaders/3DObjectFog.fs"));

		mainModels.push_back(new Model("Resources/Models/Tank/Tank.obj", &mCam, "Tank", rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), "Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_BlinnPhong.fs"));

		/*
			============
			[ CUBEMAPS ]
			============
		*/

		cubeMap.Initalise(&mCam, "Resources/CubeMap/Witcher/", "Witcher Terrain Cube Map");

		/*
			===========
			[ SPRITES ]
		    ===========
		*/

		backdropSprite.Initalise(glm::vec3(1.0f, 1.0f, 5.0f), glm::vec3(2.0f, 2.0f, 2.0f), "Resources/Textures/back.png", "Resources/Shaders/Reflect.vs", "Resources/Shaders/Reflect.fs", backIndices, backVerts, "Backround Layer");

		menuSprites.push_back(&backdropSprite); //Assign to scene

		
		glGenVertexArrays(1, &backdropSprite.VAO);
		glBindVertexArray(backdropSprite.VAO);

		glGenBuffers(1, &backdropSprite.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backdropSprite.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backIndices), backIndices, GL_STATIC_DRAW);

		glGenBuffers(1, &backdropSprite.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, backdropSprite.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backVerts), backVerts, GL_STATIC_DRAW);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		/*
			==========
			[ CAMERA ]
			==========
		*/

		mCam.initializeCamera();

		/*
			========
			[ TEXT ]
			========
		*/
		//mScore = TextLabel(mScreen, "SCORE: 0", "Resources/Fonts/DIN1451.ttf", glm::vec2(-850.0f, 400.0f));
		//mLivesText = TextLabel(mScreen, "LIVES: 3", "Resources/Fonts/DIN1451.ttf", glm::vec2(-850.0f, 450.0f));
		//mWaveNum = TextLabel(mScreen, "WAVE: 1", "Resources/Fonts/DIN1451.ttf", glm::vec2(-850.0f, 350.0f));
		//gameOverText = TextLabel(mScreen, "GAMEOVER\nSCORE: UNKNOWN\nPress c to continue", "Resources/Fonts/DIN1451.ttf", glm::vec2(-300.0f, 450.0f));
		//mainText = TextLabel(mScreen, "The Dev Forgot To Name Me\n1. Play\n2. Quit", "Resources/Fonts/Arial.ttf", glm::vec2(-850.0f, 450.0f));
		//highscoreText = TextLabel(mScreen, "Current Highscore: " + std::to_string(m_Game.highscore) , "Resources/Fonts/Arial.ttf", glm::vec2(-850.0f, -450.0f));
		//mScore.SetScale(static_cast<GLfloat>(1.0));
		//mLivesText.SetScale(static_cast<GLfloat>(1.0));
		//mWaveNum.SetScale(static_cast<GLfloat>(1.0));
		//gameOverText.SetScale(static_cast<GLfloat>(1.0));
		//mainText.SetScale(static_cast<GLfloat>(1.0));
		//highscoreText.SetScale(static_cast<GLfloat>(1.0));

		//Addition Items To Set Up

		m_Game.mCam = &mCam;

		//Start The Game

		glutDisplayFunc(Render);

		glutIdleFunc(Update);

		glutKeyboardFunc(Input::KeyboardDown);
		glutKeyboardUpFunc(Input::KeyboardUp);

		glutSpecialFunc(Input::specialCharDown);
		glutSpecialUpFunc(Input::specialCharUp);

		Console_OutputLog(L"Game Assets Initalised. Starting Game...", LOGINFO);

		glutMainLoop();
	}

	catch (...) { //If we go here there is no recovery
		Console_OutputLog(L"Something went wrong and the application cannot recover", LOGFATAL);
		system("pause");
	}

	return 0;
}