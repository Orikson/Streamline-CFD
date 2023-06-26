#ifndef WAV_H
#define WAV_H

#include <defines.h>
#include "../shapes.h"
#include "../fluid.h"
#include <vector>
using std::vector;

#ifdef WAV

#include <clw.h>
#include <glm/glm.hpp>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class Heightfield : public Fluid {
	public:
		// Initializes an nxn heightfield
		Heightfield(QOpenGLFunctions* f, CLContext* context, cl::CommandQueue& queue, float dt, unsigned int n, glm::vec2 dim);
		~Heightfield();

		void offset(int2 start, int2 end, float displacement);

		void step() override;
		void render() override;

	private:
		unsigned int n;
		GLuint VBO, EBO;
		cl::BufferGL clVBO;
		cl::Buffer clVel;
		glm::vec2 dim;

		QOpenGLVertexArrayObject* VAO;
		QOpenGLFunctions* f;
		CLContext* context;
		cl::CommandQueue& queue;
		Kernel wave2d;
};

#endif

#endif