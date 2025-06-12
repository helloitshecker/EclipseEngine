#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <EclipseGL/Mesh.hpp>
#include <EclipseGL/Camera.hpp>
#include <EclipseGL/Material.hpp>

#include <vector>
#include <string>

namespace et {

	struct transform {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		glm::mat4 mat4() const {
			glm::mat4 mat = glm::translate(glm::mat4(1.0f), position);
			mat *= glm::mat4_cast(glm::quat(glm::radians(rotation))); // Convert Euler to quaternion
			mat = glm::scale(mat, scale);
			return mat;
		}
	};

	struct object {
		std::string name;
		et::transform transform;
		e::mesh* mesh;
		e::material* material;

		std::vector<object> children;
	};
};

namespace e {
	class scene {
	public:
		std::vector<et::object> objects;
		e::camera* mainCamera{};

		scene();
		scene(e::camera& camera);

		void push_object(et::object);

	};
};
