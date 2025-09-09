#include "RenderDevice.hpp"

#include <Engine/Core/Error.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <set>
#include <array>

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

std::optional<VkShaderModule> Eclipse::RenderDevice::CreateShaderModule(const Eclipse::FileSystem::CustomFileContent<u32>& file) const {
      VkShaderModuleCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      createInfo.codeSize = file.size() * sizeof(u32);
      createInfo.pCode = file.data();

      VkShaderModule module;
      if (vkCreateShaderModule(state.logicalDevice, &createInfo, nullptr, &module) != VK_SUCCESS) {
            return std::nullopt;
      }

      return module;
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

      VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
      dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
      dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

      VkPhysicalDeviceSynchronization2Features synchronization2Features{};
      synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
      synchronization2Features.synchronization2 = VK_TRUE;
      synchronization2Features.pNext = &dynamicRenderingFeatures;

      VkDeviceCreateInfo deviceCreateInfo{};
      deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      deviceCreateInfo.pNext = &synchronization2Features;
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
      for (const auto& image : state.swapchainImages) {
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
      state.shaderManager = std::make_unique<ShaderManager>(Eclipse::ShaderManager::ShaderManagerCreateInfo{});

      if (state.shaderManager->error) {
            Eclipse::LogError("Failed to create graphics pipeline!");
            abort();
      }

      // Compiling triangle shader
      const auto triangle_shader_vert = state.shaderManager->compile({
            .name = Eclipse::FileSystem::ToShaderPath("triangle.vert").value().string(),
            .entry = "main",
            .type = Eclipse::ShaderManager::ShaderType::VERTEX
      });

      const auto triangle_shader_frag = state.shaderManager->compile({
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

      auto vert_shader_module = CreateShaderModule(triangle_shader_vert.value().spirv_code);
      auto frag_shader_module = CreateShaderModule(triangle_shader_frag.value().spirv_code);

      if (vert_shader_module == std::nullopt || frag_shader_module == std::nullopt) {
            Eclipse::LogError("Failed to attach triangle shader to vulkan :(!");
      }

      VkPipelineShaderStageCreateInfo shader_stage_create_info_v{};
      shader_stage_create_info_v.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shader_stage_create_info_v.stage = VK_SHADER_STAGE_VERTEX_BIT;
      shader_stage_create_info_v.module = vert_shader_module.value();
      shader_stage_create_info_v.pName = "main";

      VkPipelineShaderStageCreateInfo shader_stage_create_info_f{};
      shader_stage_create_info_f.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shader_stage_create_info_f.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      shader_stage_create_info_f.module = frag_shader_module.value();
      shader_stage_create_info_f.pName = "main";

      std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {shader_stage_create_info_v, shader_stage_create_info_f};

      constexpr std::array<VkDynamicState, 2> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

      VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
      dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamic_state_create_info.dynamicStateCount = static_cast<u32>(dynamic_states.size());
      dynamic_state_create_info.pDynamicStates = dynamic_states.data();

      VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
      vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
      vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
      vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
      vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

      VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
      input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(state.swapchainExtent.width);
      viewport.height = static_cast<float>(state.swapchainExtent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = state.swapchainExtent;

      VkPipelineViewportStateCreateInfo viewport_state_create_info{};
      viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewport_state_create_info.viewportCount = 1;
      viewport_state_create_info.pViewports = &viewport;
      viewport_state_create_info.scissorCount = 1;
      viewport_state_create_info.pScissors = &scissor;

      VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{};
      rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterization_state_create_info.depthClampEnable = VK_FALSE;
      rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
      rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
      rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
      rasterization_state_create_info.lineWidth = 1.0f;
      rasterization_state_create_info.depthBiasEnable = VK_FALSE;
      rasterization_state_create_info.depthBiasClamp = 0.0f;
      rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
      rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

      VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};
      multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisample_state_create_info.sampleShadingEnable = VK_FALSE;
      multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multisample_state_create_info.minSampleShading = 1.0f;
      multisample_state_create_info.pSampleMask = nullptr;
      multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
      multisample_state_create_info.alphaToOneEnable = VK_FALSE;

      VkPipelineColorBlendAttachmentState color_blend_attachment{};
      color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      color_blend_attachment.blendEnable = VK_TRUE;
      color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
      color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;

      VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{};
      color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      color_blend_state_create_info.logicOpEnable = VK_FALSE;
      color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
      color_blend_state_create_info.attachmentCount = 1;
      color_blend_state_create_info.pAttachments = &color_blend_attachment;
      color_blend_state_create_info.blendConstants[0] = 0.0f;
      color_blend_state_create_info.blendConstants[1] = 0.0f;
      color_blend_state_create_info.blendConstants[2] = 0.0f;
      color_blend_state_create_info.blendConstants[3] = 0.0f;

      VkPipelineLayoutCreateInfo pipelineLayout_create_info{};
      pipelineLayout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayout_create_info.setLayoutCount = 0;
      pipelineLayout_create_info.pSetLayouts = nullptr;
      pipelineLayout_create_info.pushConstantRangeCount = 0;
      pipelineLayout_create_info.pPushConstantRanges = nullptr;

      if (vkCreatePipelineLayout(state.logicalDevice, &pipelineLayout_create_info, nullptr, &state.pipelineLayout) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create vulkan pipeline layout!");
            abort();
      }

      VkPipelineRenderingCreateInfo pipeline_rendering_create_info{};
      pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
      pipeline_rendering_create_info.colorAttachmentCount = 1;
      pipeline_rendering_create_info.pColorAttachmentFormats = &state.swapchainFormat;

      VkGraphicsPipelineCreateInfo pipeline_create_info{};
      pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipeline_create_info.stageCount = static_cast<u32>(shader_stages.size());
      pipeline_create_info.pStages = shader_stages.data();
      pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
      pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
      pipeline_create_info.pViewportState = &viewport_state_create_info;
      pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
      pipeline_create_info.pMultisampleState = &multisample_state_create_info;
      pipeline_create_info.pDepthStencilState = nullptr;
      pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
      pipeline_create_info.pDynamicState = &dynamic_state_create_info;
      pipeline_create_info.layout = state.pipelineLayout;
      pipeline_create_info.renderPass = VK_NULL_HANDLE;
      pipeline_create_info.subpass = 0;
      pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
      pipeline_create_info.basePipelineIndex = -1;
      pipeline_create_info.pNext = &pipeline_rendering_create_info;

      if (vkCreateGraphicsPipelines(state.logicalDevice, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &state.graphicsPipeline) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create graphics pipeline!");
            abort();
      }

      vkDestroyShaderModule(state.logicalDevice, frag_shader_module.value(), nullptr);
      vkDestroyShaderModule(state.logicalDevice, vert_shader_module.value(), nullptr);
}

void Eclipse::RenderDevice::CreateCommandPool() {
      VkCommandPoolCreateInfo command_pool_create_info{};
      command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      command_pool_create_info.queueFamilyIndex = state.families.graphics.value();

      if (vkCreateCommandPool(state.logicalDevice, &command_pool_create_info, nullptr, &state.commandPool) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create command pool!");
            abort();
      }
}

void Eclipse::RenderDevice::CreateCommandBuffers() {
      state.commandBuffers.resize(state.swapchainImageViews.size());

      VkCommandBufferAllocateInfo command_buffer_allocate_info{};
      command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      command_buffer_allocate_info.commandPool = state.commandPool;
      command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      command_buffer_allocate_info.commandBufferCount = static_cast<u32>(state.commandBuffers.size());

      if (vkAllocateCommandBuffers(state.logicalDevice, &command_buffer_allocate_info, state.commandBuffers.data()) != VK_SUCCESS) {
            Eclipse::LogError("Failed to create command buffers!");
            abort();
      }
}



void Eclipse::RenderDevice::RecordCommandBuffer(const VkCommandBuffer command_buffer, const u32 image_index) const {
      constexpr VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = nullptr,
      };

      if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            Eclipse::LogError("Failed to begin command buffer!");
            abort();
      }

      const VkRenderingAttachmentInfo color_attachment_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = state.swapchainImageViews[image_index],
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = {.color = {0.0f, 0.0f, 0.0f, 1.0f}, },
      };

      const VkRenderingInfo rendering_info{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {{0, 0}, state.swapchainExtent},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_info,
      };

      vkCmdBeginRendering(command_buffer, &rendering_info);

      vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.graphicsPipeline);

      const VkViewport viewport {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<f32>(state.swapchainExtent.width),
            .height = static_cast<f32>(state.swapchainExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
      };
      vkCmdSetViewport(command_buffer, 0, 1, &viewport);

      const VkRect2D scissor {
            .offset = {0, 0},
            .extent = state.swapchainExtent
      };
      vkCmdSetScissor(command_buffer, 0, 1, &scissor);

      // DRAW COMMANDS START HERE

      vkCmdDraw(command_buffer, 3, 1, 0, 0);

      // DRAW COMMANDS END HERE

      vkCmdEndRendering(command_buffer);

      if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            Eclipse::LogError("Failed to end command buffer!");
            abort();
      }
}

