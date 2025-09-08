#include "ShaderManager.hpp"


namespace Eclipse {
      ShaderManager::ShaderManager(const ShaderManagerCreateInfo& createInfo) {
            switch (createInfo.optimizationLevel) {
                  case ShaderOptimizationLevel::NONE:
                        compileOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
                        break;
                  case ShaderOptimizationLevel::PERFORMANCE:
                        compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
                        break;
                  case ShaderOptimizationLevel::SIZE:
                        compileOptions.SetOptimizationLevel(shaderc_optimization_level_size);
                        break;
            }
      }

      std::optional<Eclipse::ShaderManager::ShaderObject> Eclipse::ShaderManager::compile(const ShaderCreateInfo& shaderInfo) {
            auto shaderFileContent = Eclipse::FileSystem::ReadTextFile(shaderInfo.name);
            if (shaderFileContent == std::nullopt) {
                  Eclipse::LogError("Failed to read shader file \"{}\"", shaderInfo.name);
                  return std::nullopt;
            }

            shaderc_shader_kind shaderKind{};
            switch (shaderInfo.type) {
                  case ShaderType::VERTEX:
                        shaderKind = shaderc_glsl_default_vertex_shader;
                        break;
                  case ShaderType::FRAGMENT:
                        shaderKind = shaderc_glsl_default_fragment_shader;
                        break;
                  case ShaderType::COMPUTE:
                        shaderKind = shaderc_glsl_default_compute_shader;
                        break;
                  default:
                        Eclipse::LogError("Shader type not recognized for shader \"{}\"!", shaderInfo.name);
                        return std::nullopt;
            }

            shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(
                  shaderFileContent.value().data(),
                  shaderFileContent.value().size(),
                  shaderKind,
                  shaderInfo.name.c_str(),
                  shaderInfo.entry.c_str(),
                  compileOptions
            );

            if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
                  Eclipse::LogError("Shader Compilation Error (\"{}\"): {}", shaderInfo.name, compilationResult.GetErrorMessage());
                  return std::nullopt;
            }

            ShaderObject obj {};
            obj.spirv_code = { compilationResult.cbegin(), compilationResult.cend() };
            obj.index = shaders.size();

            shaders.push_back(obj);

            return obj;
      }

}