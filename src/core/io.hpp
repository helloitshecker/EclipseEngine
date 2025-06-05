#pragma once

#include <fmt/core.h>
#include <fmt/ostream.h>

namespace ee {
	namespace Out {
		template<typename... Args>
		void Print(const char* format, Args&&... args) {
			fmt::print(format, std::forward<Args>(args)...);
		}
	};
};