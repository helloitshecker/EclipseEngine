#pragma once

#include <filesystem>

namespace Eclipse::FileSystem {
      using Path = std::filesystem::path;
      using TextFileContent = std::vector<char>;

      std::optional<Path> ToShaderPath(const Path&);
      std::optional<TextFileContent> ReadTextFile(const Path&);
}