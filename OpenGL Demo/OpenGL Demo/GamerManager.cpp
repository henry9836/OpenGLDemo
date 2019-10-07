#include "GamerManager.h"
#include "Input.h"


Input m_Input;

GameManager::GameManager() {
	std::string fileString = "0";
	ifstream highscoreFileIn;
	highscoreFileIn.open("highscore.txt");
	highscoreFileIn >> fileString;
	highscoreFileIn.close();
}

void GameManager::CheckGeneralInput(GameManager& m_game, Camera& camera, glm::vec3 orbittar, Model* player)
{
	switch (m_game.currentScreen)
	{
	case m_game.MAIN: {
		if (m_Input.CheckKeyDown('r')) { //reset
			Console_OutputLog(L"Reseting Demo...", LOGINFO);

			gameover = false;
			stencil = false;
			scissor = false;
			depth = false;
			leave = false;
			caminManual = false;
			wireFrame = false;

			mCam->SwitchMode(Camera::ORBIT, orbittar, glm::vec3(-5.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 5.0f);
		}
		else if (m_Input.CheckKeyDown(27)) { //quit
			Console_OutputLog(L"Exiting Application...", LOGINFO);

			leave = true;
		}
		else if (m_Input.CheckKeyDown('5')) { //stencil
			Console_OutputLog(L"Stencil Test [ON]", LOGINFO);

			stencil = true;
		}
		else if (m_Input.CheckKeyDown('6')) { //stencil
			Console_OutputLog(L"Stencil Test [OFF]", LOGINFO);

			stencil = false;
		}
		else if (m_Input.CheckKeyDown('t')) { //scissor
			Console_OutputLog(L"Scissor Test [ON]", LOGINFO);

			scissor = true;
		}
		else if (m_Input.CheckKeyDown('y')) { //scissor
			Console_OutputLog(L"Scissor Test [OFF]", LOGINFO);

			scissor = false;
		}
		else if (m_Input.CheckKeyDown('g')) { //depth
			Console_OutputLog(L"Depth Test [ON]", LOGINFO);

			depth = true;
		}
		else if (m_Input.CheckKeyDown('h')) { //depth
			Console_OutputLog(L"Depth Test [OFF]", LOGINFO);

			depth = false;
		}
		else if (m_Input.CheckKeyDown('b')) { //wireframe
			Console_OutputLog(L"Wireframe Test [ON]", LOGINFO);

			wireFrame = true;
		}
		else if (m_Input.CheckKeyDown('n')) { //wireframe
			Console_OutputLog(L"Wireframe Test [OFF]", LOGINFO);

			wireFrame = false;
		}
		else if (m_Input.CheckKeyDown('7')) { //wireframe
			Console_OutputLog(L"Frame Buffer [ON]", LOGINFO);

			POST = true;
		}
		else if (m_Input.CheckKeyDown('8')) { //wireframe
			Console_OutputLog(L"Frame Buffer [OFF]", LOGINFO);

			POST = false;
		}
		else if (m_Input.CheckKeyDown('o')) { //orbit mode
			mCam->SwitchMode(Camera::ORBIT, orbittar, glm::vec3(-5.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 5.0f);
			caminManual = false;
		}
		else if (m_Input.CheckKeyDown('m')) { //manual mode
			mCam->SwitchMode(Camera::CONTROL, (mCam->camPos += glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(-5.0f, 3.0f, 0.0f), mCam->lookDirFromFollow, 1, 1);
			caminManual = true;
		}
		if (m_Input.CheckKeyDown('w')) { //go forward
			//mCam->camPos += glm::vec3(1.0f, 0.0f, 0.0f);
			player->position += glm::vec3(1.0f, 0.0f, 0.0f);
		}
		if (m_Input.CheckKeyDown('s')) { //go back
			//mCam->camPos -= glm::vec3(1.0f, 0.0f, 0.0f);
			player->position -= glm::vec3(1.0f, 0.0f, 0.0f);
		}
		if (m_Input.CheckKeyDown('a')) { //go left
			//mCam->camPos -= glm::vec3(0.0f, 0.0f, 1.0f);
			player->position -= glm::vec3(0.0f, 0.0f, 1.0f);
		}
		if (m_Input.CheckKeyDown('d')) { //go right
			//mCam->camPos += glm::vec3(0.0f, 0.0f, 1.0f);
			player->position += glm::vec3(0.0f, 0.0f, 1.0f);
		}
		break;
	}
	default:
		break;
	}
}
