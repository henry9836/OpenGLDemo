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
#include "ConsoleController.h"
#include "TextureLoader.h"
#include "Model.h"
#include "Input.h"
#include "AI.h"

//CubeMaps
class CubeMap {
public:

	//Create the Cubemap Object
	void Initalise(Camera* _cam, std::string _pathToCubeMap, std::string _name) {

		Console_OutputLog(to_wstring("Initalising CubeMap: " + _name), LOGINFO);

		this->camera = _cam;
		this->name = _name;

		//Create Vertcies and Indices

		GLfloat CubeMapVertices[24] = {};

		GLuint CubeMapIndices[36] = {};

		for (int i = 0; i < 8; i++)
		{
			if (i < 4)
			{
				CubeMapVertices[i * 3] = -1.0f;
				for (int j = 0; j < 4; j++)
				{
					if (j < 2)
					{
						CubeMapVertices[(j * 3) + 1] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 1] = -1.0f;
					}

					if (j % 2 == 0)
					{
						CubeMapVertices[(j * 3) + 2] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 2] = -1.0f;
					}
				}
			}
			else
			{
				CubeMapVertices[i * 3] = 1.0f;
				for (int j = 4; j < 8; j++)
				{
					if (j < 6)
					{
						CubeMapVertices[(j * 3) + 1] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 1] = -1.0f;
					}

					if (j % 2 == 0)
					{
						CubeMapVertices[(j * 3) + 2] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 2] = -1.0f;
					}
				}
			}
		}

		GLuint temp[] = {
			3,2,6,
			3,6,7,

			1,4,0,
			1,5,4,

			2,1,0,
			2,3,1,

			3,5,1,
			3,7,5,

			6,4,5,
			6,5,7,

			4,2,0,
			4,6,2,

		};

		for (int i = 0; i < (int)sizeof(temp) / sizeof(temp[0]); i++)
		{
			CubeMapIndices[i] = temp[i];
		}

		//Bind and Generate Info

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeMapVertices), CubeMapVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeMapIndices), CubeMapIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

		//Face Image Names

		this->faces.push_back("right.jpg");
		this->faces.push_back("left.jpg");
		this->faces.push_back("top.jpg");
		this->faces.push_back("bottom.jpg");
		this->faces.push_back("back.jpg");
		this->faces.push_back("front.jpg");

		//Create program

		this->program = ShaderLoader::CreateProgram("Resources/Shaders/CubeMapFog.vs", "Resources/Shaders/CubeMapFog.fs");

		int width, height;
		unsigned char* image;

		//Assign the faces onto sides
		for (GLuint i = 0; i < 6; i++)
		{
			std::string tmpPath = _pathToCubeMap;
			tmpPath.append(this->faces.at(i));
			image = SOIL_load_image(tmpPath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			SOIL_free_image_data(image);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		this->texID = this->texture;

		Console_OutputLog(to_wstring("CubeMap: " + _name + " Initalised"), LOGINFO);
	}

	//Update rotation, scale and position of cube map so it renders correctly
	void Update()
	{
		this->model = glm::scale(glm::mat4(), glm::vec3(2000.0f, 2000.0f, 2000.0f));
		this->projCalc = this->camera->getMVP(this->position, this->scale, this->rotationZ) * this->model;
	}

	//Render the Cubemap
	void Render()
	{
		glUseProgram(this->program);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
		glUniform1i(glGetUniformLocation(this->program, "cubeSampler"), 0);
		glUniformMatrix4fv(glGetUniformLocation(this->program, "proj_calc"), 1, GL_FALSE, glm::value_ptr(this->projCalc));

		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	GLuint texID = NULL;

private:

	std::string name = "Untitled CubeMap";
	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	Camera* camera = nullptr;
	vector<std::string> faces;
	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;
};

//Simple 3D Objects
class Simple3DObject {
public:
	std::string name = "Untitled Basic 3D";

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(10.0f, 10.0f, 10.0f);
	glm::vec3 rotationAxisZ = glm::vec3(0.0f, 1.0f, 0.0f);

	bool reflective = false;

	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;

	float rotationAngle = 0;

	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	glm::mat4 translationMatrix;
	glm::mat4 scaleMatrix;

	int m_size = 0;

	void Initalise(glm::vec3 _position, glm::vec3 _scale, std::string textureFilePath, std::string vShaderFilePath, std::string fShaderFilePath, GLuint Indices[], GLfloat Verts[], std::string _name, bool _reflective);
	void Render(Camera* cam, CubeMap* _skyBox, GLuint Indices[]);

};

//Bullets
class Bullet {
public:
	Bullet(Model* mObject, float deltaTime);
	~Bullet();
	void Tick(float deltaTime);
	bool amAllowedAlive();
	Model* object;
	float lifeTime = 5.0f;
	float time = 0.0f;
	float deadLifeTime = 1000.0f;
	bool isOnPlayerTeam = false;
};

//Enemies
class Enemy {
public:
	Enemy(Model* mObject, float deltaTime);
	~Enemy();
	void Tick(float deltaTime);
	bool amAllowedAlive = true;
	Model* object;
	float moveSpeed = 5.0f;
};

class Player {
	public:
	Model* object;
	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
	glm::vec3 targetPos = { 0.0f, 0.0f, 0.0f };
	float maxSpeed = 7.0f;
	float maxForce = 3.0f;

	//DEBUG
	bool Flee = false;
	bool Wander = false;
	bool Pursuit = false;
	bool Evade = false;
	bool Leader = false;

	Player(Model* mObject);
	~Player();

	void Update(GLfloat deltaTime, glm::vec4 maxWorldSize);

};

class Terrain {

public:

	void Initalise(Camera* _cam, std::string _pathToHeightMap, std::string _name) {
		Console_OutputLog(to_wstring("Initalising Terrain: " + _name), LOGINFO);

		this->name = _name;
		this->camera = _cam;
		

		//Create Vertcies and Indices


		int totalSize = 0;

		//Get Info From Map
		ifstream heightMap;
		heightMap.open(_pathToHeightMap.c_str(), std::ios_base::binary);
		if (heightMap.fail())
		{
			Console_OutputLog(L"Could not load height map", LOGWARN);
			return;
		}
		else {
			//Get Image Size

			char c;

			while (heightMap >> c) {
				totalSize++;
			}

			//Resize vectors to image size

			this->rawData.resize(totalSize);
			this->heightInfo.resize(totalSize);

			this->imageSize.x = sqrt(this->rawData.size());
			this->imageSize.y = this->imageSize.x;

			totalSize = this->rawData.size();

			heightMap.close();

			heightMap.open(_pathToHeightMap.c_str(), std::ios_base::binary);

			//Put map info into vector

			heightMap.read((char*)&this->rawData[0], (std::streamsize)this->rawData.size());
		}

		heightMap.close();


		for (UINT i = 0; i < totalSize; ++i)
		{
			//heightInfo[i] = (float)rawData[i] * mInfo.HeightScale + mInfo.HeightOffset;
			this->heightInfo[i] = (float)this->rawData[i];
		}

		//Create Vertices From HeightInfo

		int row = 0;
		int col = 0;

		for (size_t i = 0; i < this->imageSize.y-1; i++) //for each row
		{
			for (size_t j = 0; j < this->imageSize.x-1; j++) //for each col
			{
				this->TerrianVertices.push_back(col);
				int x = j + (row * (this->imageSize.x-1));
				this->TerrianVertices.push_back(this->heightInfo[x]); //position on j plus where we are on the rows
				this->TerrianVertices.push_back(row);
				col++;
			}
			row++;
			col = 0;
		}
		

		//Create Indices

		// Iterate over each quad and compute indices.

		this->TerrianIndices.resize(totalSize*6);

		int k = 0;
		for (unsigned int i = 0; i < this->imageSize.y - 1; ++i) {
			for (unsigned int j = 0; j < this->imageSize.x - 1; ++j) {
				this->TerrianIndices[k] = i * this->imageSize.x + j;
				this->TerrianIndices[k + 1] = i * this->imageSize.x + j + 1;
				this->TerrianIndices[k + 2] = (i + 1) * this->imageSize.x + j;

				this->TerrianIndices[k + 3] = (i + 1) * this->imageSize.x + j;
				this->TerrianIndices[k + 4] = i * this->imageSize.x + j + 1;
				this->TerrianIndices[k + 5] = (i + 1) * this->imageSize.x + j + 1;

				k += 6; // next quad
			}
		}


		//Bind and Generate Info

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->TerrianVertices.size() * sizeof(GLfloat), &this->TerrianVertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->TerrianIndices.size() * sizeof(GLuint), &this->TerrianIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		/*glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);*/

		//Create program

		this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObjectColor.vs", "Resources/Shaders/3DObjectColor.fs");

		int width, height;
		unsigned char* image;

		//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		//this->texID = this->texture;

		Console_OutputLog(to_wstring("Terrian: " + _name + " Initalised"), LOGINFO);


	}

	void Render(Camera* camera) {
		glUseProgram(program);

		// Bind appropriate textures
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		//for (GLuint i = 0; i < this->textures.size(); i++)
		//{
		//	glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		//									  // Retrieve texture number (the N in diffuse_textureN)
		//	stringstream ss;
		//	string number;
		//	string name = this->textures[i].type;
		//	if (name == "texture_diffuse")
		//		ss << diffuseNr++; // Transfer GLuint to stream
		//	else if (name == "texture_specular")
		//		ss << specularNr++; // Transfer GLuint to stream
		//	number = ss.str();
		//	// Now set the sampler to the correct texture unit
		//	glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);
		//	// And finally bind the texture
		//	glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		//}

		glm::mat4 model;
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(this->rotationAngle), this->rotationAxisZ);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
		model = translationMatrix * rotationZ * scaleMatrix;
		glm::mat4 mvp = camera->proj * camera->view * glm::scale(glm::mat4(), scale);
		glm::vec3 camPos = camera->camPos;

		//POSITION AND SCALE
		glm::mat4 projCalc = camera->proj * camera->view * model;

		GLint mvpLoc = glGetUniformLocation(program, "proj_calc");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projCalc));
		GLint lightModel = glGetUniformLocation(program, "model");
		glUniformMatrix4fv(lightModel, 1, GL_FALSE, glm::value_ptr(model));
		//GLint camPosShade = glGetUniformLocation(program, "camPos");
		//glUniform3fv(camPosShade, 1, glm::value_ptr(camPos));
		glUniform3f(glGetUniformLocation(program, "camPos"), camera->camPos.x, camera->camPos.y, camera->camPos.z);
		//glEnable(GL_CULL_FACE);

		// Draw mesh
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->TerrianIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		// Always good practice to set everything back to defaults once configured.
		//for (GLuint i = 0; i < this->textures.size(); i++) {
		//	glActiveTexture(GL_TEXTURE0 + i);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//}

		//glDisable(GL_CULL_FACE);

	}

private:
	std::string name = "Untitled Terrian";
	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 imageSize;
	glm::vec3 rotationAxisZ = glm::vec3(0.0f, 0.0f, 0.0f);

	vector<unsigned char> rawData;
	vector<float> heightInfo;
	vector<GLfloat> TerrianVertices;
	vector<GLuint> TerrianIndices;

	Camera* camera = nullptr;
	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;

	float rotationAngle = 0;

};