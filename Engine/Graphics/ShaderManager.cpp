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

            VirtualFS::CreateInfo info {
              .mount_point = "cache://",
              .folder = FileSystem::SearchUpTreeRecursiveN("Cache", 5).value(),
            };

            Error ec;
            vfs = std::make_unique<VirtualFS>(info, ec);

            if (ec) {
                Eclipse::LogError("[SHADERMANAGER] [VIRTUAL FILESYSTEM] {}", ec.message);
                return;
            }

      }

      std::optional<Eclipse::ShaderManager::ShaderObject> Eclipse::ShaderManager::compile(const ShaderCreateInfo& shaderInfo) {
            // Read shader file
            const auto shaderFileContent = shaderInfo.file.read();

            const auto cacheFilePath = "cache://" + shaderInfo.name;
            auto cacheFile = vfs->get_file(cacheFilePath);
            if (!cacheFile) {
                Eclipse::LogWarn("Cache File not found! Continuing compilation!");
            } else if (cacheFile->read().empty()) {
                Eclipse::LogWarn("Cache File not found! Continuing compilation!");
            } else if (cacheFile->last_modified_date.get() > shaderInfo.file.last_modified_date.get()) {
                Eclipse::LogInfo("Using Cache file at \"{}\"", cacheFile->path);

                std::span<const u32> code = cacheFile->read_as<u32>();

                ShaderObject obj{};
                obj.spirv_storage.assign(code.begin(), code.end());
                obj.spirv_code = obj.spirv_storage;
                obj.index = shaders.size();

                shaders.push_back(std::move(obj));

                return shaders[shaders.size()-1];
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
                  shaderFileContent.data(),
                  shaderFileContent.size(),
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
            obj.spirv_storage.assign(compilationResult.cbegin(), compilationResult.cend());
            obj.spirv_code = std::span<const u32>(obj.spirv_storage);
            obj.index = shaders.size();

            // Write to cache
            if (cacheFile->write(obj.spirv_code))
                Eclipse::LogInfo("Cache file written to \"{}\"!", cacheFile->path);
            else
                Eclipse::LogError("Unable to write shader cache to \"{}\"", cacheFile->path);

            shaders.push_back(std::move(obj));

            return shaders[shaders.size()-1];
      }
}
