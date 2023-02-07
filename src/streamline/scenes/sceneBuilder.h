#ifndef SCENE_BUILDER_H
#define SCENE_BUILDER_H

#include <defines.h>
#include <clw.h>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <glm/glm.hpp>

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
		// Write a specified tag bit into provided data of given resolution
		virtual void blit(unsigned int* data, unsigned int tag, glm::vec3 resolution) {

		}
};

// Build scenes with standard parameterization and useful functions
class SceneBuilder {
	public:
		// 3D
		SceneBuilder(glm::vec3 fluidDomain, bool asGrid) {

		}

		// 2D
		SceneBuilder(glm::vec2 fluidDomain, bool asGrid) {

		}

		~SceneBuilder() {}
		
		vector<

	private:
		// Construct a CL image with tagged bits for each boundary
		void toGrid(string name) {

		}

		// Construct a 
		void toMesh(string name) {
		
		}
};

#endif