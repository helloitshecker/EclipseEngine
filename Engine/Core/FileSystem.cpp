#include "FileSystem.hpp"

#include <fstream>

#include <Engine/Core/Error.hpp>

static const std::vector <Eclipse::FileSystem::Path> PossibleShaderPaths = {
      "Shaders/Source",
      "../Shaders/Source",
      "../../Shaders/Source",
      "../../../Shaders/Source",
      "Engine/Shaders/Source",
      "../Engine/Shaders/Source",
      "../../Engine/Shaders/Source",
      "../../../Engine/Shaders/Source",
};

static std::vector<Eclipse::FileSystem::Path> WorkingPaths;

static bool CalculateShaderPathOffset(const Eclipse::FileSystem::Path& path) {
      WorkingPaths.clear();
      for (const auto& PossibleShaderPath : PossibleShaderPaths) {
            const Eclipse::FileSystem::Path checkPath = PossibleShaderPath / path;
            if (std::filesystem::is_regular_file(checkPath)) {
                  WorkingPaths.push_back(PossibleShaderPath);
                  return true;
            }
      }
      return false;
}

namespace Eclipse::FileSystem {
      std::optional<Path> ToShaderPath(const Path& path) {
            if (WorkingPaths.empty()) CalculateShaderPathOffset(path);
            if (WorkingPaths.empty()) return std::nullopt;
            const Path rPath = WorkingPaths[0] / path;
            if (std::filesystem::is_regular_file(rPath)) return rPath;
            else return std::nullopt;
      }


      std::optional<TextFileContent> ReadTextFile(const Path& path) {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file) {
                  Eclipse::LogError("Failed to read file \"{}\"", path.string());
                  return std::nullopt;
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            if (!file.read(buffer.data(), size)) return std::nullopt;

            return buffer;
      }
}