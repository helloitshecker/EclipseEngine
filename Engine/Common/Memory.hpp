#pragma once

#include <Engine/Common/Types.hpp>
#include <expected>
#include <string>

namespace Eclipse::Memory {
    struct Memory {
        char* begin;
        char* end;
        char* current;
    };

    struct CreateInfo {
        u64 size;
    };

    Memory* Create(CreateInfo&);
    Memory* Create(CreateInfo&&);
    void Destroy(Memory*);

    template <typename T>
    std::expected<T*, std::string> Allocate(Memory* mempool, u64 instances) {
        if (sizeof(T)*instances > static_cast<u64>(mempool->end - mempool->current)) {
            // Not enough memory
            return std::unexpected("Failed to allocate memory!");
        } else {
            T* out = reinterpret_cast<T*>(mempool->current);
            mempool->current += sizeof(T)*instances;
            return out;
        }
    }

    template <typename T>
    std::expected<T*, std::string> Allocate(Memory* mempool) {
        return Eclipse::Memory::Allocate<T>(mempool, 1);
    }
}