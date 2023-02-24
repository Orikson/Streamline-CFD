#include "sph2d.h"

#ifdef SPH_2D

SPH_2D_CPU::SPH_2D_CPU(QOpenGLFunctions* f, float dt, float rho, float mu, float k, vector<SPH_Particle2D*> init, vector<Shape*> boundaries) : Fluid("SPH_2D", dt), f(f), rho0(rho), mu(mu), particles(init), h0(/*dt * 4.0f*/0.01f), n(init.size()), k(k), boundaries(boundaries) {
	balanceField();
	balanceBoundaryField();

	vector<GLPoint> verts;
	for (int i = 0; i < n; i ++) {
		GLPoint tmp;
		tmp.pos = glm::vec3(init[i]->position, 0);
		tmp.col = glm::vec3(1, 1, 1);
		verts.push_back(tmp);
	}

	f->glGenBuffers(1, &VBO);
	f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
	f->glBufferData(GL_ARRAY_BUFFER, sizeof(GLPoint) * verts.size(), verts.data(), GL_STATIC_DRAW);
}
SPH_2D_CPU::~SPH_2D_CPU() {}

void SPH_2D_CPU::step() {
	compute_density();
	compute_pressure();
	compute_gradient();
	compute_laplacian();
	//compute_acceleration(); // e.g. from moving objects
	integrate();

	balanceField();
}

