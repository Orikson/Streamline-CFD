// Defines basic boundaries for scenes in a format accessible for SceneBuilder or direct integration into fluid solvers

#ifndef SHAPES_H
#define SHAPES_H

#include <defines.h>
#include <QOpenGLFunctions>
#include <glm/glm.hpp>
#include <algorithm> 
using std::min; using std::max;
#include <vector>
using std::vector;

// extensions to glm for integers (clm?)
struct int2 {
	int x, y;
	inline int2(int xs, int ys) {
		x = xs; y = ys;
	}
	inline int2(glm::vec2 v) {
		x = int(v.x); y = int(v.y);
	}
	bool operator== (const int2& v) const {
		return (x == v.x && y == v.y);
	}
};
struct int3 {
	int x, y, z;
	inline int3(int xs, int ys, int zs) {
		x = xs; y = ys; z = zs;
	}
	inline int3(glm::vec3 v) {
		x = int(v.x); y = int(v.y); z = int(v.z);
	}
	bool operator== (const int3& v) const {
		return (x == v.x && y == v.y && z == v.z);
	}
};

class Shape {
	public:
		// Write a specified tag value into provided data of given resolution and world dimension
		virtual void blit(char* data, glm::vec3 resolution, glm::vec3 dimensions) = 0;
		virtual void blit(char* data, glm::vec2 resolution, glm::vec2 dimensions) = 0;
		virtual bool contains(glm::vec2 point) = 0;
		virtual bool contains(glm::vec3 point) = 0;
		virtual glm::vec2 normal(glm::vec2 point) = 0;
		virtual glm::vec3 normal(glm::vec3 point) = 0;
		// Returns the set of cell indices for a given dimension cell
		virtual vector<int2> intersect(glm::vec2 h) = 0;
		virtual vector<int3> intersect(glm::vec3 h) = 0;

		glm::vec2 getVelocity2D() {	return v_2d; }
		glm::vec3 getVelocity3D() { return v_3d; }

	protected:
		unsigned int tag;
		glm::vec2 v_2d;
		glm::vec3 v_3d;
};

class Circle : virtual public Shape {
	public:
		Circle(glm::vec2 center, float radius, unsigned int tag) : center2D(center), radius(radius) { tag = tag; }
		Circle(glm::vec3 center, float radius, unsigned int tag) : center3D(center), radius(radius) { tag = tag; }
		~Circle() {}

		void blit(char* data, glm::vec3 resolution, glm::vec3 dimensions) override {
			glm::vec3 cCenter = center3D / dimensions * resolution;
			float r = radius / dimensions.x * resolution.x;
			for (unsigned int i = max(cCenter.x - r, 0.0f); i < min(cCenter.x + r, resolution.x); i ++) {
				for (unsigned int j = max(cCenter.y - r, 0.0f); j < min(cCenter.y + r, resolution.y); j ++) {
					for (unsigned int k = max(cCenter.z - r, 0.0f); k < min(cCenter.z + r, resolution.z); k ++) {
						glm::vec3 point = glm::vec3(i, j, k);
						if (glm::distance(point, cCenter) <= r) {
							data[(int)(i + j * resolution.x + k * resolution.x * resolution.y)] |= tag;
						}
					}
				}
			}
		}

		void blit(char* data, glm::vec2 resolution, glm::vec2 dimensions) override {
			glm::vec2 cCenter = center2D / dimensions * resolution;
			float r = radius / dimensions.x * resolution.x;
			for (unsigned int i = max(cCenter.x - r, 0.0f); i < min(cCenter.x + r, resolution.x); i ++) {
				for (unsigned int j = max(cCenter.y - r, 0.0f); j < min(cCenter.y + r, resolution.y); j ++) {
					glm::vec2 point = glm::vec2(i, j);
					if (glm::distance(point, cCenter) <= r) {
						data[(int)(i + j * resolution.x)] |= tag;
					}
				}
			}
		}

