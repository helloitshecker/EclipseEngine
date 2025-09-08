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

static std::vector<Eclipse::FileSystem::Path> WorkingShaderPaths;

static const std::vector <Eclipse::FileSystem::Path> PossibleCachePaths = {
      "Shaders/Cache",
      "../Shaders/Cache",
      "../../Shaders/Cache",
      "../../../Shaders/Cache",
      "Engine/Shaders/Cache",
      "../Engine/Shaders/Cache",
      "../../Engine/Shaders/Cache",
      "../../../Engine/Shaders/Cache",
};

static std::vector<Eclipse::FileSystem::Path> WorkingCachePaths;

static bool CalculateShaderPathOffset(const Eclipse::FileSystem::Path& path) {
      WorkingShaderPaths.clear();
      for (const auto& PossibleShaderPath : PossibleShaderPaths) {
            const Eclipse::FileSystem::Path checkPath = PossibleShaderPath / path;
            if (std::filesystem::is_regular_file(checkPath)) {
                  WorkingShaderPaths.push_back(PossibleShaderPath);
                  return true;
            }
      }
      return false;
}

static bool CalculateCachePathOffest(const Eclipse::FileSystem::Path& path) {
      WorkingCachePaths.clear();
      for (const auto& PossibleCachePath : PossibleCachePaths) {
            const Eclipse::FileSystem::Path checkPath = PossibleCachePath / path;
            if (std::filesystem::is_regular_file(checkPath)) {
                  WorkingShaderPaths.push_back(PossibleCachePath);
                  return true;
            }
      }
      return false;
}

namespace Eclipse::FileSystem {
      std::optional<Path> ToShaderPath(const Path& path) {
            if (WorkingShaderPaths.empty()) CalculateShaderPathOffset(path);
            if (WorkingShaderPaths.empty()) return std::nullopt;
            const Path rPath = WorkingShaderPaths[0] / path;
            if (std::filesystem::is_regular_file(rPath)) return rPath;
            else return std::nullopt;
      }

      std::optional<Path> ToCachePath(const Path& path) {
            if (WorkingCachePaths.empty()) CalculateCachePathOffest(path);
            if (WorkingCachePaths.empty()) return std::nullopt;
            const Path rPath = WorkingCachePaths[0] / path;
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

      std::optional<BinaryFileContent> ReadBinaryFile(const Path& path) {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file) {
                  return std::nullopt;
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            if (size <= 0) return std::nullopt;

            BinaryFileContent buffer(static_cast<::size>(size));
            if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
                  return std::nullopt;

            return buffer;
      }

      bool WriteTextFile(const Path& path, const TextFileContent& content) {
            std::ofstream file(path, std::ios::binary | std::ios::ate);
            if (!file) {
                  return false;
            }

            file.write(content.data(), content.size());
            return true;
      }

      bool WriteBinaryFile(const Path& path, const BinaryFileContent& content) {
            std::ofstream file(path, std::ios::binary);
            if (!file) {
                  return false;
            }
            file.write(reinterpret_cast<const char*>(content.data()), content.size());
            return true;
      }


}