#include "fluid.h"

Fluid::Fluid(string name, float dt) : name(name), deltaT(dt) {}
Fluid::~Fluid() {}
float Fluid::dt() { return deltaT; }
string Fluid::getName() { return name; }

void Fluid::step() {}
void Fluid::render() {}