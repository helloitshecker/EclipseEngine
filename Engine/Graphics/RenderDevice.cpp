#include "RenderDevice.hpp"

#include <Engine/Core/Error.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <set>

// GLOBALS

// STATIC LOCAL FUNCTIONS

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
      switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Eclipse::LogInfo("[VALIDATION] {}", pCallbackData->pMessage);
            break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Eclipse::LogWarn("[VALIDATION] {}", pCallbackData->pMessage);
            break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            Eclipse::LogError("[VALIDATION] {}", pCallbackData->pMessage);
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
            for (auto& [extensionName, specVersion] : extensions) {
                  if (strcmp(extensionName, extension) == 0) {
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
bool Eclipse::RenderDevice::CheckInstanceLayerSupport(const std::vector<const char*>& provided_layers) {
      u32 layerCount = 0;
      vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

      std::vector<VkLayerProperties> availableLayers(layerCount);
      vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

      for (const auto layer : provided_layers) {
            bool found = false;
            for (const auto& availableLayer : availableLayers) {
                  if (strcmp(availableLayer.layerName, layer) == 0) {
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

VkPhysicalDevice Eclipse::RenderDevice::SelectPhysicalDevice(const std::vector<VkPhysicalDevice>& devices) const {
      i64 bestScore = -1;
      u32 bestIndex = -1;
      u32 currentIndex = 0;
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
                  if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT)  indices.compute = i;
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
                  bestIndex = currentIndex;
            }

            currentIndex++;
      }

      if (bestScore == -1) {
            Eclipse::LogError("Device not compatible with support vulkan functionality!");
            abort();
      }

      return bestDevice;
}

Eclipse::RenderDevice::SwapchainSupportDetails Eclipse::RenderDevice::QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
      SwapchainSupportDetails details{};

      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

      u32 formatCount = 0;
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
      if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
      }

      u32 presentModeCount = 0;
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
      if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
      }

      return details;
}

// INTERNAL VULKAN WRAPPER FUNCTIONS

void Eclipse::RenderDevice::CreateInstance() {
      if (volkInitialize()) {
            Eclipse::LogError("Failed to initialize vulkan!");
            abort();
      }

      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Eclipse";
      appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
      appInfo.pEngineName = "Eclipse";
      appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
      appInfo.apiVersion = volkGetInstanceVersion();

      VkInstanceCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      info.pApplicationInfo = &appInfo;
      info.enabledExtensionCount = static_cast<u32>(state.instanceExtensions.size());
      info.ppEnabledExtensionNames = state.instanceExtensions.data();
      info.enabledLayerCount = static_cast<u32>(state.instanceLayers.size());
      info.ppEnabledLayerNames = state.instanceLayers.data();

      if (!CheckInstanceExtensionSupport(state.instanceExtensions)) {
            Eclipse::LogError("Extension \"{}\" not supported!", VAR_FOR_CheckInstanceExtensionSupport_ExtensionNotFoundName);
      }

      if (!CheckInstanceLayerSupport(state.instanceLayers)) {
            Eclipse::LogError("Layer \"{}\" not supported!", VAR_FOR_CheckInstanceLayerSupport_ExtensionNotFoundName);
      }

      if (vkCreateInstance(&info, nullptr, &state.instance) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create instance!");
            abort();
      }

      volkLoadInstance(state.instance);
}

void Eclipse::RenderDevice::CreateDebugger() {
      constexpr VkDebugUtilsMessengerCreateInfoEXT info{
              .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
              .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
              .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
              .pfnUserCallback = debug_callback,
              .pUserData = nullptr,
      };

      if (vkCreateDebugUtilsMessengerEXT(state.instance, &info, nullptr, &state.debugMessenger) != VK_SUCCESS) {
            Eclipse::LogWarn("Failed to initialize Debugger for Vulkan!");
      }
}

