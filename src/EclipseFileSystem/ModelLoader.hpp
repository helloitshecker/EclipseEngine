#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <array>
#include <EclipseUtils/Defines.hpp>

namespace et {
	struct model {
		std::vector<float> vertices;
		std::vector<et::u32> indices;
	};

	typedef std::array<float, 3> vertex;
};

namespace e {
	class loader {
	public:
		static et::model loadobj(const char* file_location);
	};
};