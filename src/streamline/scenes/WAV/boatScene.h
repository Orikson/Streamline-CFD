#ifndef BOAT_SCENE_H
#define BOAT_SCENE_H

#include <defines.h>
#include <QOpenGLFunctions>
#include "../scene.h"
#include <mesh.h>
#include <camera.h>
#include <glshader.h>

#ifdef WAV

#include <WAV/wav.h>

class BoatScene : public Scene {
	public:
		BoatScene(QOpenGLFunctions* f, CLContext* context, cl::CommandQueue& queue) : Scene(false), f(f) {
			boat = new Model(f, "../../../resources/boat.obj");
			holoLight = new Shader(f, "../../../src/shaders/opengl/vertex.vs", "../../../src/shaders/opengl/holoLight.fs");
			fluidShader = new Shader(f, "../../../src/shaders/opengl/vertex.vs", "../../../src/shaders/opengl/heightField.fs");
			camera = new ArcCamera(glm::vec3(0, 0, 0), 50, 0, 60);
			camera->maximumScroll = 60;
			camera->minimumScroll = 10;
			time = 0;
			waves = new Heightfield(f, context, queue, 1, 500, glm::vec2(40, 40));
		}
		~BoatScene() {
			delete boat;
			delete holoLight;
			delete camera;
		}

		void render() override {
			glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), 1.0f, 0.1f, 100.0f);
			glm::mat4 view = camera->getViewMatrix();

			holoLight->use();
			holoLight->setVec3("objColor", 0.5f, 0.5f, 0.5f);
			holoLight->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			holoLight->setVec3("lightPos", -2.0f, -2.0f, -2.0f);
			holoLight->setVec3("viewPos", camera->position);
			holoLight->setMat4("projection", projection);
			holoLight->setMat4("view", view);

			glm::mat4 model = glm::mat4(1.0f);
			float n = 10.0f;
			glm::vec2 tmp = n*glm::vec2(cos(time * 2.0f), cos(time));
			float theta = atan2(tmp.y, tmp.x);
			model = glm::translate(model, n*glm::vec3(cos(time)*sin(time), 0, sin(time)));
			model = glm::rotate(model, -theta + 3.1415f * 0.5f, glm::vec3(0, 1, 0));
			model = glm::translate(model, glm::vec3(0, 0, -2));
			model = glm::rotate(model, 0.05f * cos(time * 2.0f), glm::vec3(0, 0, 1));
			holoLight->setMat4("model", model);

			f->glEnable(GL_DEPTH_TEST);
			f->glEnable(GL_CULL_FACE);
			f->glCullFace(GL_BACK);
			f->glFrontFace(GL_CCW);

			boat->draw(holoLight);

			int2 start = int2((n * glm::vec2(cos(time) * sin(time), sin(time)) + 20.0f) / 40.0f * 500.0f - 5.0f);
			int2 end = int2(start.x + 10, start.y + 10);
			qDebug() << "start" << start.x << start.y;
			qDebug() << "end" << end.x << end.y;
			waves->offset(start, end, -0.01);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-20, 0, -20));
			
			fluidShader->use();
			fluidShader->setMat4("model", model);
			fluidShader->setMat4("projection", projection);
			fluidShader->setMat4("view", view);
			waves->render();
		}

		void step() override {
			time += 0.01;
			waves->step();
			//camera->updateMouse(5.0f, 0.0f);
		}

		void updateMouse(float mX, float mY, float mS) override {
			camera->updateMouse(mX, mY);
			camera->updateScroll(mS * 0.01);
		}

		void updateKeyboard(vector<char> keys) override {

		}

	private:
		float time;

		Model* boat;
		ArcCamera* camera;
		Shader* holoLight;
		Shader* fluidShader;
		QOpenGLFunctions* f;

		Heightfield* waves;
};

#endif

#endif