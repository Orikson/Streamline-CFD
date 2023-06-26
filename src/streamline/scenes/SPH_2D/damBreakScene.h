#ifndef DAM_BREAK_SCENE_H
#define DAM_BREAK_SCENE_H

#include <defines.h>
#include <QOpenGLFunctions>
#include <glm/glm.hpp>
#include <cstdlib>
#include <SPH/sph2d.h>
#include <shapes.h>
#include "../sceneBuilder.h"
#include "../scene.h"
#include <glshader.h>

#ifdef SPH_2D

class DamBreakScene : public Scene {
	public:
		DamBreakScene(QOpenGLFunctions* f) : Scene(false) {
			f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			srand(0);

			vector<SPH_Particle2D*> init;
			int n = 500;
			int b = sqrt((float)n);
			for (int i = 0; i < b; i ++) {
				for (int j = 0; j < b; j++) {
					SPH_Particle2D* tmp = new SPH_Particle2D;
					//tmp->position = glm::vec2(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.5f;
					tmp->position = glm::vec2((float)i / b + 0.5f/b + 0.01 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX), (float)j / b + 0.5f/b);
					tmp->position.x *= 0.5f;
					tmp->position.x += 0.25;
					tmp->velocity = glm::vec2(0, 0);
					tmp->mass = 1.0f;
					init.push_back(tmp);
				}
			}

			vector<Shape*> boundaries;
			boundaries.push_back(new BBRectangle(glm::vec2(0.5, -0.1), glm::vec2(1.2, 0.2), BOUNDARY));
			boundaries.push_back(new BBRectangle(glm::vec2(-0.1, 0.5), glm::vec2(0.2, 1.2), BOUNDARY));
			boundaries.push_back(new BBRectangle(glm::vec2(1.1, 0.5), glm::vec2(0.2, 1.2), BOUNDARY));
			boundaries.push_back(new BBRectangle(glm::vec2(0.5, 1.1), glm::vec2(1.2, 0.2), BOUNDARY));

			// definitely need to tweak these values a bit (and mass above)
			dam = new SPH_2D_CPU(f, 0.001, 4, 1, 1.5, init, boundaries);
		}
		~DamBreakScene() {}

		void render() override {
			dam->render();
		}

		void step() override {
			dam->step();
		}

	private:
		SPH_2D_CPU* dam;
};

#endif


#endif