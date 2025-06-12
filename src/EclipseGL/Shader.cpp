#include "Shader.hpp"
#include <EclipseUtils/Logger.hpp>

using namespace e;

shader::shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		
		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		LOG::SEND("Failed to read shader!");
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	uint32_t vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		LOG::SEND("Vertex Shader Compilation Failed!");
		std::cout << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		LOG::SEND("Fragment Shader Compilation Failed!");
		std::cout << infoLog << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		LOG::SEND("Shader Linking Failed :(");
		std::cout << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void shader::use() const {
	glUseProgram(ID);
}

void shader::setValue(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void shader::setValue(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void shader::setValue(const std::string& name, float value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void shader::setValue(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void shader::setValue(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void shader::setValue(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void shader::setValue(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void shader::setValue(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void shader::setValue(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void shader::setValue(const std::string& name, const glm::mat2& value) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void shader::setValue(const std::string& name, const glm::mat3& value) const {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void shader::setValue(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}