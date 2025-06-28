#include "Server.hpp"

#include <Engine/Graphics/Driver/GL/Driver.hpp>

Eclipse::GraphicsServer::GraphicsServer* Eclipse::GraphicsServer::Create(CreateInfo& create_info) {

    Eclipse::GraphicsServer::GraphicsServer* server = Eclipse::MemoryPool::Allocate<Eclipse::GraphicsServer::GraphicsServer>(create_info.mempool, 1);

    if (create_info.window->backend == Eclipse::Window::RenderBackend::GL) {
        Eclipse::GLServer::CreateInfo ci{};
        ci.window = create_info.window;
        ci.mempool = create_info.mempool;

        Eclipse::GLServer::GLServer* gl = Eclipse::GLServer::Create(ci);

        server->clear = &Eclipse::GLServer::GL_Clear;
    }
    else if (create_info.window->backend == Eclipse::Window::RenderBackend::VK) {

    }

    return server;
}

void Eclipse::GraphicsServer::Destroy(GraphicsServer*) {

}
