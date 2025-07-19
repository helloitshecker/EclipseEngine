#include <Engine/Common/Logger.hpp>
#include <Engine/Common/Types.hpp>
#include <Engine/Common/Memory.hpp>

// Enable for alignment
// #define ECL_ALIGNALL

struct test {
    std::string a;
    u64 x;
    u32 y;
    u32 z[10000];
};

int main() {
    auto main_memory_pool = Eclipse::Memory::Create({4096});

    Eclipse::Memory::Destroy(main_memory_pool);
}