		bool contains(glm::vec2 point) override { 
			return glm::distance(point, center2D) <= radius;
		};
		bool contains(glm::vec3 point) override {
			return glm::distance(point, center3D) <= radius;
		}

		glm::vec2 normal(glm::vec2 point) override {
			return glm::normalize(point - center2D);
		}
		glm::vec3 normal(glm::vec3 point) override {
			return glm::normalize(point - center3D);
		}

		vector<int2> intersect(glm::vec2 h) override {
			vector<int2> cells;

			glm::vec2 cCenter = center2D / h;
			float r = radius / h.x;
			for (unsigned int i = cCenter.x - r; i < cCenter.x + r; i ++) {
				for (unsigned int j = cCenter.y - r; j < cCenter.y + r; j ++) {
					glm::vec2 point = glm::vec2(i, j);
					if (glm::distance(point, cCenter) <= r) {
						cells.push_back(int2(point));
					}
				}
			}

			return cells;
		}
		vector<int3> intersect(glm::vec3 h) override {
			vector<int3> cells;

			glm::vec3 cCenter = center3D / h;
			float r = radius / h.x;
			for (unsigned int i = cCenter.x - r; i < cCenter.x + r; i ++) {
				for (unsigned int j = cCenter.y - r; j < cCenter.y + r; j ++) {
					for (unsigned int k = cCenter.z - r; k < cCenter.z + r; k ++) {
						glm::vec3 point = glm::vec3(i, j, k);
						if (glm::distance(point, cCenter) <= r) {
							cells.push_back(int3(point));
						}
					}
				}
			}

			return cells;
		}

	private:
		glm::vec2 center2D;
		glm::vec3 center3D;
		float radius;
};

class BBRectangle : virtual public Shape {
	public:
		BBRectangle(glm::vec2 center, glm::vec2 dimension, unsigned int tag) : center2D(center), dimension2D(dimension) { tag = tag; }
		BBRectangle(glm::vec3 center, glm::vec3 dimension, unsigned int tag) : center3D(center), dimension3D(dimension) { tag = tag; }
		~BBRectangle() {}

		void blit(char* data, glm::vec3 resolution, glm::vec3 dimensions) override {
			glm::vec3 cCenter = center3D / dimensions * resolution;
			glm::vec3 ddimension3D = dimension3D / dimensions * resolution;
			for (unsigned int i = max(cCenter.x - ddimension3D.x * 0.5f, 0.0f); i < min(cCenter.x + ddimension3D.x * 0.5f, resolution.x); i++) {
				for (unsigned int j = max(cCenter.y - ddimension3D.y * 0.5f, 0.0f); j < min(cCenter.y + ddimension3D.y * 0.5f, resolution.y); j++) {
					for (unsigned int k = max(cCenter.z - ddimension3D.z * 0.5f, 0.0f); k < min(cCenter.z + ddimension3D.z * 0.5f, resolution.z); k++) {
						data[(int)(i + j * resolution.x + k * resolution.x * resolution.y)] |= tag;
					}
				}
			}
		}

		void blit(char* data, glm::vec2 resolution, glm::vec2 dimensions) override {
			glm::vec2 cCenter = center2D / dimensions * resolution;
			glm::vec2 ddimension2D = dimension2D / dimensions * resolution;
			for (unsigned int i = max(cCenter.x - ddimension2D.x * 0.5f, 0.0f); i < min(cCenter.x + ddimension2D.x * 0.5f, resolution.x); i++) {
				for (unsigned int j = max(cCenter.y - ddimension2D.y * 0.5f, 0.0f); j < min(cCenter.y + ddimension2D.y * 0.5f, resolution.y); j++) {
					data[(int)(i + j * resolution.x)] |= tag;
				}
			}
		}

