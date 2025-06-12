#include "GL.hpp"
#include <EclipseUtils/Logger.hpp>

using namespace e;

gl::gl(et::procaddr procaddr) {
	if (!gladLoadGLLoader(procaddr)) {
		LOG::SEND("Failed to find opengl context!");
	}
}

void gl::set_viewport_size(et::i32 x, et::i32 y, et::u64 width, et::u64 height) const {
	glViewport(x, y, width, height);
}

void gl::enable(et::u32 flag) {
	glEnable(flag);
}

gl::~gl() {

}