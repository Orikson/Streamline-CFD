#ifndef BUBBLE_SCENE_H
#define BUBBLE_SCENE_H

#include <defines.h>
#include <QOpenGLFunctions>
#include <VPHMCF/vphmcf.h>
#include "../scene.h"
#include "../../glshader.h"

#ifdef VPH_2D

class BubbleScene : public Scene {
	public:
		BubbleScene(QOpenGLFunctions* f) : Scene(false) {
			vector<Vertex2D> verts;
			int n = 100;
			for (int i = 0; i < n; i++) {
				float t = 2 * 3.1415 / n * i;
				glm::vec2 pos = glm::vec2((2.0f + sin(3.0f * t)) * sin(t), (2.0f + sin(2.0f * t)) * cos(t));
				verts.push_back(Vertex2D(pos * 0.1f));
			}

			bubble = new Bubble2D(1, verts, f, NULL);
			asColor = new Shader(f, "../../../src/shaders/opengl/asColor.vs", "../../../src/shaders/opengl/asColor.fs");
		}
		~BubbleScene() {}

		void render() override {
			asColor->use();
			bubble->render();
		}
		
		void step() override {
			bubble->step();
		}

	private:
		Bubble2D* bubble;
		Shader* asColor;
};

#endif


#endif