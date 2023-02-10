#include "vphmcf.h"

#ifdef VPH
#ifdef VPH_2D
Bubble2D::Bubble2D(float parameterize, vector<Vertex2D> verts, QOpenGLFunctions* f, QObject* parent) : Fluid("bubble2D", parameterize * 0.01), vertices(verts), f(f) {
	n = vertices.size();
	surfaceTension = parameterize * 10;

	// Initialize VAO
	VAO = new QOpenGLVertexArrayObject(parent);
	VAO->create();
	VAO->bind();

	vector<float> positions;
	vector<unsigned int> indices;
	for (int i = 0; i < n; i++) {
		positions.insert(positions.end(), { vertices.at(i).pos.x, vertices.at(i).pos.y, 0 });
		indices.push_back(i);
	}

	f->glGenBuffers(1, &VBO);
	f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
	f->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);

	f->glGenBuffers(1, &EBO);
	f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	f->glEnableVertexAttribArray(0);

	f->glLineWidth(10);

	VAO->release();
}
Bubble2D::~Bubble2D() {}

void Bubble2D::step() {
	calcAreaAndLength();
	float initArea = area;

	calcCurvatureNormals();

	for (unsigned int i = 0; i < n; i ++) {
		glm::vec2 force = -vertices[i].normal * surfaceTension;
		vertices[i].vel += force * deltaT;
		vertices[i].pos += vertices[i].vel * deltaT;
	}
	
	regularize();

	calcAreaAndLength();
	float deltaA = initArea - area;
	float d = deltaA / length;

	calcTangents();
	calcNormals();

	for (unsigned int i = 0; i < n; i ++) {
		glm::vec2 dx = d * vertices[i].normal;
		vertices[i].pos += dx;
		vertices[i].vel += d / deltaT * vertices[i].normal;
	}
}

void Bubble2D::render() {
	step();
	updateVAO();

	VAO->bind();

	f->glDrawElements(GL_LINE_LOOP, n, GL_UNSIGNED_INT, 0);

	VAO->release();
}

void Bubble2D::regularize() {
	calcAreaAndLength();
	
	unsigned int ei = rand() % n;

	vector<Vertex2D> newVerts;
	newVerts.push_back(Vertex2D(vertices[ei].pos));
	newVerts[0].vel = vertices[ei].vel;

	glm::vec2 curp = vertices[ei].pos;
	glm::vec2 curv = vertices[ei].vel;
	glm::vec2 nextp = vertices[(ei + 1u) % n].pos;
	glm::vec2 nextv = vertices[(ei + 1u) % n].vel;
	unsigned int nexti = (ei + 2u) % n;

	float dl = length / n;

	for (unsigned int i = 0; i < n; i++) {
		float accum = 0;
		float dist = glm::distance(curp, nextp);

		while (dl - accum > dist) {
			accum += dist;
			curp = nextp;
			curv = nextv;
			nextp = vertices[nexti].pos;
			nextv = vertices[nexti].vel;
			nexti = (nexti + 1u) % n;
			dist = glm::distance(curp, nextp);
		}
		curp = glm::mix(curp, nextp, (dl - accum) / dist);
		curv = glm::mix(curv, nextv, (dl - accum) / dist);
		Vertex2D tmp(curp);
		tmp.vel = curv;
		newVerts.push_back(tmp);
	}

	vertices = newVerts;
}

void Bubble2D::calcAreaAndLength() {
	area = 0; length = 0;
	for (unsigned int i = 0; i < n; i ++) {
		unsigned int j = (i + 1u) % n;
		length += glm::distance(vertices[i].pos, vertices[j].pos);
		area += vertices[i].pos.x * vertices[j].pos.y - vertices[i].pos.y * vertices[j].pos.x;
	}
	area *= 0.5;
}

void Bubble2D::calcTangents() {
	for (unsigned int i = 0; i < n; i ++) {
		unsigned int j = (i - 1u + n) % n;
		unsigned int k = (i + 1u) % n;
		vertices[i].tangent = vertices[j].pos - vertices[k].pos;
	}
}

void Bubble2D::calcCurvatureNormals() {
	for (unsigned int i = 0; i < n; i++) {
		unsigned int j = (i - 1u + n) % n;
		unsigned int k = (i + 1u) % n;
		glm::vec2 p = vertices[i].pos;
		glm::vec2 q1 = glm::normalize(p - vertices[j].pos);
		glm::vec2 q2 = glm::normalize(p - vertices[k].pos);
		vertices[i].normal = q1 + q2;
	}
}

void Bubble2D::calcNormals() {
	for (unsigned int i = 0; i < n; i++) {
		vertices[i].normal = glm::normalize(glm::vec2(-vertices[i].tangent.y, vertices[i].tangent.x));
	}
}

void Bubble2D::updateVAO() {
	VAO->bind();

	vector<float> positions;
	for (int i = 0; i < n; i++) {
		positions.insert(positions.end(), { vertices.at(i).pos.x, vertices.at(i).pos.y, 0 });
	}

	f->glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), &positions[0]);

	VAO->release();
}

#endif // VPH_2D

#endif // VPH