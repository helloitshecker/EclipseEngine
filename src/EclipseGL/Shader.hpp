#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace e {
	class shader {
	public:
		uint32_t ID;

		shader(const char* vertexPath, const char* fragmentPath);

		void use() const;

		void setValue(const std::string& name, bool value) const;
		void setValue(const std::string& name, int value) const;
		void setValue(const std::string& name, float value) const;
		void setValue(const std::string& name, const glm::vec2& value) const;
		void setValue(const std::string& name, float x, float y) const;
		void setValue(const std::string& name, const glm::vec3& value) const;
		void setValue(const std::string& name, float x, float y, float z) const;
		void setValue(const std::string& name, const glm::vec4& value) const;
		void setValue(const std::string& name, float x, float y, float z, float w) const;
		void setValue(const std::string& name, const glm::mat2& value) const;
		void setValue(const std::string& name, const glm::mat3& value) const;
		void setValue(const std::string& name, const glm::mat4& value) const;
	};
};