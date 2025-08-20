#include "ShaderManager.hpp"

#include <toml.hpp>

#include <Engine/Core/Error.hpp>
#include <Engine/Core/Types.hpp>

const std::filesystem::path path = "../Engine/Shaders";

Eclipse::ShaderManager::ShaderManager() {
        try {
                if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
                        std::filesystem::create_directory(path);
                        EERROR("No directory such as {}, so created a new one.", path.string());
                }

                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                        if (std::filesystem::is_regular_file(entry)) {
                                shaderPaths.push_back(entry.path());
                        }
                }
        } catch (const std::exception& e) {
                EERROR("Failed to load shader path!");
                abort();
        }
}

void Eclipse::ShaderManager::compile_all() {
        const std::filesystem::path compiled_path = "../Engine/Shaders/Compiled";
        if (!std::filesystem::exists(compiled_path)) {
                std::filesystem::create_directory(compiled_path);
        }

        auto fixPath = [](const std::filesystem::path& p) {
                std::string s = p.string();
                std::replace(s.begin(), s.end(), '\\', '/');
                return s;
        };

        auto compileToSPV = [fixPath](const std::filesystem::path& hlsl_path, const std::filesystem::path& spv_path) {
                char shader_mode_char = 'v';
                if (hlsl_path.stem().extension().string() == ".frag") shader_mode_char = 'p';
                const std::string compileCmd = std::format("dxc -spirv -fspv-target-env=vulkan1.3 -E main -T {}s_6_0 -Fo {} {}", shader_mode_char, fixPath(spv_path), fixPath(hlsl_path));
                if (const i32 returnForCompilation = std::system(compileCmd.c_str()); returnForCompilation != 0) return false;
                return true;
        };

        i64 i = 0;
        for (const auto& entry : shaderPaths) {
                std::filesystem::path compiled_file_path;
                if (entry.stem().extension().string() == ".frag")
                        compiled_file_path = compiled_path / entry.filename().stem().replace_extension(".f.spv");
                else
                        compiled_file_path = compiled_path / entry.filename().stem().replace_extension(".v.spv");

                if (std::filesystem::exists(compiled_file_path)) {
                        if (std::filesystem::last_write_time(compiled_path) < std::filesystem::last_write_time(entry)) {
                                // Update is required
                                if (!compileToSPV(entry, compiled_file_path)) {
                                        EERROR("Failed to compile shader \"{}\"!", entry.string());
                                        abort();
                                } else {
                                        EINFO("Compiled Shader: [{}/{}] \"{}\"", i + 1, shaderPaths.size(), entry.string());
                                }
                        } else {
                                EINFO("Using Precompiled Shader [{}/{}] for \"{}\"", i + 1, shaderPaths.size(), entry.string());
                        }
                } else {
                        if (!compileToSPV(entry, compiled_file_path)) {
                                EERROR("Failed to compile shader \"{}\"!", entry.string());
                                abort();
                        } else {
                                EINFO("Compiled Shader: [{}/{}] \"{}\"", i + 1, shaderPaths.size(), entry.string());
                        }
                }

                i++;
        }
}
