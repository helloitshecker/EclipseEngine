#include "Memory.hpp"

#include <cstdlib>

Eclipse::Memory::Memory* Eclipse::Memory::Create(CreateInfo& create_info) {
    auto mem = static_cast<Eclipse::Memory::Memory *>(malloc(sizeof(Eclipse::Memory::Memory)));
    mem->begin = static_cast<char *>(malloc(create_info.size));
    mem->end = mem->begin + create_info.size;
    mem->current = mem->begin;
    return mem;
}

Eclipse::Memory::Memory* Eclipse::Memory::Create(CreateInfo&& create_info) {
    Eclipse::Memory::CreateInfo ci = create_info;
    return Eclipse::Memory::Create(ci);
}

void Eclipse::Memory::Destroy(Memory* mem) {
    free(mem->begin);
    free(mem);
}