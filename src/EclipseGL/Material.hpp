#pragma once

#include <EclipseGL/Shader.hpp>
#include <EclipseGL/Texture.hpp>
#include <EclipseUtils/Defines.hpp>

#include <unordered_map>
#include <string>

namespace e {
	class material {
	public:
		std::string name;
		e::shader* shader;
		std::unordered_map<const char*, glm::vec3*> vec3Uniforms;
		std::unordered_map<const char*, glm::mat4*> mat4Uniforms;
		std::unordered_map<const char*, float*> floatUniforms;
		std::unordered_map<const char*, float**> floatDUniforms;
		std::unordered_map<const char*, e::texture*> textures;

		material(const std::string name, e::shader* shader) : name(name), shader(shader) {}

		void set(const char* name, glm::vec3* value);
		void set(const char* name, glm::mat4* value);
		void set(const char* name, float* value);
		void set(const char* name, float** value);
		void set(const char* name, e::texture* texture);
	
		void apply();
	};
};
