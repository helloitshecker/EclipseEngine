#include "ShaderManager.hpp"

#include <chrono>

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
            const auto shaderFileContent = Eclipse::FileSystem::ReadTextFile(shaderInfo.name);
            if (shaderFileContent == std::nullopt) {
                  Eclipse::LogError("Failed to read shader file \"{}\"", shaderInfo.name);
                  return std::nullopt;
            }

            // Cache Usage
            std::string cache_path_str = shaderInfo.name;
            const size pos = cache_path_str.find("Source");

            // Convert path for da cache path
            cache_path_str.replace(pos, 6, "Cache");
            const std::filesystem::path cache_file = cache_path_str;

            do {
                  // External shader source (always compile)
                  if (pos == std::string::npos) {
                        break;
                  }

                  // Check if cache file exists
                  if (!std::filesystem::exists(cache_file)) {
                        Eclipse::LogWarn("Couldn't find cache file for \"{}\"! Skipping to compilation!", shaderInfo.name);
                        break;
                  }

                  // Check if cache file is up to date
                  if (std::filesystem::last_write_time(cache_file) < std::filesystem::last_write_time(shaderInfo.name)) {
                        Eclipse::LogWarn("Cache file is old for \"{}\"! Skipping to compilation!", shaderInfo.name);
                        break;
                  }

                  // Now all set, use it
                  const auto spirv_file_content = Eclipse::FileSystem::ReadCustomFile<u32>(cache_file);
                  if (spirv_file_content == std::nullopt) {
                        Eclipse::LogError("Failed to load Cache file \"{}\" for unknown reasons! Skipping to compilation!", cache_path_str);
                        break;
                  }
                  {
                        Eclipse::LogInfo("Loading \"{}\" from cache!", shaderInfo.name);

                        ShaderObject obj{};
                        obj.spirv_code = spirv_file_content.value();
                        obj.index = shaders.size();
                        shaders.push_back(obj);

                        return obj;
                  }
            } while (0);

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

            // Write to cache
            Eclipse::FileSystem::WriteCustomFile<u32>(cache_file, obj.spirv_code);

            Eclipse::LogInfo("Cache file written to \"{}\"!", cache_path_str);

            return obj;
      }

      std::optional<Eclipse::ShaderManager::ShaderObject> ShaderManager::compile(const ShaderCreateInfoEXT& shaderInfo) {
          if (shaderInfo.source._src_min.empty()) {
              Eclipse::LogError("Shader source content is empty for \"{}\"!", shaderInfo.name);
              return std::nullopt;
          }

          std::string cache_path_str = shaderInfo.name + ".cache";
          const std::filesystem::path cache_file = cache_path_str;


          if (std::filesystem::exists(cache_file)) {
              const auto last_write_time_cache_file = static_cast<u64>(std::chrono::system_clock::to_time_t(
                  std::chrono::clock_cast<std::chrono::system_clock>(
                      std::filesystem::last_write_time(cache_file)
                      )));
              if (last_write_time_cache_file > shaderInfo.source.last_modified_date) {
                const auto spirv_file_content = Eclipse::FileSystem::ReadCustomFile<u32>(cache_file);
                if (spirv_file_content) {
                    Eclipse::LogInfo("Loading \"{}\" from cache!", shaderInfo.name);
                    ShaderObject obj{};
                    obj.spirv_code = spirv_file_content.value();
                    obj.index = shaders.size();
                    shaders.push_back(obj);
                    return obj;
                } else {
                    Eclipse::LogWarn("Failed to read cache for \"{}\"! Recompiling...", shaderInfo.name);
                }
              }
          }

          // Determine shader kind
          shaderc_shader_kind shaderKind{};
          switch (shaderInfo.type) {
          case ShaderType::VERTEX:   shaderKind = shaderc_glsl_default_vertex_shader; break;
          case ShaderType::FRAGMENT: shaderKind = shaderc_glsl_default_fragment_shader; break;
          case ShaderType::COMPUTE:  shaderKind = shaderc_glsl_default_compute_shader; break;
          default:
              Eclipse::LogError("Shader type not recognized for \"{}\"!", shaderInfo.name);
              return std::nullopt;
          }

          // Compile using BinaryFileContent
          shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(
              shaderInfo.source._src_min.data(),
              shaderInfo.source._src_min.size(),
              shaderKind,
              shaderInfo.name.c_str(),
              shaderInfo.entry.c_str(),
              compileOptions
              );

          if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
              Eclipse::LogError("Shader Compilation Error (\"{}\"): {}", shaderInfo.name, compilationResult.GetErrorMessage());
              return std::nullopt;
          }

          ShaderObject obj{};
          obj.spirv_code = { compilationResult.cbegin(), compilationResult.cend() };
          obj.index = shaders.size();
          shaders.push_back(obj);

          // Write cache
          Eclipse::FileSystem::WriteCustomFile<u32>(cache_file, obj.spirv_code);
          Eclipse::LogInfo("Cache file written to \"{}\"!", cache_path_str);

          return obj;
      }

}
