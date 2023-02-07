#ifndef SCENE_H
#define SCENE_H

class Scene {
	public:
		Scene(bool useScreenPlane) : usePlane(useScreenPlane) {}
		~Scene() {}
		virtual void render() {}
		virtual void step() {}
		bool useScreenPlane() { return usePlane; }

	protected:
		bool usePlane;
};

#endif