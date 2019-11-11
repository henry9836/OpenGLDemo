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

#include <random>
#include <vector>

#include "Camera.h"

static float randFloat() {
	float r = (float)rand() / (double)RAND_MAX;
	return r;
}

class Particle {
public:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0,0,0);
	float elapsedTime = 0;
	float speed = 1;
	float lifeTime = 100;
	int id = 0;
	Camera* camera;

	Particle(glm::vec3 initalPos, int _id, float _speed, Camera* _cam, float _elapsedTime) {
		this->position = initalPos;
		this->id = _id;
		this->speed = _speed;
		this->camera = _cam;
		this->elapsedTime = _elapsedTime;
	}

	void Tick(float deltaTime) {
		elapsedTime += deltaTime;
		//if we are beyond our lifetime
		if (elapsedTime > lifeTime) {
			//Reset position
			this->position = glm::vec3(0, 0, 0);
			//Give random velo
			this->velocity = glm::vec3(0.25 * cos(this->id * .0167) + 0.25f * randFloat() - 0.125f, 1.5f + 0.25f * randFloat() - 0.125f, 0.25 * sin(this->id * .0167) + 0.25f * randFloat() - 0.125f);
			//Reset timer and set random lifetime
			this->elapsedTime = 0;
			this->lifeTime = randFloat() + 0.125;
		}
		else {
			//Update pos of particle
			this->velocity += -0.2 * 0.0167f;
			this->position += velocity;
		}

	}

};

class ParticleSystem {
public:
	vector<Particle*> particles;
	vector<glm::vec3> vPosition;

	glm::vec3 scale = glm::vec3(10, 10, 10);
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::mat4 rotation;

	Camera* camera = nullptr;
	GLuint VAO, VBO, texture, program;
	float numParticles = 10;
	string name = "Untitled Particle System";

	ParticleSystem(glm::vec3 originPos, Camera* _cam, string texFileName, int numofParticles, string _name) {
		Console_OutputLog(to_wstring("Creating Particle System: " + _name), LOGINFO);
		this->name = _name;
		this->numParticles = numofParticles;
		this->camera = _cam;

		//Create particles
		for (size_t i = 0; i < numParticles; i++)
		{
			this->vPosition.push_back(glm::vec3(0,0,0));
			this->particles.push_back(new Particle(glm::vec3(0.25 * cos(i * .0167) + 0.25f * randFloat() - 0.125f, 2.0f + 0.25f * randFloat() - 0.125f, 0.25 * sin(i* .0167) + 0.25f * randFloat() - 0.125f), i, 1.0f, this->camera, 0.0f));
		}

		//Create VAO and VBO

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vPosition.size(), &vPosition[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	};
	~ParticleSystem() {

	};

	void Render(float deltaTime) {
		//Get new positions
		for (size_t i = 0; i < this->numParticles; i++)
		{
			this->particles.at(i)->Tick(deltaTime);
			this->vPosition.at(i) = this->particles.at(i)->position;
		}

		//MV and cam look dir is guessed

		//glm::mat4 viewMat = camera->getMV(this->position, this->scale, this->rotation);
		glm::mat4 viewMat = camera->getVP();
		glm::vec3 vQuad1, vQuad2;
		glm::vec3 vView = glm::normalize(camera->camFrontDir);

		vQuad1 = glm::normalize(glm::cross(vView, camera->camUpDir));
		vQuad2 = glm::normalize(glm::cross(vView, vQuad1));

		//Time to finally render

		glUseProgram(this->program);

		glUniform3f(glGetUniformLocation(this->program, "vQuad1"), vQuad1.x, vQuad1.y, vQuad1.z);
		glUniform3f(glGetUniformLocation(this->program, "vQuad2"), vQuad2.x, vQuad2.y, vQuad2.z);

		glUniformMatrix4fv(glGetUniformLocation(program, "vp"), 1, GL_FALSE, glm::value_ptr(camera->getVP()));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program, "Texture"), 0); 
		glBindTexture(GL_TEXTURE_2D, texture);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vPosition.size(), &vPosition[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, this->numParticles);

		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

	};


};