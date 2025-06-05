#include "renderer.hpp"

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <glad/glad.h>
#include <stdexcept>

HDC hdc{};
HGLRC context{};

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

void ee::Renderer::InitializeRenderer(ee::Window::InternalInfo& ifo) {
    
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1 };
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    hdc = GetDC(static_cast<HWND>(ifo.hwnd));
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    HGLRC tempContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempContext);

    auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglCreateContextAttribsARB)
        throw std::runtime_error("Failed to create OpenGL context!");

    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };

    HGLRC realContext = wglCreateContextAttribsARB(hdc, nullptr, attribs);
    if (!realContext) throw std::runtime_error("Failed to create OpenGL 4.6 context!");

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempContext);

    wglMakeCurrent(hdc, realContext);

    if (!gladLoadGL())
        throw std::runtime_error("Failed to initialize OpenGL!");

    context = realContext;
}

void ee::Renderer::ClearScreen(ee::FVec4 color) {
    glClearColor(color.x, color.y, color.z, color.r);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ee::Renderer::SwapBuffers() {
    ::SwapBuffers(hdc);
}

void ee::Renderer::SetViewport(ee::IVec2 size) {
    glViewport(0, 0, size.w, size.h);
}