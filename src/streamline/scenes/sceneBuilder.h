#ifndef SCENE_BUILDER_H
#define SCENE_BUILDER_H

#include <defines.h>
#include <iostream>
#include <type_traits>
#include <clw.h>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <glm/glm.hpp>
#include <algorithm> 
#include <shapes.h>
using std::min; using std::max;

struct GLMesh {
	QOpenGLVertexArrayObject* VAO;
	GLuint VBO, EBO;

	vector<float> vertices;
	vector<unsigned int> indices;
};

enum SceneFlags {
	TOP_FREE = 1,
	BOTTOM_FREE = 2,
	LEFT_FREE = 4,
	RIGHT_FREE = 8,
	FRONT_FREE = 16,
	BACK_FREE = 32
};

enum SceneTags {
	BOUNDARY = 1,
	EMIT_P_X = 2,
	EMIT_P_Y = 4,
	EMIT_P_Z = 8,
	EMIT_N_X = 16,
	EMIT_N_Y = 32,
	EMIT_N_Z = 64
};

// Build scenes with standard parameterization and useful functions
class SceneBuilder {
	public:
		// Fluid domain in meters
		SceneBuilder(glm::vec2 fluidDomain, bool asGrid, cl::Context context, unsigned int flags) : fluidDomain2D(fluidDomain), asGrid(asGrid), context(context), flags(flags) {
			is2D = true;
		}
		SceneBuilder(glm::vec3 fluidDomain, bool asGrid, cl::Context context, unsigned int flags) : fluidDomain3D(fluidDomain), asGrid(asGrid), context(context), flags(flags) {
			is2D = false;
		}
		SceneBuilder() {

		}
		~SceneBuilder() {}

		void addCircle(glm::vec3 center, float radius, unsigned int tag) {
			shapes.push_back(new Circle(center, radius, tag));
		}
		void addCircle(glm::vec2 center, float radius, unsigned int tag) {
			shapes.push_back(new Circle(center, radius, tag));
		}

		void build(string name, glm::vec2 resolution) {
			if (asGrid) {
				toGrid(name, resolution);
			} else {

			}
		}
		void build(string name, glm::vec3 resolution) {
			if (asGrid) {
				toGrid(name, resolution);
			} else {

			}
		}
		
		cl::Image2D getImage2D() {
			return image2D;
		}
		cl::Image3D getImage3D() {
			return image3D;
		}

	private:
		glm::vec2 fluidDomain2D;
		glm::vec3 fluidDomain3D;
		bool asGrid, is2D;
		vector<Shape*> shapes;
		cl::Image3D image3D;
		cl::Image2D image2D;
		cl::Context context;
		char* data;
		unsigned int flags;

		// Construct a CL image with tagged bits for each boundary
		void toGrid(string name, glm::vec2 resolution) {
			size_t w = (size_t)resolution.x; size_t h = (size_t)resolution.y;
			data = new char[w * h];
			for (unsigned int i = 0; i < w * h; i++) {
				if (!(flags & BOTTOM_FREE) && i < w) {
					data[i] = 1;
				} else if (!(flags & TOP_FREE) && i >= w * h - w) {
					data[i] = 1;
				} else if (!(flags & LEFT_FREE) && i % w == 0) {
					data[i] = 1;
				} else if (!(flags & RIGHT_FREE) && i % w == w - 1) {
					data[i] = 1;
				} else {
					data[i] = 0;
				}
			}

			for (unsigned int i = 0; i < shapes.size(); i++) {
				shapes[i]->blit(data, resolution, fluidDomain2D);
			}

			cl_int err;
			image2D = cl::Image2D(
				context,
				CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
				cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
				w, h,
				w * sizeof(char),
				(void*)data,
				&err
			);
			qDebug() << "Error: " << err;
		}
		void toGrid(string name, glm::vec3 resolution) {
			size_t w = (size_t)resolution.x; size_t h = (size_t)resolution.y; size_t d = (size_t)resolution.z;

			data = new char[w * h * d];
			for (unsigned int i = 0; i < w * h * d; i++) {
				data[i] = 0;
			}
			for (unsigned int i = 0; i < shapes.size(); i++) {
				shapes[i]->blit(data, resolution, fluidDomain3D);
			}

			cl_int err;
			image3D = cl::Image3D(
				context,
				CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
				cl::ImageFormat(CL_R, CL_UNSIGNED_INT8),
				w, h, d,
				w * sizeof(unsigned int),
				w * h * sizeof(unsigned int),
				(void*)data,
				&err
			);
			qDebug() << "Error: " << err;
		}

		// Construct a 
		void toMesh(string name) {
		
		}
};

#endif