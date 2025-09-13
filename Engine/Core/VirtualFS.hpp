#pragma once

#include <mio/mmap.hpp>

#include <Engine/Core/Types.hpp>
#include <Engine/Core/FileSystem.hpp>

#include <vector>
#include <string>

constexpr u32 EPAK_MAGIC_NUMBER = 0x07200323; // My birthday ;) (23-03-2007) (made like this for hex editor)
constexpr u32 EPAK_CURRENT_VERSION = 1;

namespace Eclipse {
    class VirtualFS {
    public:
        // Metadata Structs
        struct EPAK_Header {
            u32 magic;
            u32 version;
            u64 table_offset;
            u64 table_size;
        };

        struct EPAK_Table_Entry {
            char filename[64];
            char path[64];
        };

        using EPAK_Table = std::vector<EPAK_Table_Entry>;

        // Funs

        struct MountInfo {
            Eclipse::FileSystem::Path epak_path;
            bool bundled;
        };

        std::optional<EPAK_Table> mount(const MountInfo& info);
        ~VirtualFS() = default;

        // Funcs


    private:
    };
}
