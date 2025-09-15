#include "VirtualFS.hpp"
#include <cstring>
#include <chrono>
#include <ctime>
#include <filesystem>

Eclipse::VirtualFS::MountedDevice gDevice;

static std::optional<Eclipse::VirtualFS::EPAK_Table_Entry> DoesThisFileExistInTheMountedDevice(const Eclipse::FileSystem::Path& file_name) {
    for (const auto& file : gDevice.table) {
        if (file.filename == file_name.string()) return file;
    }
    return std::nullopt;
}

namespace Eclipse {
    VirtualFS::VirtualFS(const FileSystem::Path& path, const FileSystem::Path& mount_alias) {
        const auto res = FileSystem::ReadBinaryFile(path);
        if (!res) {
            Eclipse::LogError("Failed to load resource file!");
            abort();
        }

        EPAK_Header header = *reinterpret_cast<const EPAK_Header*>(res->data());

        if (header.magic != EPAK_MAGIC_NUMBER) {
            Eclipse::LogError("Resource file header68849 is corrupted!");
            abort();
        }

        if (header.version > EPAK_CURRENT_VERSION) {
            Eclipse::LogError("Resource file version not supported!");
            abort();
        }

        gDevice.path = path.parent_path().string();
        gDevice.mount_alias = mount_alias.string();
        gDevice.version = header.version;
        gDevice.header = header;

        gDevice.table.resize(header.table_size);

        char* pT = (char*)res->data() + sizeof(EPAK_Header);
        for (u64 i = 0; i < header.table_size; i++) {
            gDevice.table[i] = *reinterpret_cast<const EPAK_Table_Entry*>(pT);
            pT += sizeof(EPAK_Table_Entry);
        }

        for (const auto& d : gDevice.table) {
            char buffer[64];
            std::tm local_tm;
            const long tim = d.last_modified_date;
            localtime_r(&tim, &local_tm);
            std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", &local_tm);
            Eclipse::LogInfo("File Name: {}\n\tFile Path: {}\n\tTime Of Creation: {}\n\tFile Size: {}", d.filename, d.fspath, buffer, d.filesize);
        }
    }

    VirtualFS::~VirtualFS() {

        Eclipse::LogInfo("Updating Resource File!");

        FileSystem::BinaryFileContent res(sizeof(EPAK_Header) + gDevice.table.size() * sizeof(EPAK_Table_Entry));
        char* wp = res.data();

        std::memcpy(wp, &gDevice.header, sizeof(EPAK_Header));
        std::memcpy(wp + sizeof(EPAK_Header), gDevice.table.data(), gDevice.table.size() * sizeof(EPAK_Table_Entry));

        FileSystem::WriteBinaryFile(gDevice.path, res);
    }

    std::vector<std::string> VirtualFS::get_files() {
        std::vector<std::string> files;
        files.reserve(gDevice.table.size());

        for (const auto& e : gDevice.table)
            files.push_back(e.filename);

        return files;
    }

    std::optional<VirtualFS::EPAK_File> VirtualFS::read_file(const FileSystem::Path& path) {
        if (path.empty()) return std::nullopt;

        FileSystem::Path relative_path = path.lexically_relative(gDevice.mount_alias); // erases res://

        const auto res = DoesThisFileExistInTheMountedDevice(relative_path);
        if (!res) {
            Eclipse::LogError("Path \"{}\" doesn't exist!", path.string());
            return std::nullopt;
        }

        relative_path = gDevice.path / relative_path;
        relative_path.replace_extension(relative_path.extension().string() + ".ebin");

        EPAK_File file{};
        file.filename = res->filename;
        file.last_modified_date = res->last_modified_date;

        u64 file_size = std::filesystem::file_size(relative_path);

        if (file_size > 1024 * 1024) {
            // std::error_code err;
            // file._src.map(relative_path.string(), err);
            // if (err) {
            //     Eclipse::LogError("Failed to mmap file: {}", res->filename);
            //     return std::nullopt;
            // }
            // file.data = (char*)file._src.data();
            // file.size = file._src.size();
        } else {
            auto con = FileSystem::ReadBinaryFile(relative_path);
            if (!con) {
                Eclipse::LogError("Failed to load file: {}", res->filename);
                return std::nullopt;
            }
            file._src_min = *con;
            file.data = (char*)file._src_min.data();
            file.size = file._src_min.size();
        }

        return file;
    }

