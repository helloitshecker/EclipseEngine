#pragma once

#include <glad/glad.h>
#include <EclipseUtils/Defines.hpp>
#include <vector>

namespace e {
	class mesh {
	private:
		et::u32 VAO{};
		et::u32 VBO{};
		et::u32 EBO{};
		et::u32 indexCount{};

	public:
		mesh(const std::vector<float>& vertices, const std::vector<et::u32>& indices);
		~mesh();

		// Prevent Copying (Avoid double deletion issues)
		mesh(const mesh&) = delete;
		mesh& operator=(const mesh&) = delete;

		// Implement Move Constructor
		mesh(mesh&& other) noexcept;

		// Implement Move Assignment
		mesh& operator=(mesh&& other) noexcept;

		void draw() const;
	};
}
