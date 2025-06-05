#pragma once

#include <string>
#include <functional>
#include <core/types.hpp>

namespace ee {
	namespace Window {
		
		struct CreateInfo {
			std::string title;
			ee::IVec2 size;
			bool fullscreen;
			bool resizable;
			bool adaptiveres;
			bool vsync;
		};

		typedef void* Window;

		Window		Create(CreateInfo&);
		void		Destroy(Window);
		bool		ShouldClose(Window);
		void		Update(Window);
		ee::IVec2	QuerySize(Window);
	};
};