    bool VirtualFS::write_file(const FileSystem::Path& path, const FileSystem::BinaryFileContent& content) {
        if (path.empty()) return false;

        FileSystem::Path relative_path = path.lexically_relative(gDevice.mount_alias); // remove res://

        // find file
        const auto res = DoesThisFileExistInTheMountedDevice(relative_path);
        if (!res) {
            Eclipse::LogError("Path \"{}\" doesn't exist!", path.string());
            return false;
        }

        // Make path system for
        relative_path = gDevice.path / relative_path;
        relative_path.replace_extension(relative_path.extension().string() + ".ebin");

        // Write
        if (!FileSystem::WriteBinaryFile(relative_path, content)) {
            Eclipse::LogError("Failed to write to \"{}\"", path.string());
            return false;
        }

        // Update header
        const FileSystem::Path search_path = path.lexically_relative(gDevice.mount_alias);
        for (auto& file : gDevice.table) {
            if (file.filename == search_path) {
                file.filesize = content.size();
                file.last_modified_date = (u64)std::time(nullptr);
                break;
            }
        }

        return true;
    }

    void VirtualFS::GenerateResourceFile() {
        const auto res_file_path = FileSystem::SearchUpTreeRecursiveFileN("Resources/Resources.epak", 5);
        const FileSystem::Path res_folder = res_file_path.value().parent_path();

        EPAK_Table table{};

        for (const auto& entry : std::filesystem::directory_iterator(res_folder)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".ebin") continue;

            // Get Time
            const auto ftime = static_cast<u64>(std::chrono::system_clock::to_time_t(
                        std::chrono::clock_cast<std::chrono::system_clock>(
                            std::filesystem::last_write_time(res_folder / entry.path().filename())
                    )));

            // Get Filename and remove .ebin
            std::string filename = entry.path().filename().string();
            filename = filename.erase(filename.find(".ebin"), 5);

            // Get Filename with .ebin
            const std::string fspath = entry.path().filename().string();

            // Entry to push to table
            EPAK_Table_Entry cell{};
            cell.last_modified_date = ftime;
            std::strncpy(cell.filename, filename.c_str(), sizeof(cell.filename) - 1);
            cell.filename[sizeof(cell.filename)-1] = '\0';
            std::strncpy(cell.fspath, fspath.c_str(), sizeof(cell.fspath) - 1);
            cell.fspath[sizeof(cell.fspath)-1] = '\0';

            cell.filesize = std::filesystem::file_size(res_folder / entry.path().filename());

            table.push_back(cell);
        }

        EPAK_Header header{};
        header.magic = EPAK_MAGIC_NUMBER;
        header.version = EPAK_CURRENT_VERSION;
        header.table_size = table.size();

        FileSystem::BinaryFileContent write_con(sizeof(EPAK_Header) + table.size() * sizeof(EPAK_Table_Entry));
        char* wp = write_con.data();

        std::memcpy(wp, &header, sizeof(EPAK_Header));
        std::memcpy(wp + sizeof(EPAK_Header), table.data(), table.size() * sizeof(EPAK_Table_Entry));

        FileSystem::WriteBinaryFile(res_file_path.value(), write_con);
    }

    bool VirtualFS::create_file(const FileSystem::Path& mount_path, const FileSystem::BinaryFileContent& initial_content = {}) {
        if (mount_path.empty()) return false;

        // Get relative path inside the mount
        FileSystem::Path relative_path = mount_path.lexically_relative(gDevice.mount_alias);

        // Check if file already exists
        if (DoesThisFileExistInTheMountedDevice(relative_path)) {
            Eclipse::LogError("File \"{}\" already exists!", mount_path.string());
            return false;
        }

        // Compute real path on disk
        FileSystem::Path real_path = gDevice.path / relative_path;
        real_path.replace_extension(real_path.extension().string() + ".ebin");

        // Write initial content (can be empty)
        if (!FileSystem::WriteBinaryFile(real_path, initial_content)) {
            Eclipse::LogError("Failed to create file \"{}\"", mount_path.string());
            return false;
        }

        // Add entry to the table
        EPAK_Table_Entry entry{};
        entry.filesize = initial_content.size();
        entry.last_modified_date = static_cast<u64>(std::time(nullptr));

        std::string filename_str = mount_path.filename().string();
        std::strncpy(entry.filename, filename_str.c_str(), sizeof(entry.filename)-1);
        entry.filename[sizeof(entry.filename)-1] = '\0';

        std::string fspath_str = real_path.filename().string();
        std::strncpy(entry.fspath, fspath_str.c_str(), sizeof(entry.fspath)-1);
        entry.fspath[sizeof(entry.fspath)-1] = '\0';

        gDevice.table.push_back(entry);

        // Update the .epak file
        gDevice.header.table_size = gDevice.table.size();

        FileSystem::BinaryFileContent epak_data(sizeof(EPAK_Header) + gDevice.table.size() * sizeof(EPAK_Table_Entry));
        char* wp = epak_data.data();
        std::memcpy(wp, &gDevice.header, sizeof(EPAK_Header));
        std::memcpy(wp + sizeof(EPAK_Header), gDevice.table.data(), gDevice.table.size() * sizeof(EPAK_Table_Entry));

        if (!FileSystem::WriteBinaryFile(std::filesystem::path(gDevice.path) / "Resources.epak", epak_data)) {
            Eclipse::LogError("Failed to update resource file!");
            return false;
        }

        return true;
    }

}
