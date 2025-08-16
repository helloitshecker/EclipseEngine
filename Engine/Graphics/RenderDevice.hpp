#pragma once

#include <Engine/Core/Types.hpp>

#include <vector>

#include <volk.h>
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace Eclipse {
        class RenderDevice {
        public:
                struct CreateInfo {
                        const std::vector<const char *>& extensions;
                };

                RenderDevice(const CreateInfo&);
                ~RenderDevice();
        private:
                struct State {
                        std::vector<const char*> extensions;

                        VkInstance instance = VK_NULL_HANDLE;
                };

                State state {};

                void CreateInstance(const std::vector<const char*>&);

                static bool CheckExtensionSupport(const std::vector<const char*>&);
        };
}