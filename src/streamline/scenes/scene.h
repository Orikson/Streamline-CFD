#ifndef SCENE_H
#define SCENE_H

class Scene {
	public:
		Scene(bool useScreenPlane) : usePlane(useScreenPlane) {}
		~Scene() {}
		virtual void render() {}
		virtual void step() {}
		virtual void updateMouse(float mX, float mY, float mS) {}
		virtual void updateKeyboard(vector<char> keys) {}
		bool useScreenPlane() { return usePlane; }

	protected:
		bool usePlane;
};

#endif