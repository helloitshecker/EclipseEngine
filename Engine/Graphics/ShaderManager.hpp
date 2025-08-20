#pragma once

#include <filesystem>

namespace Eclipse {
        class ShaderManager {
        public:
                ShaderManager();
                void compile_all();
                std::vector<std::filesystem::path> shaderPaths;
        };

}