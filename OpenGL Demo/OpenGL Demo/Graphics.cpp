#include "Graphics.h"

WindowInfo windowInfo;
Camera camera;
ExampleCube exampleCube;

GLfloat currentTime;
GLfloat deltaTime;
GLfloat pasttime;

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//camera

	camera.Tick(windowInfo.screenSize, deltaTime);

	//skybox here

	//scissor test
	//glEnable(GL_SCISSOR_TEST);
	//glScissor(0, 100, 1280, 510); //void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)

	//render objects

	exampleCube.Render();

	//Stencil
	//glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);

	//glDisable(GL_SCISSOR_TEST);

	glutSwapBuffers();
}

void Update() {

	currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (currentTime - pasttime) * 0.1f;
	pasttime = currentTime;

	camera.camPos += glm::vec3(0.1f, 0.1f, 0.1f);

	Render();
}

void Initalize(int argc, char** argv) {

	Console_OutputLog(L"Starting OpenGL Service...", LOGINFO);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 16); 
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_STENCIL_TEST);
	
	glutInitWindowPosition(50, 50);

	glutInitWindowSize(static_cast<int>(windowInfo.screenSize.x), static_cast<int>(windowInfo.screenSize.y));
	glutCreateWindow("OpenGL Demo");

	if (glewInit() != GLEW_OK)
	{
		Console_OutputLog(L"Cannot Start OpenGL Service", LOGFATAL);
		system("pause");
		exit(0);
	}

	glClearColor(1.0, 0.0f, 0.0f, 1.0f);

	//Camera

	camera.initializeCamera();

	Console_OutputLog(L"Creating Objects...", LOGINFO);

	//Create Objects

	exampleCube.Init();

	//Start loops

	Console_OutputLog(L"Finished Creating Objects", LOGINFO);

	Console_OutputLog(L"Application Initalized, Starting...", LOGINFO);

	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	//glutKeyboardFunc(Input::KeyboardDown);
	//glutKeyboardUpFunc(Input::KeyboardUp);

	glutMainLoop();
}

void ExampleCube::Init()
{

	Console_OutputLog(L"Creating an example cube...", LOGINFO);

	GLfloat Cvertices[] = {
	-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f,  1.0f,		1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,
	1.0f,  1.0f,  1.0f,		1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,		1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,
	1.0f,  1.0f, -1.0f,		1.0f, 1.0f, 1.0f,
	};

	GLuint Cindices[] =
	{
		0, 1, 2,
		3, 7, 1,
		5, 4, 7,
		6, 2, 4,
	};

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


	this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs");

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//create pointers

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),
		(GLvoid*)0
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),
		(GLvoid*)(3 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),
		(GLvoid*)(6 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(2);
}

void ExampleCube::Render()
{
	glUseProgram(program);
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
