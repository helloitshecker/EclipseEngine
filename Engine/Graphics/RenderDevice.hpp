#pragma once

#include <Engine/Core/Types.hpp>
#include <Engine/Core/Window.hpp>

#include <vector>
#include <optional>

#include <volk.h>
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace Eclipse {
        class RenderDevice {
        public:
                struct CreateInfo {
                        const std::vector<const char *>& extensions;
                        Window& window;
                        bool debug;
                };

                explicit RenderDevice(const CreateInfo&);
                ~RenderDevice();
        private:
                struct QueueFamilyIndices {
                        std::optional<u32> graphics;
                        std::optional<u32> present;
                        std::optional<u32> compute;
                        std::optional<u32> transfer;

                        [[nodiscard]] bool isComplete() const {
                                return graphics.has_value() && present.has_value() && compute.has_value() && transfer.has_value();
                        }
                };

                struct State {
                        bool debug = false;
                        bool dedicated = false;

                        std::vector<const char*> instanceExtensions;
                        std::vector<const char*> instanceLayers;

                        QueueFamilyIndices families;

                        VkInstance instance = VK_NULL_HANDLE;
                        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
                        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
                        VkDevice logicalDevice = VK_NULL_HANDLE;
                        VkSurfaceKHR surface = VK_NULL_HANDLE;
                };

                State state {};

                void CreateInstance();
                void CreateDebugger();
                void CreateDevice();

                static bool CheckInstanceExtensionSupport(const std::vector<const char*>&);
                static bool CheckInstanceLayerSupport(const std::vector<const char*>&);
                VkPhysicalDevice SelectPhysicalDevice(const std::vector<VkPhysicalDevice>&);
        };
}