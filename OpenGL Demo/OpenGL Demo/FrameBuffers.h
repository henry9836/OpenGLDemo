#pragma once
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "ConsoleController.h"
#include "ScreenInfo.h"

ScreenInfo screen;

GLfloat frameVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

class FrameBuffer {
public:
	//Create Frame Buffer
	void Initalize() {
		Console_OutputLog(L"Creating Frame Buffer...", LOGINFO);
		

		//Create the VAO and VBO
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);


		//Create program
		program = ShaderLoader::CreateProgram("Resources/Shaders/POST.vs", "Resources/Shaders/POST.fs");

		//Set up the RBO
		glGenFramebuffers(1, &buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer);

		glGenTextures(1, &textureBuffer);
		glBindTexture(GL_TEXTURE_2D, textureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen.SCR_WIDTH, screen.SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen.SCR_WIDTH, screen.SCR_HEIGHT); 
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		//Did the frame buffer get created correctly?
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Console_OutputLog(L"Could Not Create Frame Buffer", LOGWARN);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	};

	//Render Frame Buffer
	void Render(float currentTime) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); 

		glUseProgram(this->program);
		glActiveTexture(GL_TEXTURE0);

		GLint currentTimeLoc = glGetUniformLocation(this->program, "currentTime");
		glUniform1f(currentTimeLoc, currentTime);
		glUniform1i(glGetUniformLocation(this->program, "renderTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, this->textureBuffer);

		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	};

	GLuint RBO;
	GLuint buffer;
	GLuint textureBuffer;
	GLuint program;

private:
	GLuint VAO;
	GLuint VBO;
};
