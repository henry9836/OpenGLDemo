#pragma once

#include <vector>
#include <fstream>
#include "3D.h"
#include "AI.h"

class GameManager {
public:
	GameManager();

	enum Screens {
		MAIN,
		GAME,
		GAMEOVER
	};
	int currentScreen = MAIN;

	std::vector<Enemy*>* enemyList;
	std::vector<AIObject*>* aiList;

	Camera* mCam;

	int score = 0;
	int wave = 1;
	int waveSpawnAmount = 10;
	int lives = 3;
	int amountSpawned = 0;
	int highscore = 0;
	float enemyMoveSpeed = 0.5f;
	bool waveSpawned = false;
	bool gameover = false;
	bool leave = false;
	
	void CheckGeneralInput(GameManager& m_game);
};

