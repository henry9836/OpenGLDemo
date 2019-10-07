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
#include <fstream>

#include "Camera.h"
#include "ShaderLoader.h"

class GeoModel {
public:
	void Initialize() {
		GLfloat points[] = {
			0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
		};

		this->shaderProgram = ShaderLoader::CreateProgram("Resources/Shaders/GeometryShader.vs", "Resources/Shaders/GeometryShader.fs", "Resources/Shaders/GeometryShader.gs");

		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
	};

	void Render(Camera* camera) {
		glUseProgram(shaderProgram);
		glm::mat4 model; model = glm::translate(model, position);
		glm::mat4 mvp = camera->proj * camera->view * model;
		GLint vpLoc = glGetUniformLocation(shaderProgram, "mvp");
		glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	};

	GLuint VAO, VBO, EBO;
	glm::vec3 position;
	GLuint shaderProgram;

};