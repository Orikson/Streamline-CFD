#include "wav.h"

#ifdef WAV

Heightfield::Heightfield(QOpenGLFunctions* f, CLContext* context, cl::CommandQueue& queue, float dt, unsigned int n, glm::vec2 dim) : Fluid("heightfield", dt), f(f), context(context), queue(queue), n(n), dim(dim) {
	// initialize VAO
	vector<float> vertices;
	vector<float> velocities;
	vector<unsigned int> indices;

	for (int i = 0; i < n; i ++) {
		for (int j = 0; j < n; j ++) {
			vertices.push_back((float)j / (float)n * dim.x); // x
			vertices.push_back(0.0f); // y
			vertices.push_back((float)i / (float)n * dim.y); // z

			if (i < n - 1) {
				for (unsigned int k = 0; k < 2; k++) {
					indices.push_back(j + n * (i + k));
				}
			}

			velocities.push_back(0.0f);
		}
	}
	
	VAO = new QOpenGLVertexArrayObject(NULL);
	VAO->create();
	VAO->bind();

	f->glGenBuffers(1, &VBO);
	f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
	f->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	f->glGenBuffers(1, &EBO);
	f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	f->glEnableVertexAttribArray(0);

	// initialize CL objects
	cl_int err;
	cl::Context clContext = context->getContext();
	clVBO = cl::BufferGL(clContext, CL_MEM_READ_WRITE, VBO, &err);
	clVel = cl::Buffer(clContext, CL_MEM_READ_WRITE, sizeof(float) * velocities.size(), NULL, &err);
	err = queue.enqueueWriteBuffer(clVel, CL_FALSE, 0, sizeof(float) * velocities.size(), &velocities[0]);
	queue.finish();

	context->createProgram("wave2d", "../../../src/shaders/opencl/wave2d.cl", "");
	cl::NDRange globalOffset = cl::NullRange;
	cl::NDRange globalRange = cl::NDRange(n, n);
	cl::NDRange localRange = cl::NullRange;
	vector<cl::Memory> glObjects;
	glObjects.push_back(clVBO);
	wave2d = Kernel(queue, globalOffset, globalRange, localRange, glObjects);

	// unbind VAO
	VAO->release();
}

Heightfield::~Heightfield() {
	delete VAO;
}


void Heightfield::offset(int2 start, int2 end, float displacement) {
	VAO->bind();
	vector<float> data;
	for (int i = start.y; i < end.y; i ++) {
		data.clear();

		for (int j = start.x; j < end.x; j ++) {
			data.push_back((float)j / (float)n * dim.x);
			data.push_back(displacement);
			data.push_back((float)i / (float)n * dim.y);
		}

		f->glBufferSubData(GL_ARRAY_BUFFER, ((size_t)start.x + ((size_t)i) * n) * 3 * sizeof(float), sizeof(data), &data[0]);
	}
	VAO->release();
	f->glFinish();
}

void Heightfield::step() {
	context->runProgram("wave2d", "wave2d", wave2d, clVBO, clVel, n, deltaT);
}

void Heightfield::render() {
	VAO->bind();
	for (unsigned int i = 0; i < n; i ++) {
		f->glDrawElements(GL_TRIANGLE_STRIP, n * 2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * n * 2 * i));
	}
	VAO->release();
}

#endif