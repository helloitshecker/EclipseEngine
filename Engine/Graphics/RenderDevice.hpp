#pragma once

#include <Engine/Core/Types.hpp>
#include <Engine/Core/Window.hpp>
#include <Engine/Graphics/ShaderManager.hpp>

#include <vector>
#include <array>
#include <optional>

#include <volk.h>
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

constexpr u64 MAX_FRAMES_IN_FLIGHT = 2;

namespace Eclipse {
        class RenderDevice {
        public:
                struct CreateInfo {
                        const std::vector<const char *> extensions;
                        Window& window;
                        bool debug;
                };

                explicit RenderDevice(const CreateInfo&);
                ~RenderDevice();

                void DrawFrame(void);
        private:
                struct QueueFamilyIndices {
                        std::optional<u32> graphics;
                        std::optional<u32> present;
                        std::optional<u32> compute;
                        std::optional<u32> transfer;

                        bool isComplete() const {
                                return graphics.has_value() && present.has_value() && compute.has_value() && transfer.has_value();
                        }
                };

                struct SwapchainSupportDetails {
                        VkSurfaceCapabilitiesKHR capabilities;
                        std::vector<VkSurfaceFormatKHR> formats;
                        std::vector<VkPresentModeKHR> presentModes;
                };

                struct State {
                        bool debug = false;
                        bool dedicated = false;

                        i32 windowWidth;
                        i32 windowHeight;

                        std::vector<const char*> instanceExtensions;
                        std::vector<const char*> instanceLayers;
                        std::vector<const char*> deviceExtensions;

                        std::vector<VkImage> swapchainImages;
                        std::vector<VkImageView> swapchainImageViews;

                        std::vector<VkCommandBuffer> commandBuffers;

                        std::vector<VkSemaphore> imageAvailableSemaphores;
                        std::vector<VkSemaphore> renderFinishedSemaphores;
                        std::vector<VkFence> inFlightFences;
                        u32 currentFrame = 0;
                        bool framebufferResized = false;

                        QueueFamilyIndices families;

                        VkInstance instance = VK_NULL_HANDLE;
                        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
                        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
                        VkDevice logicalDevice = VK_NULL_HANDLE;
                        VkSurfaceKHR surface = VK_NULL_HANDLE;
                        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
                        VkFormat swapchainFormat = VK_FORMAT_UNDEFINED;
                        VkExtent2D swapchainExtent = {};
                        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
                        VkPipeline graphicsPipeline = VK_NULL_HANDLE;
                        VkCommandPool commandPool = VK_NULL_HANDLE;

                        VkQueue graphicsQueue;
                        VkQueue computeQueue;
                        VkQueue presentQueue;
                        VkQueue transferQueue;

                        std::unique_ptr<ShaderManager> shaderManager;
                };

                State state {};

                void CreateInstance();
                void CreateDebugger();
                void CreateDevice();
                void CreateSwapchain();
                void CreateImageViews();
                void CreateGraphicsPipeline();
                void CreateCommandPool();
                void CreateCommandBuffers();
                void CreateSyncObjects();

                void RecordCommandBuffer(VkCommandBuffer, u32) const;

                static bool CheckInstanceExtensionSupport(const std::vector<const char*>&);
                static bool CheckInstanceLayerSupport(const std::vector<const char*>&);
                [[nodiscard]] VkPhysicalDevice SelectPhysicalDevice(const std::vector<VkPhysicalDevice>&) const;
                static SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice, VkSurfaceKHR);
                [[nodiscard]] std::optional<VkShaderModule> CreateShaderModule(const Eclipse::FileSystem::CustomFileContent<u32>&) const;
        };
}