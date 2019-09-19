#include "GamerManager.h"
#include "Input.h"


Input m_Input;

GameManager::GameManager() {
	std::string fileString = "0";
	ifstream highscoreFileIn;
	highscoreFileIn.open("highscore.txt");
	highscoreFileIn >> fileString;
	highscoreFileIn.close();

	this->highscore = std::atoi(fileString.c_str());
}

void GameManager::CheckGeneralInput(GameManager& m_game)
{
	switch (m_game.currentScreen)
	{
	case m_game.MAIN: {
		if (m_Input.CheckKeyDown('1')) {
			m_game.currentScreen = m_game.GAME;
		}
		if (m_Input.CheckKeyDown('2')){
			m_game.leave = true;
		}
		break;
	}
	case m_game.GAME: {
		if ((m_game.enemyList->empty() && m_game.aiList->empty()) || m_game.amountSpawned < m_game.waveSpawnAmount) {
			int spawner = (rand() % 10000) + 0;
			if (spawner > 9700 && amountSpawned <= waveSpawnAmount) { 
				int spawnPos = (rand() % 3) + 1;
				m_game.aiList->push_back(new AIObject);
				switch (spawnPos)
				{
				case 1: {
					//m_game.enemyList->push_back(new Enemy(new Model("Resources/Models/EnemyShell/Dog 1.obj", mCam, "Enemy", float((rand() % 360) + 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(15.0f, 1.0f, (rand() % 30) - 15), glm::vec3(1.5f, 1.5f, 1.5f), "Resources/3DObject_Diffuse.vs", "Resources/3DObject_BlinnPhong.fs"), 0.0f));
					m_game.aiList->back()->Init(new Model("Resources/Models/EnemyShell/Dog 1.obj", mCam, "AIEnemy", float((rand() % 360) + 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(15.0, 1.0f, (rand() % 30) - 15), glm::vec3(1.5f, 1.5f, 1.5f), "Resources/3DObject_Diffuse.vs", "Resources/3DObject_BlinnPhong.fs"));
					break;
				}
				case 2: {
					//m_game.enemyList->push_back(new Enemy(new Model("Resources/Models/EnemyShell/Dog 1.obj", mCam, "Enemy", float((rand() % 360) + 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3((rand() % 15) + 2, 1.0f, -15.0f), glm::vec3(1.5f, 1.5f, 1.5f), "Resources/3DObject_Diffuse.vs", "Resources/3DObject_BlinnPhong.fs"), 0.0f));
					m_game.aiList->back()->Init(new Model("Resources/Models/EnemyShell/Dog 1.obj", mCam, "AIEnemy", float((rand() % 360) + 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3((rand() % 15) + 2, 1.0f, -15.0f), glm::vec3(1.5f, 1.5f, 1.5f), "Resources/3DObject_Diffuse.vs", "Resources/3DObject_BlinnPhong.fs"));
					break;
				}
				case 3: {
					//m_game.enemyList->push_back(new Enemy(new Model("Resources/Models/EnemyShell/Dog 1.obj", mCam, "Enemy", float((rand() % 360) + 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3((rand() % 15) + 2, 1.0f, 15.0f), glm::vec3(1.5f, 1.5f, 1.5f), "Resources/3DObject_Diffuse.vs", "Resources/3DObject_BlinnPhong.fs"), 0.0f));
					m_game.aiList->back()->Init(new Model("Resources/Models/EnemyShell/Dog 1.obj", mCam, "AIEnemy", float((rand() % 360) + 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3((rand() % 15) + 2, 1.0f, 15.0f), glm::vec3(1.5f, 1.5f, 1.5f), "Resources/3DObject_Diffuse.vs", "Resources/3DObject_BlinnPhong.fs"));
					break;
				}
				default:
					break;
				}
				
				m_game.amountSpawned++;
			}
			else if (amountSpawned >= waveSpawnAmount) { //when enemyList is gone
				m_game.waveSpawnAmount += 2;
				m_game.amountSpawned = 0;
				m_game.enemyMoveSpeed += 0.5f;
				m_game.wave++;
			}
		}
		
		if (lives <= 0) {
			m_game.gameover = true;
			m_game.currentScreen = m_game.GAMEOVER;
		}

		break;
	}
	case m_game.GAMEOVER: {

		if (m_game.score > m_game.highscore) {
			Console_OutputLog(L"New Highscore!", LOGINFO);
			m_game.highscore = m_game.score;
			ofstream highscoreFileOut;
			highscoreFileOut.open("highscore.txt");
			highscoreFileOut << std::to_string(m_game.score);
			highscoreFileOut.close();
		}

		if (m_Input.CheckKeyDown('c')) {
			m_game.score = 0;
			m_game.wave = 1;
			m_game.waveSpawnAmount = 3;
			m_game.lives = 3;
			m_game.amountSpawned = 0;
			m_game.enemyMoveSpeed = 0.5f;
			m_game.waveSpawned = false;
			m_game.gameover = false;
			m_game.leave = false;
			m_game.currentScreen = m_game.MAIN;
		}
		break;
	}
	default:
		break;
	}
}