void Eclipse::RenderDevice::CreateDevice() {
      u32 physicalDeviceCount{};
      vkEnumeratePhysicalDevices(state.instance, &physicalDeviceCount, nullptr);

      if (physicalDeviceCount == 0) {
            Eclipse::LogError("No Vulkan Compatible Device Found!");
            abort();
      }

      std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
      vkEnumeratePhysicalDevices(state.instance, &physicalDeviceCount, physicalDevices.data());

      state.physicalDevice = SelectPhysicalDevice(physicalDevices);

      // Show selected device debug info
      VkPhysicalDeviceProperties properties;
      vkGetPhysicalDeviceProperties(state.physicalDevice, &properties);

      Eclipse::LogInfo("Device Selected: {}", properties.deviceName);
      Eclipse::LogInfo("Device Type: {}", properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Dedicated" : "Integrated");

      if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            state.dedicated = true;

      u32 queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(state.physicalDevice, &queueFamilyCount, nullptr);

      std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(state.physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

      for (u32 i = 0; i < queueFamilyCount; i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                  state.families.graphics = i;
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                  state.families.compute = i;
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                  state.families.transfer = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(state.physicalDevice, i, state.surface, &presentSupport);

            if (presentSupport) {
                  state.families.present = i;
            }

            if (state.families.isComplete()) break;
      }

      if (!state.families.isComplete()) {
            Eclipse::LogError("Selected device doesnt support all queue families!");
      }
      if (!state.families.graphics.has_value()) {
            Eclipse::LogError("Selected device doesnt support graphics!");
      }
      if (!state.families.compute.has_value()) {
            Eclipse::LogError("Selected device doesnt support compute!");
      }
      if (!state.families.present.has_value()) {
            Eclipse::LogError("Selected device doesnt support presentation!");
      }
      if (!state.families.transfer.has_value()) {
            Eclipse::LogError("Selected device doesnt support transfer!");
      }

      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
      f32 queuePriorities[] = { 1.0f };
      std::set<u32> uniqueFamilies = {
              state.families.graphics.value(),
              state.families.present.value(),
              state.families.compute.value(),
              state.families.transfer.value(),
      };

      for (u32 family : uniqueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = family,
                    .queueCount = 1,
                    .pQueuePriorities = queuePriorities,
            };
            queueCreateInfos.push_back(queueCreateInfo);
      }

      VkPhysicalDeviceFeatures deviceFeatures{};

      VkDeviceCreateInfo deviceCreateInfo{};
      deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      deviceCreateInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
      deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
      deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
      deviceCreateInfo.enabledExtensionCount = static_cast<u32>(state.deviceExtensions.size());
      deviceCreateInfo.ppEnabledExtensionNames = state.deviceExtensions.data();

      if (vkCreateDevice(state.physicalDevice, &deviceCreateInfo, nullptr, &state.logicalDevice) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create logical device!");
            abort();
      }

      vkGetDeviceQueue(state.logicalDevice, state.families.graphics.value(), 0, &state.graphicsQueue);
      vkGetDeviceQueue(state.logicalDevice, state.families.compute.value(), 0, &state.computeQueue);
      vkGetDeviceQueue(state.logicalDevice, state.families.transfer.value(), 0, &state.transferQueue);
      vkGetDeviceQueue(state.logicalDevice, state.families.present.value(), 0, &state.presentQueue);
}

void Eclipse::RenderDevice::CreateSwapchain() {
      SwapchainSupportDetails support = QuerySwapchainSupport(state.physicalDevice, state.surface);

      auto chooseSurfaceFormat = [](const std::vector<VkSurfaceFormatKHR>& formats) {
            for (const auto& format : formats) {
                  if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        return format;
                  }
            }
            return formats[0];
            };
      auto choosePresentMode = [](const std::vector<VkPresentModeKHR>& modes) {
            for (const auto& mode : modes) {
                  if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                        return mode;
                  }
            }
            return VK_PRESENT_MODE_FIFO_KHR;
            };
      auto chooseExtent = [&](const VkSurfaceCapabilitiesKHR& caps) {
            if (caps.currentExtent.width != UINT32_MAX) {
                  return caps.currentExtent;
            }
            else {
                  VkExtent2D actualExtent{
                          static_cast<u32>(state.windowWidth),
                          static_cast<u32>(state.windowHeight)
                  };

                  actualExtent.width = std::max(caps.minImageExtent.width,
                        std::min(caps.maxImageExtent.width, actualExtent.width));
                  actualExtent.height = std::max(caps.minImageExtent.height,
                        std::min(caps.maxImageExtent.height, actualExtent.height));

                  return actualExtent;
            }
            };

      VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(support.formats);
      VkPresentModeKHR presentMode = choosePresentMode(support.presentModes);
      VkExtent2D extent = chooseExtent(support.capabilities);

      u32 imageCount = support.capabilities.minImageCount + 1;
      if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
            imageCount = support.capabilities.maxImageCount;
      }

      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = state.surface;
      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      createInfo.imageArrayLayers = 1;
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      u32 queueFamilyIndices[] = {
              state.families.graphics.value(),
              state.families.present.value(),
      };

      if (state.families.graphics != state.families.present) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
      }
      else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      }

      createInfo.preTransform = support.capabilities.currentTransform;
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;
      createInfo.oldSwapchain = VK_NULL_HANDLE;

      if (vkCreateSwapchainKHR(state.logicalDevice, &createInfo, nullptr, &state.swapchain) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create Vulkan swapchain!");
            abort();
      }

      vkGetSwapchainImagesKHR(state.logicalDevice, state.swapchain, &imageCount, nullptr);
      state.swapchainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(state.logicalDevice, state.swapchain, &imageCount, state.swapchainImages.data());

      state.swapchainFormat = surfaceFormat.format;
      state.swapchainExtent = extent;
}

