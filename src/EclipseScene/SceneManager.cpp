#include "SceneManager.hpp"

using namespace e;

scene::scene() {
	camera _mainCamera = camera(glm::vec3(0.0f, 0.0f, 3.0f));
	mainCamera = &_mainCamera;
}

scene::scene(camera& camera) {
	mainCamera = &camera;
}

void scene::push_object(et::object object) {
	objects.push_back(object);
}
