#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>

#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum GameStates { MENU, PLAYING, GAME_OVER };

class Game {
	float GAME_SPEED;
	unsigned int birdTexture, bird_koTexture, bgTexture, bg_koTexture, pipeTexture, birdVAO, bgVAO, pipeVAO, score;
	glm::vec3 birdCurPos;
	glm::vec3 bgCurPos;
	std::vector<glm::vec3> pipeCurPos;

	void play(){
		if (flyUpCount == 0) {
			birdCurPos.y = glm::max((float)(birdCurPos.y - 0.005), -0.77f);
		}
		else {
			birdCurPos.y = glm::min((float)(birdCurPos.y + 0.01), 0.9f);
			flyUpCount--;
			if (birdCurPos.y == 0.9f)
				flyUpCount = 0;
		}

		if (bgCurPos.x <= -4.0f) {
			bgCurPos.x = 0.0f;
		}

		generateBG();

		generateBird();
		
		generatePipes();
	}

	void generateBG() {
		bgShader.use();

		bgCurPos.x -= GAME_SPEED;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, bgCurPos);
		bgShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bgTexture);
		glBindVertexArray(bgVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glm::vec3 bgScrollPos = bgCurPos;
		bgScrollPos.x = bgCurPos.x + 4.0f;
		model = glm::translate(glm::mat4(1.0f), bgScrollPos);
		bgShader.setMat4("model", model);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void generateBird() {
		birdShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, birdCurPos);
		birdShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, birdTexture);
		glBindVertexArray(birdVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void generatePipes() {
		for (auto& curPos : pipeCurPos) {
			if (curPos.x <= -2.5f)
				curPos.x = 1.5f;

			pipeShader.use();

			if (curPos.x >= 1.5f and curPos.x <= 1.55f) {
				curPos.y = (rand() % 50 + 50) / 100.0f;
			}
			curPos.x -= GAME_SPEED;

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(curPos.x, -curPos.y, 0.0f));
			pipeShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, pipeTexture);
			glBindVertexArray(pipeVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(1.0f, -1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(curPos.x, -1.5f + curPos.y, 0.0f));
			pipeShader.setMat4("model", model);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	bool checkCollision() {
		float by = birdCurPos.y;
		for (int i = 0; i < pipeCurPos.size(); i++) {
			float px = pipeCurPos[i].x;
			float py = pipeCurPos[i].y;
			if (abs(px - 0.0f) > 0.1f)
				continue;
			if (by + py < 0.6f || by + py > 0.9f) {
				return true;
			}
			score += 1;
		}
		return false;
	}

	void gameOver() {
		bgShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		bgShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bg_koTexture);
		glBindVertexArray(bgVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Bird
		birdShader.use();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.77f, 1.0f));;
		birdShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_koTexture);
		glBindVertexArray(birdVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	public:
		Shader bgShader, birdShader, pipeShader;
		unsigned int flyUpCount;
		GameStates curGameState;

		Game(unsigned int birdShaderID, unsigned int bgShaderID, unsigned int pipeShaderID,
			unsigned int birdTexture, unsigned int bird_koTexture, unsigned int bgTexture, unsigned int bg_koTexture, unsigned int pipeTexture,
			unsigned int birdVAO, unsigned int bgVAO, unsigned int pipeVAO) 
			: GAME_SPEED(0.002){
			
			curGameState = PLAYING;
			this->bgShader.SetID(bgShaderID);
			this->birdShader.SetID(birdShaderID);
			this->pipeShader.SetID(pipeShaderID);
			this->birdTexture = birdTexture;
			this->bird_koTexture = bird_koTexture;
			this->bgTexture = bgTexture;
			this->bg_koTexture = bg_koTexture;
			this->pipeTexture = pipeTexture;
			this->birdVAO = birdVAO;
			this->bgVAO = bgVAO;
			this->pipeVAO = pipeVAO;
		}

		void init() {
			birdCurPos = glm::vec3(0.0f);
			bgCurPos = glm::vec3(0.0f);
			pipeCurPos = { glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(3.0f),
											glm::vec3(3.5f, 0.0f, 0.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(4.5f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f) };
			flyUpCount = 0;
			score = 0;
		}

		void run() {
			if (curGameState == MENU) {
				std::cout << "menu";
			}
			else if (curGameState == PLAYING) {
				play();
				if (birdCurPos.y <= -0.77f || checkCollision()) {
					curGameState = GAME_OVER;
				}
			}
			else if (curGameState == GAME_OVER) {
				gameOver();
			}
		}

		int getScore() {
			return this->score / 100;
		}
};

#endif