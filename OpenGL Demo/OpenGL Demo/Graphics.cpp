#include "Graphics.h"

WindowInfo windowInfo;

void Render() {
	//skybox here

	//scissor test
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 100, 1280, 510); //void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)

	//render objects

	glDisable(GL_SCISSOR_TEST);

}

void Update() {

}

void Initalize(int argc, char** argv) {

	Console_OutputLog(L"Starting OpenGL Service...", LOGINFO);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 16); 
	glEnable(GL_MULTISAMPLE);

	
	glutInitWindowPosition(50, 50);

	glutInitWindowSize(static_cast<int>(windowInfo.screenSize.x), static_cast<int>(windowInfo.screenSize.y));
	glutCreateWindow("OpenGL Demo");

	if (glewInit() != GLEW_OK)
	{
		Console_OutputLog(L"Cannot Start OpenGL Service", LOGFATAL);
		system("pause");
		exit(0);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClearColor(1.0, 0.5f, 0.5f, 1.0f);

	Console_OutputLog(L"Application Initalized, Starting...", LOGINFO);

	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	//glutKeyboardFunc(Input::KeyboardDown);
	//glutKeyboardUpFunc(Input::KeyboardUp);

	glutMainLoop();
}
