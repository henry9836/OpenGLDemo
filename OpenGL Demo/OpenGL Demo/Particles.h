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
#include <algorithm>

#include "Camera.h"

static float randFloat() {
	float r = (float)rand() / (double)RAND_MAX;
	return r;
}

class Particle {
public:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0,0,0);
	glm::vec3 originPos = glm::vec3(0, 0, 0);
	Camera* camera;
	float elapsedTime = 0;
	float speed = 1;
	float lifeTime = 5;
	float distanceToCam = 1;
	int id = 0;

	Particle(glm::vec3 initalPos, int _id, float _speed, Camera* _cam, float _elapsedTime, glm::vec3 _origin) {
		this->position = initalPos;
		this->id = _id;
		this->speed = _speed;
		this->camera = _cam;
		this->elapsedTime = _elapsedTime;
		this->originPos = _origin;
	}

	void Tick(float deltaTime) {
		elapsedTime += deltaTime;
		distanceToCam = glm::distance(this->camera->camPos, this->position);
		//if we are beyond our lifetime
		if (elapsedTime > lifeTime) {
			//Reset position
			this->position = this->originPos;
			//Give random velo
			this->velocity = glm::vec3(0.25 * cos(this->id * .0167) + 0.25f * randFloat() - 0.125f, 1.5f + 0.25f * randFloat() - 0.125f, 0.25 * sin(this->id * .0167) + 0.25f * randFloat() - 0.125f);
			//Reset timer and set random lifetime
			this->elapsedTime = 0;
			this->lifeTime = randFloat() + 0.125;
		}
		else {
			//Update pos of particle
			//this->velocity += ((-5000.9 * 0.0167f) * speed)*deltaTime;
			this->velocity += glm::vec3(0.0f, -1.5f, 0.0f) * deltaTime;
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
	float size = 0.5f;
	string name = "Untitled Particle System";

	ParticleSystem(glm::vec3 originPos, Camera* _cam, string texFileName, int numofParticles, string _name, float _speed, float _size) {
		Console_OutputLog(to_wstring("Creating Particle System: " + _name), LOGINFO);
		this->name = _name;
		this->numParticles = numofParticles;
		this->camera = _cam;
		this->size = _size;

		//Create particles
		for (size_t i = 0; i < numParticles; i++)
		{
			this->vPosition.push_back(originPos);
			this->particles.push_back(new Particle(glm::vec3(0.25 * cos(i * .0167) + 0.25f * randFloat() - 0.125f, 2.0f + 0.25f * randFloat() - 0.125f, 0.25 * sin(i* .0167) + 0.25f * randFloat() - 0.125f), i, _speed, this->camera, 0.0f, originPos));
		}

		//Guess what I found the issue. Forgot to add the program line on init, great now thats fixed I have 3 hours left to do the rest
		this->program = ShaderLoader::CreateProgram("Resources/Shaders/Particle.vs", "Resources/Shaders/Particle.fs", "Resources/Shaders/Particle.gs");

		//Gen Buffers/Arrays

		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);

		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vPosition.size(), this->vPosition.data(), GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//Load Texture
		
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		int width, height;

		unsigned char* image = SOIL_load_image(
			texFileName.c_str(),
			&width, &height,
			0,
			SOIL_LOAD_RGBA
		);

		glTexImage2D(
			GL_TEXTURE_2D,
			0, GL_RGBA,
			width, height, 0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image
		);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Unbind

		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		Console_OutputLog(to_wstring("Created Particle System: " + _name), LOGINFO);

	};
	~ParticleSystem() {

	};

	bool particleComparison(Particle* a, Particle* b) {
		return (a->distanceToCam > b->distanceToCam);
	}

	void Render(float deltaTime) {
		//Get new positions
		for (size_t i = 0; i < this->numParticles; i++)
		{
			this->particles.at(i)->Tick(deltaTime);
			this->vPosition.at(i) = this->particles.at(i)->position;
		}

		//Sort particles to avoid bad z ordering
		//std::sort(particles.begin(), particles.end(), particleComparison); //Doesn't work for some reason
		std::sort(particles.begin(), particles.end(), [&](Particle* a, Particle* b) { return a->distanceToCam > b->distanceToCam; });


		//Update positions in bufffer
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vPosition.size(), this->vPosition.data(), GL_STATIC_DRAW);


		glUseProgram(this->program);

		//Binding the array
		glBindVertexArray(this->VAO);

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Setting and binding the correct texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		glUniform1i(glGetUniformLocation(this->program, "tex"), 0);

		//Get Camera Info
		glm::mat4 viewMat = camera->getVP();
		glm::vec3 vQuad1, vQuad2;
		glm::vec3 vView = glm::normalize(camera->camFrontDir);
		glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 ModelMatrix = TranslationMatrix * camera->getVP();

		//Make Quads
		vQuad1 = glm::cross(vView, camera->camUpDir);
		vQuad1 = glm::normalize(vQuad1);
		vQuad2 = glm::cross(vView, vQuad1);
		vQuad2 = glm::normalize(vQuad2);

		//Pass in Quads
		glUniform3f(glGetUniformLocation(this->program, "vQuad1"), vQuad1.x, vQuad1.y, vQuad1.z);
		glUniform3f(glGetUniformLocation(this->program, "vQuad2"), vQuad2.x, vQuad2.y, vQuad2.z);

		//Pass in VP and Size of particle
		glUniformMatrix4fv(glGetUniformLocation(this->program, "vp"), 1, GL_FALSE, glm::value_ptr(camera->getVP()));
		glUniform1f(glGetUniformLocation(this->program, "size"), this->size);

		glDisable(GL_CULL_FACE);
		glPointSize(1.0f);
		glDrawArrays(GL_POINTS, 0, this->particles.size());

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		glBindVertexArray(0);


		////OLD

		//glm::mat4 viewMat = camera->getVP();
		//glm::vec3 vQuad1, vQuad2;
		//glm::vec3 vView = glm::normalize(camera->camFrontDir);

		//vQuad1 = glm::normalize(glm::cross(vView, camera->camUpDir));
		//vQuad2 = glm::normalize(glm::cross(vView, vQuad1));

		////Time to finally render

		//glUseProgram(this->program);

		//glUniform3f(glGetUniformLocation(this->program, "vQuad1"), vQuad1.x, vQuad1.y, vQuad1.z);
		//glUniform3f(glGetUniformLocation(this->program, "vQuad2"), vQuad2.x, vQuad2.y, vQuad2.z);

		//glUniformMatrix4fv(glGetUniformLocation(program, "vp"), 1, GL_FALSE, glm::value_ptr(camera->getVP()));

		//glActiveTexture(GL_TEXTURE0);
		//glUniform1i(glGetUniformLocation(program, "Texture"), 0); 
		//glBindTexture(GL_TEXTURE_2D, texture);

		//glEnable(GL_BLEND);
		//glDepthMask(GL_FALSE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vPosition.size(), &vPosition[0], GL_STATIC_DRAW);

		//glBindVertexArray(VAO);
		//glDrawArrays(GL_POINTS, 0, this->numParticles);

		//glBindVertexArray(0);

		//glDepthMask(GL_TRUE);
		//glDisable(GL_BLEND);

	};


};