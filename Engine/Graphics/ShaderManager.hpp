#pragma once

#include <Engine/Core/Types.hpp>
#include <Engine/Core/FileSystem.hpp>
#include <Engine/Core/Error.hpp>
#include <Engine/Core/VirtualFS.hpp>

#include <shaderc/shaderc.hpp>

namespace Eclipse {
class ShaderManager {
public:

    enum class ShaderOptimizationLevel {
        PERFORMANCE,
        SIZE,
        NONE
    };

    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        COMPUTE
    };

    struct ShaderManagerCreateInfo {
        ShaderOptimizationLevel optimizationLevel = ShaderOptimizationLevel::PERFORMANCE;
    };

    struct ShaderCreateInfo {
        VirtualFS::File& file;
        std::string name;
        std::string entry = "main";
        ShaderType type;
    };

    struct ShaderObject {
        std::vector<u32> spirv_storage;
        std::span<const u32> spirv_code;

    private:
        u64 index = UINT64_MAX;
        friend class ShaderManager;
    };

    bool error = false;

    ShaderManager(const ShaderManagerCreateInfo& createInfo);

    std::optional<ShaderObject> compile(const ShaderCreateInfo& shaderInfo);
private:
    shaderc::Compiler compiler;
    shaderc::CompileOptions compileOptions;

    std::vector<ShaderObject> shaders;

    std::unique_ptr<VirtualFS> vfs;
};
}
