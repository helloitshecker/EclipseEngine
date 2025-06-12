#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>

namespace e {
	class texture {
	public:
		uint32_t _texture;

		texture(std::string texture_path);
		void bind();
		void bind(GLenum index);
	};
};