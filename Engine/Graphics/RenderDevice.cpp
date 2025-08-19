#include "RenderDevice.hpp"

#include <Engine/Core/Error.hpp>

#include <SDL3/SDL.h>

#include "SDL3/SDL_vulkan.h"

#define crash() abort();

// GLOBALS

// STATIC LOCAL FUNCTIONS

static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                        std::println("[INFO ][VALIDATION] {}", pCallbackData->pMessage);
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        std::println("[WARN ][VALIDATION] {}", pCallbackData->pMessage);
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        std::println(stderr, "[ERROR][VALIDATION] {}", pCallbackData->pMessage);
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                        std::println(stderr, "[FATAL][VALIDATION] {}", pCallbackData->pMessage);
                        break;
        }

        return false;
}

// STATIC FUNCTIONS (FOR HELP)

static std::string VAR_FOR_CheckInstanceExtensionSupport_ExtensionNotFoundName;
bool Eclipse::RenderDevice::CheckInstanceExtensionSupport(const std::vector<const char*>& provided_extensions) {
        u32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (auto& extension : provided_extensions) {
                bool found = false;
                for (size_t i = 0; i < extensions.size(); i++) {
                        if (strcmp(extensions[i].extensionName, extension) == 0) {
                                found = true;
                                break;
                        }
                }
                if (!found) {
                        VAR_FOR_CheckInstanceExtensionSupport_ExtensionNotFoundName.append(extension);
                        return false;
                }
        }

        return true;
}

static std::string VAR_FOR_CheckInstanceLayerSupport_ExtensionNotFoundName;
bool Eclipse::RenderDevice::CheckInstanceLayerSupport(const std::vector<const char *>& provided_layers) {
        u32 layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (auto layer : provided_layers) {
                bool found = false;
                for (size_t i = 0; i < availableLayers.size(); i++) {
                        if (strcmp(availableLayers[i].layerName, layer) == 0) {
                                found = true;
                                break;
                        }
                }

                if (!found) {
                        VAR_FOR_CheckInstanceLayerSupport_ExtensionNotFoundName.append(layer);
                        return false;
                }
        }

        return true;
}

VkPhysicalDevice Eclipse::RenderDevice::SelectPhysicalDevice(const std::vector<VkPhysicalDevice>& devices) {
  i64 bestScore = -1;
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

    for (auto& device : devices) {
        i64 score = 0;

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            const auto& qf = queueFamilies[i];
            if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphics = i;
            if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT)  indices.compute  = i;
            if (qf.queueFlags & VK_QUEUE_TRANSFER_BIT) indices.transfer = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, state.surface, &presentSupport);
            if (presentSupport) indices.present = i;
        }

        if (!indices.isComplete()) continue;

        VkPhysicalDeviceVulkan13Features features13{};
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

        VkPhysicalDeviceFeatures2 features2{};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &features13;

        vkGetPhysicalDeviceFeatures2(device, &features2);

        if (features13.dynamicRendering != VK_TRUE || features13.synchronization2 != VK_TRUE) {
            continue;
        }

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

        score += static_cast<i64>(properties.limits.maxSamplerAnisotropy);
        score += static_cast<i64>(properties.limits.maxImageDimension2D);
        score += static_cast<i64>(properties.limits.maxUniformBufferRange);
        score += static_cast<i64>(properties.limits.maxStorageBufferRange);
        score += static_cast<i64>(properties.limits.maxPushConstantsSize);
        score += static_cast<i64>(properties.limits.maxDescriptorSetUniformBuffers);
        score += static_cast<i64>(properties.limits.maxFramebufferWidth + properties.limits.maxFramebufferHeight);

        // Memory check (any device-local heap)
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if (memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                score += 1000;
                break;
            }
        }

        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
        }
    }

    return bestDevice;
}

// INTERNAL VULKAN WRAPPER FUNCTIONS

void Eclipse::RenderDevice::CreateInstance() {
        if (volkInitialize()) {
                EERROR("Failed to initialize vulkan!\n");
                crash();
        }

        const VkApplicationInfo appInfo {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName = "Eclipse",
                .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
                .pEngineName = "Eclipse",
                .engineVersion = VK_MAKE_VERSION(0, 0, 1),
                .apiVersion = volkGetInstanceVersion(),
        };

        const VkInstanceCreateInfo info {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &appInfo,
                .enabledExtensionCount = static_cast<u32>(state.instanceExtensions.size()),
                .ppEnabledExtensionNames = state.instanceExtensions.data(),
                .enabledLayerCount = static_cast<u32>(state.instanceLayers.size()),
                .ppEnabledLayerNames = state.instanceLayers.data(),
        };

        if (!CheckInstanceExtensionSupport(state.instanceExtensions)) {
                EERROR("Extension \"{}\" not supported!", VAR_FOR_CheckInstanceExtensionSupport_ExtensionNotFoundName);
                crash();
        }

        if (!CheckInstanceLayerSupport(state.instanceLayers)) {
                EERROR("Layer \"{}\" not supported!", VAR_FOR_CheckInstanceLayerSupport_ExtensionNotFoundName);
                crash();
        }

        if (vkCreateInstance(&info, nullptr, &state.instance) != VK_SUCCESS) {
                EERROR("Failed to create instance!");
                crash();
        }

        volkLoadInstance(state.instance);
}

void Eclipse::RenderDevice::CreateDebugger() {
        constexpr VkDebugUtilsMessengerCreateInfoEXT info {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = _debug_callback,
                .pUserData = nullptr,
        };

        if (vkCreateDebugUtilsMessengerEXT(state.instance, &info, nullptr, &state.debugMessenger) != VK_SUCCESS) {
                EERROR("Failed to initialize Debugger for Vulkan!");
                crash();
        }
}

void Eclipse::RenderDevice::CreateDevice() {
        u32 physicalDeviceCount{};
        vkEnumeratePhysicalDevices(state.instance, &physicalDeviceCount, nullptr);

        if (physicalDeviceCount == 0) {
                EERROR("No Vulkan Compatible Device Found!");
                crash();
        }

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(state.instance, &physicalDeviceCount, physicalDevices.data());

        state.physicalDevice = SelectPhysicalDevice(physicalDevices);

        // Show selected device debug info
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(state.physicalDevice, &properties);

        EINFO("Device Selected: {}", properties.deviceName);
        EINFO("Device Type: {}", properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Dedicated" : "Integrated");

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                state.dedicated = true;
}

// CLIENT SIDE FUNCTIONS

Eclipse::RenderDevice::RenderDevice(const CreateInfo& info) {

        // Transfer state from create info to a state object
        state.debug = info.debug;
        state.instanceExtensions = info.extensions;

        // Enable Validation Layers if debug is enabled
        if (state.debug) {
                state.instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
                state.instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        CreateInstance();
        EINFO("Vulkan Instance Created!");

        if (state.debug) {
                CreateDebugger();
                EINFO("Vulkan Debugger Created!");
        }

        if (!SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(info.window.handle), state.instance, nullptr, &state.surface)) {
                EERROR("Failed to create surface!");
                crash();
        }

        CreateDevice();
        EINFO("Vulkan Device Created!");
}

Eclipse::RenderDevice::~RenderDevice() {
        if (state.debug) vkDestroyDebugUtilsMessengerEXT(state.instance, state.debugMessenger, nullptr);
        vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
        vkDestroyInstance(state.instance, nullptr);
}