#pragma once

#include <Engine/Core/Error.hpp>
#include <Engine/Core/FileSystem.hpp>
#include <Engine/Core/Types.hpp>

#include <mio/mmap.hpp>

#include <chrono>
#include <span>

namespace Eclipse {
class VirtualFS {
public:
    struct CreateInfo {
        FileSystem::Path mount_point;
        FileSystem::Path folder;
    };

    enum class FileState {
        Unloaded,
        Cached,
        Mapped
    };

    struct Date {
    private:
        fs::file_time_type _t;

    public:
        Date(const fs::file_time_type time) : _t(time) {}

        u64 get() {
            return _t.time_since_epoch().count();
        }
    };

    struct File {
    private:
        FileSystem::Path absolute_path;
        FileState state;

        FileSystem::BinaryFileContent vcontent;
        mio::mmap_source mcontent;

        bool bigfile;
    public:
        File(const FileSystem::Path& full_path) : absolute_path(full_path), state(FileState::Unloaded), bigfile(fs::file_size(full_path) >= 1024*1024*512), last_modified_date(fs::last_write_time(full_path)) {
            path = absolute_path.relative_path();
            name = absolute_path.filename().string();
        }

        std::string name;
        FileSystem::Path path;
        Date last_modified_date;

        u64 size();
        bool write(const FileSystem::BinaryFileContent&);
        bool write(const FileSystem::BinaryFileContent&, const u64 offset);

        template<typename T>
        bool write(const T& data) const {
            static_assert(std::is_trivially_copyable_v<T>, "File::write() only supports trivially copyable types");
            return Eclipse::FileSystem::WriteBinaryFile(absolute_path, std::as_bytes(std::span(&data, 1)));
        }

        template<typename T>
        bool write(const std::vector<T>& data) const {
            static_assert(std::is_trivially_copyable_v<T>, "File::write() only supports trivially copyable types");

            return Eclipse::FileSystem::WriteBinaryFile(absolute_path, std::as_bytes(std::span(data.data(), data.size())));
        }

        template<typename T>
        bool write(const T& value) {
            static_assert(std::is_trivial_v<T>, "T must be a trivial type");
            FileSystem::CustomFileContent<T> content{ value };
            return Eclipse::FileSystem::WriteCustomFile(absolute_path, content);
        }

        template<typename T>
        bool write(std::span<const T> span) {
            static_assert(std::is_trivial_v<T>, "T must be a trivial type");
            FileSystem::CustomFileContent<T> content(span.begin(), span.end());
            return Eclipse::FileSystem::WriteCustomFile(absolute_path, content);
        }

        std::span<const char> read();
        std::span<const char> read(const u64 offset);

        template<typename T>
        std::span<const T> read_as(const u64 offset = 0) {
            auto char_span = read(offset);
            if (char_span.empty()) return {};

            // Ensure not to read past a multiple of sizeof(T)
            const size_t valid_size = (char_span.size() / sizeof(T)) * sizeof(T);

            if (valid_size != char_span.size()) {
                Eclipse::LogWarn("File size not divisible by sizeof(T), truncating!");
            }

            // Alignment check (important only if T > 1-byte)
            if constexpr (alignof(T) > 1) {
                if (reinterpret_cast<uintptr_t>(char_span.data()) % alignof(T) != 0) {
                    Eclipse::LogError("Data not aligned for type T!");
                    return {};
                }
            }

            return std::span<const T>(
                reinterpret_cast<const T*>(char_span.data()),
                valid_size / sizeof(T)
                );
        }

        bool refresh();
    };

    VirtualFS(const CreateInfo& info, Error& error);
    ~VirtualFS() = default;

    std::optional<File> get_file(const FileSystem::Path& path);
    std::shared_ptr<File> get_file_as_owner(const FileSystem::Path& path);
private:
    FileSystem::Path mount_point;
    FileSystem::Path parent_folder;
};
}
