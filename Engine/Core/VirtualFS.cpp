#include "VirtualFS.hpp"

namespace fs = std::filesystem;

namespace Eclipse {
    VirtualFS::VirtualFS(const CreateInfo& info, Error& error) : mount_point(info.mount_point), parent_folder(fs::absolute(info.folder)) {

        std::error_code ec;
        if (!fs::exists(parent_folder, ec) || ec) {
            error = Error(std::format("[FILESYSTEM] Invalid Path \"{}\"", parent_folder), ErrorType::Filesystem_InvalidPath);
            return;
        }

    }

    std::optional<VirtualFS::File> VirtualFS::get_file(const FileSystem::Path& path) {
        const auto lex = path.lexically_relative(mount_point);
        const auto real_path = parent_folder / lex;

        std::error_code ec;

        // If file does not exist, create it
        if (!fs::exists(real_path, ec)) {
            if (ec) {
                Eclipse::LogError("Failed to check file existence for \"{}\". Error: {}", path, ec.message());
                return std::nullopt;
            }

            // Create parent directories if needed
            fs::create_directories(real_path.parent_path(), ec);
            if (ec) {
                Eclipse::LogError("Failed to create parent directories for \"{}\". Error: {}", path, ec.message());
                return std::nullopt;
            }

            // Create empty file
            std::ofstream ofs(real_path);
            if (!ofs) {
                Eclipse::LogError("Failed to create file \"{}\".", path);
                return std::nullopt;
            }
            ofs.close();

            Eclipse::LogInfo("Created missing file: {}", path);
        }

        if (!fs::is_regular_file(real_path, ec)) {
            Eclipse::LogError("Path \"{}\" is not a regular file! Error: {}", path, ec.message());
            return std::nullopt;
        }

        return File(real_path);
    }


    std::shared_ptr<VirtualFS::File> VirtualFS::get_file_as_owner(const FileSystem::Path& path) {
        const auto lex = path.lexically_relative(mount_point);
        const auto real_path = parent_folder / lex;

        std::error_code ec;

        // If file does not exist, create it
        if (!fs::exists(real_path, ec)) {
            if (ec) {
                Eclipse::LogError("Failed to check file existence for \"{}\". Error: {}", path, ec.message());
                return nullptr;
            }

            // Create parent directories if needed
            fs::create_directories(real_path.parent_path(), ec);
            if (ec) {
                Eclipse::LogError("Failed to create parent directories for \"{}\". Error: {}", path, ec.message());
                return nullptr;
            }

            // Create empty file
            std::ofstream ofs(real_path);
            if (!ofs) {
                Eclipse::LogError("Failed to create file \"{}\".", path);
                return nullptr;
            }
            ofs.close();

            Eclipse::LogInfo("Created missing file: {}", path);
        }

        if (!fs::is_regular_file(real_path, ec)) {
            Eclipse::LogError("Path \"{}\" is not a regular file! Error: {}", path, ec.message());
            return nullptr;
        }

        std::shared_ptr<File> file = std::make_shared<File>(real_path);

        return file;
    }

    u64 VirtualFS::File::size() {
        return fs::file_size(absolute_path);
    }

    bool VirtualFS::File::write(const FileSystem::BinaryFileContent& bin) {
        return Eclipse::FileSystem::WriteBinaryFile(absolute_path, bin);
    }

    bool VirtualFS::File::write(const FileSystem::BinaryFileContent& bin, const u64 offset) {
        return Eclipse::FileSystem::WriteBinaryFile(absolute_path, bin, offset);
    }

    std::span<const char> VirtualFS::File::read() {
        if (bigfile && state == FileState::Mapped) {
            return { mcontent.data(), mcontent.size() };
        } else if (bigfile && state == FileState::Unloaded) {
            std::error_code ec;
            mcontent.map(absolute_path.string(), ec);
            if (ec) return {};
            state = FileState::Mapped;
            return {mcontent.data(), mcontent.size()};
        } else if (state == FileState::Cached) {
            return {vcontent.data(), vcontent.size()};
        } else if (state == FileState::Unloaded) {
            auto r = FileSystem::ReadBinaryFile(absolute_path);
            if (r == std::nullopt) return {};
            vcontent = std::move(r.value());
            state = FileState::Cached;
            return {vcontent.data(), vcontent.size()};
        } else {
            return {};
        }
    }

    std::span<const char> VirtualFS::File::read(const u64 offset) {
        if (bigfile && state == FileState::Mapped) {
            return { mcontent.data() + offset , mcontent.size() - offset };
        } else if (bigfile && state == FileState::Unloaded) {
            std::error_code ec;
            mcontent.map(absolute_path.string(), ec);
            if (ec) return {};
            state = FileState::Mapped;
            return {mcontent.data() + offset, mcontent.size() - offset};
        } else if (state == FileState::Cached) {
            return {vcontent.data() + offset, vcontent.size() - offset};
        } else if (state == FileState::Unloaded) {
            auto r = FileSystem::ReadBinaryFile(absolute_path);
            if (r == std::nullopt) return {};
            vcontent = std::move(r.value());
            state = FileState::Cached;
            return {vcontent.data() + offset, vcontent.size() - offset};
        } else {
            return {};
        }
    }

    bool VirtualFS::File::refresh() {
        if (state == FileState::Mapped) {
            return true;
        } else {
            state = FileState::Unloaded;
            return true;
        }
    }
}

