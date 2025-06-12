#include "Mesh.hpp"

using namespace e;

mesh::mesh(const std::vector<float>& vertices, const std::vector<et::u32>& indices) {
	// Get array size
	indexCount = indices.size();
	et::u32 argumentCount = 6; // Should be configurable if layout changes

	// Generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Upload index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(et::u32), indices.data(), GL_STATIC_DRAW);

	// Position Attributes (0 -> 2) vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, argumentCount * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal Attributes (3 -> 5) normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, argumentCount * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind
}

void mesh::draw() const {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

mesh::~mesh() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

// Move Constructor
mesh::mesh(mesh&& other) noexcept
	: VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), indexCount(other.indexCount) {
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

// Move Assignment
mesh& mesh::operator=(mesh&& other) noexcept {
	if (this != &other) {
		// Cleanup existing resources
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);

		// Move ownership
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		indexCount = other.indexCount;

		// Reset old object
		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
	}
	return *this;
}