void SPH_2D_CPU::render() {
	vector<GLPoint> verts;
	for (int i = 0; i < n; i++) {
		GLPoint tmp;
		tmp.pos = glm::vec3(particles[i]->position * 2.0f - glm::vec2(1, 1), 0);
		tmp.col = glm::vec3(1, 1, 1);
		verts.push_back(tmp);
	}

	glPointSize(16);
	f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
	f->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLPoint) * verts.size(), verts.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(GLPoint), (void*)offsetof(GLPoint, pos));
	glColorPointer(3, GL_FLOAT, sizeof(GLPoint), (void*)offsetof(GLPoint, col));
	f->glDrawArrays(GL_POINTS, 0, verts.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	f->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

vector<int2> SPH_2D_CPU::selectNeighborhood(glm::vec2 position) {
	glm::vec2 coord = position / h0;
	glm::vec2 quadrant = glm::vec2(floor(coord.x * 2.0f) / 2.0f - floor(coord.x), floor(coord.y * 2.0f) / 2.0f - floor(coord.y)) * 4.0f - 1.0f;
	coord = glm::vec2(floor(coord.x), floor(coord.y));
	vector<int2> neighborhood;
	neighborhood.push_back(coord);
	neighborhood.push_back(coord + glm::vec2(quadrant.x, 0));
	neighborhood.push_back(coord + glm::vec2(0, quadrant.y));
	neighborhood.push_back(coord + glm::vec2(quadrant.x, quadrant.y));
	return neighborhood;
}

void SPH_2D_CPU::compute_density() {
	// compute rho for each particle
	for (int i = 0; i < n; i ++) {
		SPH_Particle2D* pi = particles[i];
		pi->density = 0;

		// Select particle neighborhood
		vector<int2> neighborhood = selectNeighborhood(pi->position);

		// Compute density
		// rho i = sigma mj W(r-rj, h)
		for (int k = 0; k < neighborhood.size(); k ++) {
			vector<SPH_Particle2D*> neighborParticles = particleField[neighborhood[k]];
			for (int j = 0; j < neighborParticles.size(); j ++) {
				SPH_Particle2D* pj = neighborParticles[j];
				pi->density += pj->mass * W(pi->position - pj->position, h0);
			}
		}
	}
}
void SPH_2D_CPU::compute_pressure() {
	// compute p for each particle
	for (int i = 0; i < n; i++) {
		SPH_Particle2D* pi = particles[i];
		pi->pressure = k * (pi->density - rho0);
	}
}
void SPH_2D_CPU::compute_gradient() {
	// compute del p / rho for each particle
	for (int i = 0; i < n; i++) {
		SPH_Particle2D* pi = particles[i];
		pi->pressureGrad = glm::vec2(0, 0);

		// Select particle neighborhood
		vector<int2> neighborhood = selectNeighborhood(pi->position);

		// Compute density
		// rho i = sigma mj W(r-rj, h)
		for (int k = 0; k < neighborhood.size(); k ++) {
			vector<SPH_Particle2D*> neighborParticles = particleField[neighborhood[k]];
			for (int j = 0; j < neighborParticles.size(); j ++) {
				SPH_Particle2D* pj = neighborParticles[j];
				pi->pressureGrad += pj->mass * (pi->pressure / (pi->density * pi->density) + pj->pressure / (pj->density * pj->density)) * gradW(pi->position - pj->position, h0);
			}
		}
	}
}
void SPH_2D_CPU::compute_laplacian() {
	// compute del p / rho for each particle
	for (int i = 0; i < n; i++) {
		SPH_Particle2D* pi = particles[i];
		pi->lapV = glm::vec2(0, 0);

		// Select particle neighborhood
		vector<int2> neighborhood = selectNeighborhood(pi->position);

		// Compute density
		// rho i = sigma mj W(r-rj, h)
		for (int k = 0; k < neighborhood.size(); k ++) {
			vector<SPH_Particle2D*> neighborParticles = particleField[neighborhood[k]];
			for (int j = 0; j < neighborParticles.size(); j ++) {
				SPH_Particle2D* pj = neighborParticles[j];
				pi->lapV += pj->mass * ((pj->velocity - pi->velocity)/pj->density) * lapW(pi->position - pj->position, h0);
			}
		}
		pi->lapV *= mu / pi->density;
	}
}
void SPH_2D_CPU::integrate() {
	// compute dv/dt, dx/dt for each particle
	for (int i = 0; i < n; i++) {
		SPH_Particle2D* pi = particles[i];
		glm::vec2 a = glm::vec2(0, -GRAVITY) - pi->pressureGrad + pi->lapV;
		pi->velocity += a * deltaT;
		glm::vec2 oldPosition = pi->position;
		pi->position += pi->velocity * deltaT;
		
		// dampen
		pi->velocity *= 0.99;

		// implements naive boundary conditions
		// accurate boundary conditions would consider sampling falloff in kernels
		// this is, however, sufficient for general cg purposes
		
		// domain bounce back
		// Select particle neighborhood
		vector<int2> neighborhood = selectNeighborhood(pi->position);

		for (int k = 0; k < neighborhood.size(); k ++) {
			vector<Shape*> neighborBoundaries = boundaryField[neighborhood[k]];
			for (int j = 0; j < neighborBoundaries.size(); j ++) {
				if (neighborBoundaries[j]->contains(pi->position)) {
					pi->position = oldPosition;
					pi->velocity = glm::reflect(pi->velocity, neighborBoundaries[j]->normal(pi->position));
					
					// Break out of nested loop without extra logic
					goto CONTINUE;
				}
			}
		}
		CONTINUE:;
	}
}

void SPH_2D_CPU::balanceField() {
	// zero map
	for (auto & [key, cell] : particleField) {
		cell.clear();
	}

	// fill map
	for (int i = 0; i < n; i ++) {
		SPH_Particle2D* particle = particles[i];
		int2 key = int2(particle->position.x / h0, particle->position.y / h0);
		if (particleField.count(key) == 0) {
			particleField[key] = vector<SPH_Particle2D*>();
		}
		particleField[key].push_back(particle);
	}
}
void SPH_2D_CPU::balanceBoundaryField() {
	for (int i = 0; i < boundaries.size(); i ++) {
		vector<int2> boundaryCells = boundaries[i]->intersect(glm::vec2(h0, h0));
		for (int j = 0; j < boundaryCells.size(); j ++) {
			if (boundaryField.count(boundaryCells[j]) == 0) {
				boundaryField[boundaryCells[j]] = vector<Shape*>();
			}
			boundaryField[boundaryCells[j]].push_back(boundaries[i]);
		}
	}
}

float SPH_2D_CPU::W(glm::vec2 r_rb, float h) {
	if (glm::length(r_rb) > h) { return 0; }
	return (315.0f / (64.0f * PI * powf(h, 9.0f))) * powf(h * h - glm::length(r_rb) * glm::length(r_rb), 3.0f);
}

glm::vec2 SPH_2D_CPU::gradW(glm::vec2 r_rb, float h) {
	if (r_rb == glm::vec2(0, 0)) { return glm::vec2(0, 0); }
	if (glm::length(r_rb) > h) { return glm::vec2(0, 0); }
	return (-45.0f / (PI * powf(h, 6.0f))) * powf(h - glm::length(r_rb), 2.0f) * glm::normalize(r_rb);
}

float SPH_2D_CPU::lapW(glm::vec2 r_rb, float h) {
	if (glm::length(r_rb) > h) { return 0; }
	return (45.0f / (PI * powf(h, 6.0f))) * (h - glm::length(r_rb));
}

#endif