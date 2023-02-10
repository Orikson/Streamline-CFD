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
using std::min; using std::max;

struct GLMesh {
	QOpenGLVertexArrayObject* VAO;
	GLuint VBO, EBO;

	vector<float> vertices;
	vector<unsigned int> indices;
};

class Shape {
	public:
		Shape() {}
		~Shape() {}
		// Write a specified tag value into provided data of given resolution and world dimension
		virtual void blit(char* data, unsigned int tag, glm::vec3 resolution, glm::vec3 dimensions) {}
		virtual void blit(char* data, unsigned int tag, glm::vec2 resolution, glm::vec2 dimensions) {}
};

class Circle : public Shape {
	public:
		Circle(glm::vec2 center, float radius) : center2D(center), radius(radius) {
			
		}
		Circle(glm::vec3 center, float radius) : center3D(center), radius(radius) {

		}
		~Circle() {
			
		}

		void blit(char* data, unsigned int tag, glm::vec3 resolution, glm::vec3 dimensions) override {
			glm::vec3 cCenter = center3D / dimensions * resolution;
			float r = radius / dimensions.x * resolution.x;
			for (unsigned int i = max(cCenter.x - r, 0.0f); i < min(cCenter.x + r, resolution.x); i++) {
				for (unsigned int j = max(cCenter.y - r, 0.0f); j < min(cCenter.y + r, resolution.y); j++) {
					for (unsigned int k = max(cCenter.z - r, 0.0f); k < min(cCenter.z + r, resolution.z); k++) {
						glm::vec3 point = glm::vec3(i, j, k);
						if (glm::distance(point, cCenter) <= r) {
							data[(int)(i + j * resolution.x + k * resolution.x * resolution.y)] |= tag;
						}
					}
				}
			}
		}

		void blit(char* data, unsigned int tag, glm::vec2 resolution, glm::vec2 dimensions) override {
			glm::vec2 cCenter = center2D / dimensions * resolution;
			float r = radius / dimensions.x * resolution.x;
			for (unsigned int i = max(cCenter.x - r, 0.0f); i < min(cCenter.x + r, resolution.x); i++) {
				for (unsigned int j = max(cCenter.y - r, 0.0f); j < min(cCenter.y + r, resolution.y); j++) {
					glm::vec2 point = glm::vec2(i, j);
					if (glm::distance(point, cCenter) <= r) {
						data[(int)(i + j * resolution.x)] |= tag;
					}
				}
			}
		}

	private:
		glm::vec2 center2D;
		glm::vec3 center3D;
		float radius;
};

enum SceneFlags {
	TOP_FREE = 1,
	BOTTOM_FREE = 2,
	LEFT_FREE = 4,
	RIGHT_FREE = 8,
	FRONT_FREE = 16,
	BACK_FREE = 32
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

		void addCircle(glm::vec3 center, float radius) {
			shapes.push_back(new Circle(center, radius));
		}
		void addCircle(glm::vec2 center, float radius) {
			shapes.push_back(new Circle(center, radius));
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
				if (!(flags & TOP_FREE) && i < w) {
					data[i] = 1;
				} else if (!(flags & BOTTOM_FREE) && i > w * h - w) {
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
				shapes[i]->blit(data, 1, resolution, fluidDomain2D);
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
				shapes[i]->blit(data, 1, resolution, fluidDomain3D);
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