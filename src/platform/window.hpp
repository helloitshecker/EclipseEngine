#pragma once

#include <string>
#include <memory>
#include <core/types.hpp>
#include <stdexcept>

inline const std::runtime_error WindowCreationFailed("Failed to create window");
constexpr int USERDEFAULT = 0x80000000;

namespace ee {
	class Window {
	public:
		
		enum class WindowState {
			HIDE = 0,
			SHOWNORMAL = 1,
			NORMAL = 1,
			SHOWMINIMIZED = 2,
			SHOWMAXIMIZED = 3,
			MAXIMIZE = 3,
			SHOWNOACTIVATE = 4,
			SHOW = 5,
			MINIMIZE = 6,
			SHOWMINNOACTIVE = 7,
			SHOWNA = 8,
			RESTORE = 9,
			SHOWDEFAULT = 10,
			FORCEMINIMIZE = 11
		};

		struct CreateInfo {
			std::wstring title;
			ee::IVec2 size;
			ee::IVec2 position;
			bool fullscreen;
			bool resizable;
			bool adaptiveres;
			bool vsync;
			bool darktheme;
			WindowState state;
		};

		struct CurrentState {
			ee::IVec2 size;
			bool resize;
		};

		Window(CreateInfo& create_info);
		void PollEvents();
		bool ShouldClose();
		void SetFullscreenState(bool fullscreen, bool adpativeres, ee::IVec2 res);
		void SetTitleBarDarkMode(bool mode);
		CurrentState* GetCurrentStatePtr();
		void* GetWindowHandle();
		~Window();
	};
};