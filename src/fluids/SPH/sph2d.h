#ifndef SPH_2D_H
#define SPH_2D_H

#include <defines.h>
#include "../shapes.h"
#include "../fluid.h"

#ifdef SPH_2D

#include <unordered_map>
using std::unordered_map;
#include <vector>
using std::vector;
#include <glm/glm.hpp>
#include <QOpenGLFunctions>

#include <math.h>

struct GLPoint {
	glm::vec3 pos;
	glm::vec3 col;
};

// add hashes for int2 and int3
namespace std {
	template <>
	struct hash<int2> {
		std::size_t operator()(const int2& k) const {
			using std::size_t;
			using std::hash;
			using std::string;
			return ((hash<int>()(k.x)
				^ (hash<int>()(k.y) << 1)) >> 1);
		}
	};

	template <>
	struct hash<int3> {
		std::size_t operator()(const int3& k) const {
			using std::size_t;
			using std::hash;
			using std::string;
			return ((hash<int>()(k.x)
				^ (hash<int>()(k.y) << 1)) >> 1)
				^ (hash<int>()(k.z) << 1);
		}
	};
}

struct SPH_Particle2D {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 pressureGrad;
	glm::vec2 lapV;
	float density;
	float mass;
	float pressure;
};

class SPH_2D_CPU : public Fluid {
	public:
		// by default position is (0, 0) -> dim
		SPH_2D_CPU(QOpenGLFunctions* f, float dt, float rho, float mu, float k, vector<SPH_Particle2D*> init, vector<Shape*> boundaries);
		~SPH_2D_CPU();

		void step() override;

		// three ideas for rendering:
		// 1. use OpenCL to draw particles to OpenGL texture, then use screen plane
		// 2. instance "particle" quads and use pure OpenGL (if I want fancy particles)
		// 3. VBO then use GL_POINTS
		void render() override;

	private:
		QOpenGLFunctions* f;
		GLuint VBO;

		unsigned int n;
		float rho0, mu, h0, k;

		vector<int2> selectNeighborhood(glm::vec2 position);

		void compute_density();
		void compute_pressure();
		void compute_gradient();
		void compute_laplacian();
		void integrate();

		void balanceField();
		void balanceBoundaryField();

		float W(glm::vec2 r_rb, float h);
		glm::vec2 gradW(glm::vec2 r_rb, float h);
		float lapW(glm::vec2 r_rb, float h);

		vector<SPH_Particle2D*> particles;
		unordered_map<int2, vector<SPH_Particle2D*>> particleField;
		
		vector<Shape*> boundaries;
		unordered_map<int2, vector<Shape*>> boundaryField;
};

#endif

#endif