void Eclipse::RenderDevice::CreateSyncObjects() {
      state.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
      state.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
      state.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

      VkSemaphoreCreateInfo semaphoreInfo{};
      semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

      VkFenceCreateInfo fenceInfo{};
      fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(
                  state.logicalDevice, &semaphoreInfo, nullptr, &state.imageAvailableSemaphores[i]) != VK_SUCCESS ||
                  vkCreateSemaphore(state.logicalDevice, &semaphoreInfo, nullptr, &state.renderFinishedSemaphores[i]) != VK_SUCCESS ||
                  vkCreateFence(state.logicalDevice, &fenceInfo, nullptr, &state.inFlightFences[i]) != VK_SUCCESS
            ) {
                  Eclipse::LogError("Failed to create synchronization objects!");
                  abort();
            }
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

      CreateCommandPool();
      Eclipse::LogInfo("Vulkan Command Pool Created!");

      CreateCommandBuffers();
      Eclipse::LogInfo("Vulkan Command Buffer Created!");

      CreateSyncObjects();
      Eclipse::LogInfo("Vulkan Sync Object Created!");
}

Eclipse::RenderDevice::~RenderDevice() {
      vkDeviceWaitIdle(state.logicalDevice);

      if (state.debug) vkDestroyDebugUtilsMessengerEXT(state.instance, state.debugMessenger, nullptr);

      for (const auto& imageView : state.swapchainImageViews) {
            vkDestroyImageView(state.logicalDevice, imageView, nullptr);
      }

      for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(state.logicalDevice, state.renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(state.logicalDevice, state.imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(state.logicalDevice, state.inFlightFences[i], nullptr);
      }

      vkDestroyCommandPool(state.logicalDevice, state.commandPool, nullptr);
      vkDestroyPipeline(state.logicalDevice, state.graphicsPipeline, nullptr);
      vkDestroyPipelineLayout(state.logicalDevice, state.pipelineLayout, nullptr);
      vkDestroySwapchainKHR(state.logicalDevice, state.swapchain, nullptr);
      vkDestroyDevice(state.logicalDevice, nullptr);
      vkDestroySurfaceKHR(state.instance, state.surface, nullptr);
      vkDestroyInstance(state.instance, nullptr);
}

void Eclipse::RenderDevice::DrawFrame() {
      vkWaitForFences(state.logicalDevice, 1, &state.inFlightFences[state.currentFrame], VK_TRUE, UINT64_MAX);
      vkResetFences(state.logicalDevice, 1, &state.inFlightFences[state.currentFrame]);

      uint32_t imageIndex;
      vkAcquireNextImageKHR(state.logicalDevice, state.swapchain, UINT64_MAX, state.imageAvailableSemaphores[state.currentFrame], VK_NULL_HANDLE, &imageIndex);

      vkResetCommandBuffer(state.commandBuffers[state.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
      RecordCommandBuffer(state.commandBuffers[state.currentFrame], imageIndex);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

      VkSemaphore waitSemaphores[] = {state.imageAvailableSemaphores[state.currentFrame]};
      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphores;
      submitInfo.pWaitDstStageMask = waitStages;

      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &state.commandBuffers[state.currentFrame];

      VkSemaphore signalSemaphores[] = {state.renderFinishedSemaphores[state.currentFrame]};
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;

      if (vkQueueSubmit(state.graphicsQueue, 1, &submitInfo, state.inFlightFences[state.currentFrame]) != VK_SUCCESS) {
            Eclipse::LogError("Failed to acquire image!");
            abort();
      }

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;

      VkSwapchainKHR swapChains[] = {state.swapchain};
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapChains;

      presentInfo.pImageIndices = &imageIndex;

      vkQueuePresentKHR(state.presentQueue, &presentInfo);

      state.currentFrame = (state.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}