void Eclipse::RenderDevice::CreateImageViews() {
      state.swapchainImageViews.resize(state.swapchainImages.size());

      i32 i = 0;
      for (auto& image : state.swapchainImages) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = image;
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = state.swapchainFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(state.logicalDevice, &createInfo, nullptr, &state.swapchainImageViews[i++]) != VK_SUCCESS) {
                  Eclipse::LogError("Failed to create Vulkan image view!");
                  abort();
            }
      }
}

void Eclipse::RenderDevice::CreateGraphicsPipeline() {
      Eclipse::ShaderManager shaderManager {
            {
                  .optimizationLevel = Eclipse::ShaderManager::ShaderOptimizationLevel::PERFORMANCE
            }
      };

      if (shaderManager.error) {
            Eclipse::LogError("Failed to create graphics pipeline!");
            abort();
      }

      // Compiling triangle shader
      auto triangle_shader_vert = shaderManager.compile({
            .name = Eclipse::FileSystem::ToShaderPath("triangle.vert").value().string(),
            .entry = "main",
            .type = Eclipse::ShaderManager::ShaderType::VERTEX
      });

      auto triangle_shader_frag = shaderManager.compile({
            .name = Eclipse::FileSystem::ToShaderPath("triangle.frag").value().string(),
            .entry = "main",
            .type = Eclipse::ShaderManager::ShaderType::FRAGMENT
      });

      if (triangle_shader_vert == std::nullopt || triangle_shader_frag == std::nullopt) {
            Eclipse::LogError("Failed to compile triangle shader!");
            abort();
      } else {
            Eclipse::LogInfo("Successfully compiled triangle shader!");
      }

}

// CLIENT SIDE FUNCTIONS

Eclipse::RenderDevice::RenderDevice(const CreateInfo& info) {

      // Transfer state from create info to a state object
      state.debug = info.debug;
      state.instanceExtensions = info.extensions;

      state.deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
      state.deviceExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
      state.deviceExtensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

      // Enable Validation Layers if debug is enabled
      if (state.debug) {
            state.instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
            state.instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      }

      SDL_GetWindowSize(static_cast<SDL_Window*>(info.window.handle), &state.windowWidth, &state.windowHeight);

      CreateInstance();
      Eclipse::LogInfo("Vulkan Instance Created!");

      if (state.debug) {
            CreateDebugger();
            Eclipse::LogInfo("Vulkan Debugger Created!");
      }

      if (!SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(info.window.handle), state.instance, nullptr, &state.surface)) {
            Eclipse::LogError("Failed to create surface!");
      }

      CreateDevice();
      Eclipse::LogInfo("Vulkan Device Created!");

      CreateSwapchain();
      Eclipse::LogInfo("Vulkan Swapchain Created!");

      CreateImageViews();
      Eclipse::LogInfo("Vulkan Image View Created!");

      CreateGraphicsPipeline();
      Eclipse::LogInfo("Vulkan Graphics Pipeline Created!");
}

Eclipse::RenderDevice::~RenderDevice() {
      if (state.debug) vkDestroyDebugUtilsMessengerEXT(state.instance, state.debugMessenger, nullptr);

      for (auto imageView : state.swapchainImageViews) {
            vkDestroyImageView(state.logicalDevice, imageView, nullptr);
      }

      vkDestroySwapchainKHR(state.logicalDevice, state.swapchain, nullptr);
      vkDestroyDevice(state.logicalDevice, nullptr);
      vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
      vkDestroyInstance(state.instance, nullptr);
}