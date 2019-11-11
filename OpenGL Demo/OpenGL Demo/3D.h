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
#include <stdlib.h>
#include <time.h>    

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

		this->program = ShaderLoader::CreateProgram("Resources/Shaders/CubeMap.vs", "Resources/Shaders/CubeMap.fs");

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


	bool inBounds(UINT i, UINT j)
	{
		// True if ij are valid indices; false otherwise.
		return
			i >= 0 && i < this->imageSize.y &&
			j >= 0 && j < this->imageSize.x;
	}

	float average(UINT i, UINT j)
	{
		// Function computes the average height of the ij element.
		// It averages itself with its eight neighbor pixels.  Note
		// that if a pixel is missing neighbor, we just don't include it
		// in the average--that is, edge pixels don't have a neighbor pixel.
		//
		// ----------
		// | 1| 2| 3|
		// ----------
		// |4 |ij| 6|
		// ----------
		// | 7| 8| 9|
		// ----------

		float avg = 0.0f;
		float num = 0.0f;

		for (UINT m = i - 1; m <= i + 1; ++m)
		{
			for (UINT n = j - 1; n <= j + 1; ++n)
			{
				if (inBounds(m, n))
				{
					avg += heightInfo[m * (int)this->imageSize.y + n];
					num += 1.0f;
				}
			}
		}

		return avg / num;
	}

	//generate noise at point
	float random(float x, float y) {

		//int n = x + y * 57;
		int n = x + y * randomNum;
		n = (n << 13) ^ n; 
		int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff; 
		return 1.0 - double(t) * 0.931322574615478515625e-9;
	}

	//Interpolate point
	float interpolate(float a, float b, float x) {
		return a * (1 - x) + b * x;
	}

	//Smooth Noise
	float smoothNoise(float x, float y) {
		float corners;
		float sides;
		float center;


		corners = (random(x - 1, y - 1) + random(x + 1, y - 1) + random(x - 1, y + 1) + random(x + 1, y + 1)) / 16;
		sides = (random(x - 1, y) + random(x + 1, y) + random(x, y - 1) + random(x, y + 1)) / 8;
		center = random(x,y) / 4;

		return (corners + sides + center);
	}

	//Generate Perlin Noise
	float noise(float x, float y) {
		float fractional_X = x - int(x); 
		float fractional_Y = y - int(y);

		//smooths 
		float v1 = smoothNoise(int(x), int(y));
		float v2 = smoothNoise(int(x) + 1, int(y));
		float v3 = smoothNoise(int(x), int(y) + 1);
		float v4 = smoothNoise(int(x) + 1, int(y) + 1);

		// interpolates 
		float i1 = interpolate(v1, v2, fractional_X); 
		float i2 = interpolate(v3, v4, fractional_X);

		return interpolate(i1, i2, fractional_Y);
	}

	//Generate Perlin Noise
	float totalNoisePerPoint(float x, float y) {
		int octaves = 8; float zoom = 20.0f; 
		float persistence = 0.5f; 
		float total = 0.0f;

		for (int i = 0; i < octaves - 1; i++) {
			float frequency = pow(2, i) / zoom; 
			float amplitude = pow(persistence, i);
			total += noise(x * frequency, y * frequency) * amplitude;
		} 
		
		return total;
	}

	//Smooth out the terrain
	void smoothTerrain() {
		int pos = 0;
		
		vector<float> smoothed(heightInfo.size());

		for (size_t y = 0; y < this->imageSize.y; y++)
		{
			for (size_t x = 0; x < this->imageSize.x; x++)
			{
				smoothed[y * (int)this->imageSize.x + x] = average(y, x);
			}
		}
		
		heightInfo = smoothed;

	}

	void findNormal() {
		// Estimate normals for interior nodes using central difference.
		float invTwoDX = 1.0f / (2.0f * 1.0f);
		float invTwoDZ = 1.0f / (2.0f * 1.0f);
		for (int i = 2; i < this->imageSize.y - 1; ++i)
		{
			for (int j = 2; j < this->imageSize.x - 1; ++j)
			{
				float t = this->heightInfo[(i - 1) * (int)this->imageSize.x + j];
				float b = this->heightInfo[(i + 1) * (int)this->imageSize.x + j];
				float l = this->heightInfo[i * (int)this->imageSize.x + j - 1];
				float r = this->heightInfo[i * (int)this->imageSize.x + j + 1];

				glm::vec3 tanZ(0.0f, (t - b) * invTwoDZ, 1.0f);
				glm::vec3 tanX(1.0f, (r - l) * invTwoDX, 0.0f);

				glm::vec3 N;
				N = glm::cross(tanZ, tanX);
				glm::normalize(N);

				this->TerrianNormals[(i - 2) * (int)this->imageSize.x + (j - 2)] = N;
			}
		}
	}

	float width()const
	{
		return (this->imageSize.x - 1) * 1;
	}

	float depth()const
	{
		return (this->imageSize.y - 1) * 1;
	}

	float getHeight(float x, float z)const
	{
		try {
			// Transform from terrain local space to "cell" space.
			float c = (x + 0.5f * width()) / 1;
			float d = (z - 0.5f * depth()) / -1;

			// Get the row and column we are in.
			int row = (int)floorf(d);
			int col = (int)floorf(c);

			if (row >= 0 && col >= 0) {

				// Grab the heights of the cell we are in.
				// A*--*B
				//  | /|
				//  |/ |
				// C*--*D
				float A = heightInfo[row * (int)this->imageSize.x + col];
				float B = heightInfo[row * (int)this->imageSize.x + col + 1];
				float C = heightInfo[(row + 1) * (int)this->imageSize.x + col];
				float D = heightInfo[(row + 1) * (int)this->imageSize.x + col + 1];

				// Where we are relative to the cell.
				float s = c - (float)col;
				float t = d - (float)row;

				// If upper triangle ABC.
				if (s + t <= 1.0f)
				{
					float uy = B - A;
					float vy = C - A;
					return A + s * uy + t * vy;
				}
				else // lower triangle DCB.
				{
					float uy = C - D;
					float vy = B - D;
					return D + (1.0f - s) * uy + (1.0f - t) * vy;
				}
			}
			else {
				return 0;
			}
		}
		catch (...) {
			return 0;
		}
	}

	void Initalise(Camera* _cam, std::string _pathToHeightMap, std::string _name, bool randomGen, glm::vec2 _size, float _heightScale) {
		Console_OutputLog(to_wstring("Initalising Terrain: " + _name), LOGINFO);

		this->name = _name;
		this->camera = _cam;
		
		this->randomNum = rand() % 100 + 13;

		float heightScale = _heightScale;
		

		//Create Vertcies and Indices


		int totalSize = 0;

		if (randomGen) {

			Console_OutputLog(to_wstring("Using Seed: " + to_string(this->randomNum)), LOGINFO);

			totalSize = _size.x * _size.y;
			this->rawData.resize(totalSize);
			//this->heightInfo.resize(totalSize * 2);
			//For each y
			for (size_t y = 0; y < _size.y; y++)
			{
				//For each x
				for (size_t x = 0; x < _size.x; x++)
				{
					//Generate Perlin Noise On Point
					this->heightInfo.push_back(totalNoisePerPoint(x, y));
				}
			}
			//Make a fake image size
			this->imageSize.x = (GLfloat)sqrt(this->rawData.size());
			this->imageSize.y = this->imageSize.x;

			//scale heightmap
			for (UINT i = 0; i < heightInfo.size(); ++i)
			{
				if (heightInfo[i] != 0) {
					heightInfo[i] = (heightInfo[i] * heightScale) + 100;
				}
			}
		}
		else {
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

				int w = 0, h = 0;

				unsigned char* image = SOIL_load_image
				(
					_pathToHeightMap.c_str(),
					&w, &h, 0,
					SOIL_LOAD_L
				);

				if (image == nullptr) {
					Console_OutputLog(L"Could not load height map with SOIL defaulting to 512x512 image size", LOGWARN);
					w = 513;
					h = 513;
				}

				totalSize = w * h;

				//Resize vectors to image size

				this->rawData.resize(totalSize);
				this->heightInfo.resize(totalSize * 2);

				this->imageSize.x = (GLfloat)sqrt(this->rawData.size());
				this->imageSize.y = this->imageSize.x;

				heightMap.close();

				heightMap.open(_pathToHeightMap.c_str(), std::ios_base::binary);

				//Put map info into vector

				heightMap.read((char*)& this->rawData[0], (std::streamsize)this->rawData.size());
			}

			heightMap.close();


			for (UINT i = 0; i < rawData.size(); ++i)
			{
				if (heightInfo[i] != 0) {
					heightInfo[i] = ((float)rawData[i] * heightScale) + 100;
				}
			}

			//smooth terrian
			smoothTerrain();
		}
		//Create Vertices From HeightInfo

		int row = 0;
		int col = 0;


		float halfWidth = ((int)this->imageSize.x - 1) * 1.0f * 0.5f;
		float halfDepth = ((int)this->imageSize.y - 1) * 1.0f * 0.5f;

		float du = 1.0f / ((int)this->imageSize.x - 1);
		float dv = 1.0f / ((int)this->imageSize.y - 1);

		TerrianNormals.resize(totalSize + 1);
		int inter = 0;
		findNormal();

		//Create collision vectors
		collisionInfo.resize((int)this->imageSize.y);
		for (size_t i = 0; i < collisionInfo.size(); i++)
		{
			collisionInfo.at(i).resize((int)this->imageSize.x);
		}

		for (UINT i = 0; i < this->imageSize.y -1; ++i)
		{
			float z = halfDepth - i * 1.0f;
			for (UINT j = 0; j < this->imageSize.x; ++j)
			{

				float x = -halfWidth + j * 1.0f;
				float y = heightInfo[i * (int)this->imageSize.x + j];


				//Positions
				this->TerrianVertices.push_back(x);
				this->TerrianVertices.push_back(y);
				this->TerrianVertices.push_back(z);
				
				//Normal
				this->TerrianVertices.push_back(TerrianNormals[inter].x);
				this->TerrianVertices.push_back(TerrianNormals[inter].y);
				this->TerrianVertices.push_back(TerrianNormals[inter].z);

				inter++;

				//Stretch texture over grid.
				this->TerrianVertices.push_back(j * du);
				this->TerrianVertices.push_back(i * dv);

			}
		}

		//Create Indices

		// Iterate over each quad and compute indices.

		this->TerrianIndices.resize(totalSize*6);

		int k = 0;
		for (unsigned int i = 0; i < this->imageSize.y - 1; ++i) {
			for (unsigned int j = 0; j < this->imageSize.x - 1; ++j) {
				this->TerrianIndices[k] = i * (int)this->imageSize.x + j;
				this->TerrianIndices[k + 1] = i * (int)this->imageSize.x + j + 1;
				this->TerrianIndices[k + 2] = (i + 1) * (int)this->imageSize.x + j;

				this->TerrianIndices[k + 3] = (i + 1) * (int)this->imageSize.x + j;
				this->TerrianIndices[k + 4] = i * (int)this->imageSize.x + j + 1;
				this->TerrianIndices[k + 5] = (i + 1) * (int)this->imageSize.x + j + 1;

				k += 6; // next quad
			}
		}


		//Bind and Generate Info

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		int width, height;

		unsigned char* image = SOIL_load_image("Resources/Textures/map.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->TerrianVertices.size() * sizeof(GLfloat), &this->TerrianVertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->TerrianIndices.size() * sizeof(GLuint), &this->TerrianIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2); // REMEMBER UR INDICES

		//Create program

		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObjectColor.vs", "Resources/Shaders/3DObjectColor.fs");
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs"); //Renders 2D
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic3D.vs", "Resources/Shaders/Basic3D.fs"); //Render
		this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_BlinnPhong.fs");
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_DiffuseColor.fs");

		Console_OutputLog(to_wstring("Terrian: " + _name + " Initalised"), LOGINFO);


	}

	void Render(Camera* camera) {

		glUseProgram(this->program);
		glBindVertexArray(this->VAO);

		glm::mat4 model;
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(this->rotationAngle), this->rotationAxisZ);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
		model = translationMatrix * rotationZ * scaleMatrix;
		glm::mat4 mvp = camera->proj * camera->view * model;
		glm::vec3 camPos = camera->camPos;

		//POSITION AND SCALE
		glm::mat4 projCalc = camera->proj * camera->view * model;

		//PATCH

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		glUniform1i(glGetUniformLocation(this->program, "texture_diffuse1"), 0);


		//PATCH END
		GLint mvpLoc = glGetUniformLocation(program, "proj_calc");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projCalc));
		//GLint mvpLoc2 = glGetUniformLocation(program, "vp");
		//glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(projCalc));
		GLint modelPass = glGetUniformLocation(program, "model");
		glUniformMatrix4fv(modelPass, 1, GL_FALSE, glm::value_ptr(model));
		GLint camPosPass = glGetUniformLocation(program, "camPos");
		glUniformMatrix3fv(camPosPass, 1, GL_FALSE, glm::value_ptr(camPos));


		

		glDrawElements(GL_TRIANGLES, this->TerrianIndices.size(), GL_UNSIGNED_INT, 0);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		//Clearing the vertex array
		glBindVertexArray(0);
		glUseProgram(0);

	}

	vector<vector<float>> collisionInfo;
	glm::vec3 position = glm::vec3(0.0f, -150.0f, 0.0f);

private:
	int randomNum = 57;
	std::string name = "Untitled Terrian";
	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 imageSize;
	glm::vec3 rotationAxisZ = glm::vec3(1.0f, 0.0f, 0.0f);

	vector<unsigned char> rawData;
	vector<float> heightInfo;
	vector<GLfloat> TerrianVertices;
	vector<GLuint> TerrianIndices;
	vector<glm::vec3> TerrianNormals;
	

	Camera* camera = nullptr;
	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;

	float rotationAngle = 0;

};

class GeoShape {
public:
	
	void Render(Camera* cam) {
		glUseProgram(program);

		glm::mat4 model;
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(this->rotationAngle), this->rotationAxisZ);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
		model = translationMatrix * rotationZ * scaleMatrix;

		glm::mat4 mvp = cam->proj * cam->view * model;
		GLint vpLoc = glGetUniformLocation(program, "mvp");
		glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	};

	GLuint VAO = 0, VBO = 0, EBO = 0;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 rotationAxisZ = glm::vec3(1.0f, 0.0f, 0.0f);
	float rotationAngle = 90;
	GLuint program = 0;
};