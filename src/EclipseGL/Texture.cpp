#include "Texture.hpp"
#include <EclipseUtils/Logger.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace e;

texture::texture(std::string texture_path) {
	int width, height, nrChannels;
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_set_flip_vertically_on_load(true);
	
	unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		LOG::SEND("Failed to load texture!");
	}
	
	stbi_image_free(data);
}

void texture::bind() {
	glBindTexture(GL_TEXTURE_2D, _texture);
}

void texture::bind(GLenum index) {
	glActiveTexture(index);
	bind();
}