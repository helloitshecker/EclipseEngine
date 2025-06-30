#pragma once

#include <cstdlib>
#include <Engine/Common/Assert.hpp>

namespace Eclipse {
    namespace Memory {
        namespace Pool {
            struct CreateInfo {
                size_t size;
            };

            struct Pool {
                size_t capacity;
                char* start;
                char* current;
                char* end;
            };

            inline Pool* Create(CreateInfo& create_info) {
                Pool* pool = (Pool*)malloc(sizeof(Pool));
                ECL_ASSERT(pool != nullptr, "Failed to allocate memory!");

                pool->capacity = create_info.size;
                pool->start = (char*)malloc(pool->capacity);
                ECL_ASSERT(pool->start != nullptr, "Failed to allocate memory!");
                pool->current = pool->start;
                pool->end = pool->start + pool->capacity * sizeof(char);

                return pool;
            }

            inline void Destroy(Pool* pool) {
                free(pool->start);
                free(pool);
            }
        }



        template <typename T>
        inline T* Allocate(Pool::Pool* pool, size_t size) {
            ECL_ASSERT(pool->current + sizeof(T) * size <= pool->end, "This allocation can lead to Pool Overflow!");
            T* ret = reinterpret_cast<T*>(pool->current);
            pool->current += sizeof(T) * size;
            return ret;
        }
    }
}
