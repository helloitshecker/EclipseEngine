#include "VirtualFS.hpp"
#include <string.h>
namespace Eclipse {
    std::optional<VirtualFS::EPAK_Table> VirtualFS::mount(const MountInfo& info) {
        const auto file = FileSystem::ReadBinaryFile(info.epak_path);
        // FileSystem::BinaryFileContent fc(file.value());
        // EPAK_Table_Entry e {
        //     .filename = "Nigga.txt",
        //     .path = "./Nigga.txt",
        // };
        // char* x = (char*)&e;
        // for (u32 i = 0; i < sizeof(e); i++) {
        //     fc.push_back((std::byte)(*x));
        //     x++;
        // }
        // FileSystem::WriteBinaryFile(info.epak_path, fc);
        // abort();

        if (file == std::nullopt) {
            Eclipse::LogWarn("Failed to load {}! Creating new!", info.epak_path.string());

            // Create new file with minimal content
            constexpr EPAK_Header header {
                .magic = EPAK_MAGIC_NUMBER,
                .version = EPAK_CURRENT_VERSION,
                .table_offset = sizeof(EPAK_Header),
                .table_size = 0
            };

            using header_type_1 = FileSystem::CustomFileContent<EPAK_Header>;
            header_type_1 header_input_1(sizeof(header), header);

            FileSystem::WriteCustomFile(info.epak_path, header_input_1);
        }

        // File Exists
        // Check header

        if (file->size() < sizeof(EPAK_Header)) {
            Eclipse::LogError("Resource File is empty!");
            return std::nullopt;
        }

        const EPAK_Header header = *reinterpret_cast<const EPAK_Header*>(file.value().data());

        if (header.magic != EPAK_MAGIC_NUMBER) {
            Eclipse::LogError("Resource File Header is corrupted!");
            return std::nullopt;
        }

        Eclipse::LogInfo("Resource File Version: {}", header.version);
        Eclipse::LogInfo("Resource File Table Size: {}", header.table_size);

        EPAK_Table table{header.table_size};

        u64 entry_offset = header.table_offset;
        for (auto& entry : table) {
            entry = *reinterpret_cast<const EPAK_Table_Entry*>(file.value().data() + entry_offset);

            entry_offset += sizeof(EPAK_Table_Entry);
        }

        return table;
    }
}
