#pragma once
#include <platform/window.hpp>

#ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif
#ifndef WGL_CONTEXT_MINOR_VERSION_ARB 
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif
#ifndef WGL_CONTEXT_LAYER_PLANE_ARB   
#define WGL_CONTEXT_LAYER_PLANE_ARB   0x2093
#endif
#ifndef WGL_CONTEXT_FLAGS_ARB         
#define WGL_CONTEXT_FLAGS_ARB         0x2094
#endif
#ifndef WGL_CONTEXT_PROFILE_MASK_ARB 
#define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126
#endif
#ifndef WGL_CONTEXT_CORE_PROFILE_BIT_ARB
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#endif
#ifndef WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

#include <core/types.hpp>

namespace ee {
	class Renderer {
	public:
		void InitializeRenderer(ee::Window& wc);
		void ClearScreen(ee::FVec4 color);
		void SwapBuffers();
		void SetViewport(ee::IVec2 size);
		void SetVsync(bool enabled);
	};
};