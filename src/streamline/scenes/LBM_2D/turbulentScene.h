#ifndef TURBULENT_SCENE_H
#define TURBULENT_SCENE_H

#include <defines.h>
#include <QOpenGLFunctions>
#include <clw.h>
#include <glm/glm.hpp>
#include "../../glshader.h"
#include "../scene.h"
#include "../sceneBuilder.h"

#ifdef D2Q9

#define DOMAIN glm::vec2(3,3)
#define RESOLUTION glm::vec2(128, 128)

class TurbulentScene : public Scene {
	public:
		// Needs to know about OpenGL functions, the cl context (for building cl images/kernels), the command queue (for building kernels) and the cl-gl interop texture
		TurbulentScene(QOpenGLFunctions* f, CLContext* context, cl::CommandQueue& queue, cl::ImageGL& interop, size_t screenWidth, size_t screenHeight) : Scene(true), context(context), interop(interop) {
			sceneBuilder = SceneBuilder(DOMAIN, true, context->getContext(), RIGHT_FREE);
			sceneBuilder.addCircle(glm::vec2(1, 1.5), 0.2);
			sceneBuilder.build("turbulentScene", RESOLUTION);
			boundaries = sceneBuilder.getImage2D();

			readTexture = new Shader(f, "../../../src/shaders/opengl/render.vs", "../../../src/shaders/opengl/render.fs");
			context->createProgram("render_lbm", "../../../src/shaders/opencl/lbmRender.cl", "");

			cl::NDRange globalOffset = cl::NullRange;
			cl::NDRange globalRange = cl::NDRange(screenWidth, screenHeight);
			cl::NDRange localRange = cl::NullRange;
			vector<cl::Memory> glObjects;
			glObjects.push_back(interop);
			renderKernel = Kernel(queue, globalOffset, globalRange, localRange, glObjects);
		}
		~TurbulentScene() {
			delete readTexture;
		}

		void render() override {
			context->runProgram("render_lbm", renderKernel, boundaries, 0.0f, interop);

			readTexture->use();
		}

		void step() override {
			
		}

	private:
		CLContext* context;

		SceneBuilder sceneBuilder;
		cl::Image2D boundaries;
		cl::ImageGL interop;

		Kernel renderKernel;

		Shader* readTexture;
};


#endif

#endif