#include "Material.hpp"

using namespace e;

void material::set(const char* name, glm::vec3* value) {
	vec3Uniforms[name] = value;
}

void material::set(const char* name, glm::mat4* value) {
	mat4Uniforms[name] = value;
}

void material::set(const char* name, float* value) {
	floatUniforms[name] = value;
}

void material::set(const char* name, float** value) {
	floatDUniforms[name] = value;
}

void material::set(const char* name, e::texture* texture) {
	textures[name] = texture;
}

void material::apply() {
	shader->use();
	for (const auto& pair : vec3Uniforms) {
		shader->setValue(pair.first, *pair.second);
	}
	for (const auto& pair : mat4Uniforms) {
		shader->setValue(pair.first, *pair.second);
	}
	for (const auto& pair : floatUniforms) {
		shader->setValue(pair.first, *pair.second);
	}
	for (const auto& pair : textures) {
		pair.second->bind();
		shader->setValue(pair.first, (int)pair.second->_texture);
	}
}