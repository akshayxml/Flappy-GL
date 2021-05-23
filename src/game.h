#ifndef VAO_H
#define VAO_H

#include <iostream>
#include <vector>

#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum GameStates { MENU, PLAYING, GAME_OVER };

class Game {
	float GAME_SPEED;
	unsigned int flyUpCount, birdTexture, bgTexture, pipeTexture, birdVAO, bgVAO, pipeVAO;
	glm::vec3 birdCurPos;
	glm::vec3 bgCurPos;
	std::vector<glm::vec3> pipeCurPos;
	GameStates curGameState;
	Shader bgShader, birdShader, pipeShader;

	public:
		Game(Shader bgShader, Shader birdShader, Shader pipeShader,
			unsigned int birdTexture, unsigned int bgTexture, unsigned int pipeTexture,
			unsigned int birdVAO, unsigned int bgVAO, unsigned int pipeVAO) 
			: GAME_SPEED(0.002), flyUpCount(0), birdCurPos(glm::vec3(0.0f)), bgCurPos(glm::vec3(0.0f)) {
			
			pipeCurPos = { glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(3.0f),
											glm::vec3(3.5f, 0.0f, 0.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(4.5f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f) };
			curGameState = PLAYING;
			this->bgShader = bgShader;
			this->birdShader = birdShader;
			this->pipeShader = pipeShader;
			this->birdTexture = birdTexture;
			this->bgTexture = bgTexture;
			this->pipeTexture = pipeTexture;
			this->birdVAO = birdVAO;
			this->bgVAO = bgVAO;
			this->pipeVAO = pipeVAO;
		}

		void run() {
			if (curGameState == MENU) {
				std::cout << "menuu";
			}
			else if (curGameState == PLAYING) {
				play();
				if (birdCurPos.y <= -0.77f) {
					curGameState = GAME_OVER;
				}
				else {
					float by = birdCurPos.y;
					for (int i = 0; i < pipeCurPos.size(); i++) {
						float px = pipeCurPos[i].x;
						float py = pipeCurPos[i].y;
						if (abs(px - 0.0f) > 0.1f)
							continue;
						if (by + py < 0.6f || by + py > 0.9f) {
							curGameState = GAME_OVER;
						}
					}
				}
			}
			else if (curGameState == GAME_OVER) {

			}
		}
};
#endif