		bool contains(glm::vec2 point) override {
			return
				point.x > center2D.x - dimension2D.x * 0.5 &&
				point.x < center2D.x + dimension2D.x * 0.5 &&
				point.y > center2D.y - dimension2D.y * 0.5 &&
				point.y < center2D.y + dimension2D.y * 0.5;
		}
		bool contains(glm::vec3 point) override {
			return
				point.x > center3D.x - dimension3D.x * 0.5 &&
				point.x < center3D.x + dimension3D.x * 0.5 &&
				point.y > center3D.y - dimension3D.y * 0.5 &&
				point.y < center3D.y + dimension3D.y * 0.5 &&
				point.z > center3D.z - dimension3D.z * 0.5 &&
				point.z < center3D.z + dimension3D.z * 0.5;
		}

		glm::vec2 normal(glm::vec2 point) override {
			glm::vec2 norm = point - center2D;
			glm::vec2 temp = glm::clamp(norm, dimension2D * 0.5f, -dimension2D * 0.5f);
			if (temp == point) {
				// inside rectangle
				float R = dimension2D.x * 0.5 - norm.x;
				float L = norm.x + dimension2D.x * 0.5;
				float T = dimension2D.y * 0.5 - norm.y;
				float B = norm.y + dimension2D.y * 0.5;
				float minimum = min(R, min(L, min(T, B)));
				if (minimum == R) { return glm::vec2(R, 0); }
				else if (minimum == L) { return glm::vec2(-L, 0); }
				else if (minimum == T) { return glm::vec2(0, T); }
				else { return glm::vec2(0, -B); }
			}
			return norm - temp;
		}
		glm::vec3 normal(glm::vec3 point) override {
			glm::vec3 norm = point - center3D;
			glm::vec3 temp = glm::clamp(norm, dimension3D * 0.5f, -dimension3D * 0.5f);
			if (temp == point) {
				// inside rectangle
				float R = dimension3D.x * 0.5 - norm.x;
				float L = norm.x + dimension3D.x * 0.5;
				float T = dimension3D.y * 0.5 - norm.y;
				float B = norm.y + dimension3D.y * 0.5;
				float U = dimension3D.z * 0.5 - norm.z;
				float D = norm.z + dimension3D.z * 0.5;
				float minimum = min(R, min(L, min(T, min(B, min(U, D)))));
				if (minimum == R) { return glm::vec3(R, 0, 0); }
				else if (minimum == L) { return glm::vec3(-L, 0, 0); }
				else if (minimum == T) { return glm::vec3(0, T, 0); }
				else if (minimum == B) { return glm::vec3(0, -B, 0); }
				else if (minimum == U) { return glm::vec3(0, 0, U); }
				else { return glm::vec3(0, 0, -D); }
			}
			return norm - temp;
		}

		vector<int2> intersect(glm::vec2 h) override {
			vector<int2> cells;

			glm::vec2 cCenter = center2D / h;
			glm::vec2 d = dimension2D / h;
			for (int i = cCenter.x - d.x * 0.5; i <= cCenter.x + d.x * 0.5; i++) {
				for (int j = cCenter.y - d.y * 0.5; j <= cCenter.y + d.y * 0.5; j++) {
					glm::vec2 point = glm::vec2(i, j);
					cells.push_back(int2(point));
				}
			}

			return cells;
		}
		vector<int3> intersect(glm::vec3 h) override {
			vector<int3> cells;

			glm::vec3 cCenter = center3D / h;
			glm::vec3 d = dimension3D / h;
			for (int i = cCenter.x - d.x; i < cCenter.x + d.x; i++) {
				for (int j = cCenter.y - d.y; j < cCenter.y + d.y; j++) {
					for (int k = cCenter.z - d.z; k < cCenter.z + d.z; k++) {
						glm::vec3 point = glm::vec3(i, j, k);
						cells.push_back(int3(point));
					}
				}
			}

			return cells;
		}

	private:
		glm::vec2 center2D;
		glm::vec3 center3D;
		glm::vec2 dimension2D;
		glm::vec3 dimension3D;
};


#endif