#pragma once

#include <filesystem>
#include <fstream>

#include <Engine/Core/Types.hpp>

#include "Error.hpp"

namespace Eclipse::FileSystem {
      using Path = std::filesystem::path;
      using TextFileContent = std::vector<char>;
      using BinaryFileContent = std::vector<std::byte>;

      template<typename T>
      using CustomFileContent = std::vector<T>;

      std::optional<Path> ToShaderPath(const Path&);
      std::optional<Path> ToCachePath(const Path&);
      std::optional<TextFileContent> ReadTextFile(const Path&);
      std::optional<BinaryFileContent> ReadBinaryFile(const Path&);

      template<typename T>
      std::optional<CustomFileContent<T>> ReadCustomFile(const Path& path) {
            static_assert(std::is_trivial_v<T>, "CustomFileContent<T> must be a trivial type");

            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                  return std::nullopt;
            }

            const std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            if (size < 0) return std::nullopt;

            if (size % sizeof(T) != 0) return std::nullopt;

            CustomFileContent<T> content(size / sizeof(T));
            if (!file.read(reinterpret_cast<char*>(content.data()), size)) return std::nullopt;

            return content;
      }

      bool WriteTextFile(const Path&, const TextFileContent&);
      bool WriteBinaryFile(const Path&, const BinaryFileContent&);

      template<typename T>
      bool WriteCustomFile(const Path& path, const CustomFileContent<T>& content) {
            static_assert(std::is_trivial_v<T>, "CustomFileContent<T> must be a trivial type");

            std::ofstream file(path, std::ios::binary);
            if (!file.is_open()) {
                  return false;
            }

            file.write(reinterpret_cast<const char*>(content.data()), content.size() * sizeof(T));
            return true;
      }

}
