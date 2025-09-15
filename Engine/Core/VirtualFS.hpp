#pragma once

#include <mio/mmap.hpp>

#include <Engine/Core/Types.hpp>
#include <Engine/Core/FileSystem.hpp>

#include <vector>
#include <string>

constexpr u32 EPAK_MAGIC_NUMBER = 0x07200323; // My birthday ;) (23-03-2007) (made like this for hex editor)
constexpr u32 EPAK_CURRENT_VERSION = 1;
constexpr u32 EPAK_MAX_ENTRIES = 2046;
constexpr u32 EBIN_MAGIC_NUMBER = 0x13201023; // My Sister's Birthday
constexpr u32 EBIN_CURRENT_VERSION = 1;

namespace Eclipse {
    class VirtualFS {
    public:

        using EPAK_File_Date = u64;

        struct EPAK_Header {
            u32 magic;
            u32 version;
            u64 table_size;
        };

        struct EPAK_Table_Entry {
            char filename[64];
            char fspath[128];
            EPAK_File_Date last_modified_date;
            u64 filesize;
        };

        using EPAK_Table = std::vector<EPAK_Table_Entry>;

        struct EPAK_File {
            std::string filename;
            EPAK_File_Date last_modified_date;
            char* data;
            u64 size;
            // mio::mmap_source _src;
            FileSystem::BinaryFileContent _src_min;
        };

        VirtualFS(const FileSystem::Path& path, const FileSystem::Path& mount_alias);
        ~VirtualFS();

        std::vector<std::string> get_files();
        std::optional<EPAK_File> read_file(const FileSystem::Path& path);
        bool write_file(const FileSystem::Path& path, const FileSystem::BinaryFileContent& content);
        bool create_file(const FileSystem::Path& mount_path, const FileSystem::BinaryFileContent& initial_content);

        static void GenerateResourceFile();

        // Funcs

        struct MountedDevice {
            std::string path;
            std::string mount_alias;
            u32 version;

            EPAK_Header header;
            EPAK_Table table;
        };
    };
}
