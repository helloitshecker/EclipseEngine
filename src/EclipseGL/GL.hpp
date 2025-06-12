#pragma once

#include <glad/glad.h>
#include <EclipseGL/Shader.hpp>
#include <EclipseGL/Texture.hpp>
#include <EclipseUtils/Defines.hpp>

namespace et {
	typedef GLADloadproc procaddr;
};

namespace e {
	class gl {
	public:
		gl(et::procaddr procaddr);
		void set_viewport_size(et::i32 x, et::i32 y, et::u64 width, et::u64 height) const;
		void enable(et::u32 flag);
		~gl();
	};
};