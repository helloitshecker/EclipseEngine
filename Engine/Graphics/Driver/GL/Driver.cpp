#include "Driver.hpp"

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <Engine/Common/Assert.hpp>

Eclipse::GLServer::GLServer* Eclipse::GLServer::Create(CreateInfo& create_info) {
    SDL_GLContext glcontext = SDL_GL_CreateContext((SDL_Window*)(create_info.window->ptr));
    SDL_GL_MakeCurrent((SDL_Window*)(create_info.window->ptr), glcontext);

    ECL_ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "Failed to load GL Function Pointers!");

    Eclipse::GLServer::GLServer* ptr = Eclipse::MemoryPool::Allocate<Eclipse::GLServer::GLServer>(create_info.mempool, 1);
    ptr->glcontext = glcontext;

    return ptr;
}

void Eclipse::GLServer::Destroy() {

}

void Eclipse::GLServer::GL_Clear(Eclipse::FloatVec4& val) {
    glClearColor(val.r, val.g, val.b, val.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
