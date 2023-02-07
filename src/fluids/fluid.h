#ifndef FLUID_H
#define FLUID_H

#include <string>
using std::string;

class Fluid {
	public:
		Fluid(string name, float dt);
		~Fluid();

		virtual void step();
		virtual void render();

		float dt();
		string getName();

	protected:
		float deltaT;
		string name;
};

#endif