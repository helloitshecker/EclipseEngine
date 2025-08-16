#include "RenderDevice.hpp"

#include <Engine/Core/Error.hpp>

#define crash() abort();

static std::string VAR_FOR_CheckExtensionSupport_ExtensionNotFoundName;
bool Eclipse::RenderDevice::CheckExtensionSupport(const std::vector<const char*>& provided_extensions) {
        u32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (auto& extension : provided_extensions) {
                bool found = false;
                for (size_t i = 0; i < extensions.size(); i++) {
                        if (strcmp(extensions[i].extensionName, extension) == 0) {
                                found = true;
                        }
                }
                if (!found) {
                        VAR_FOR_CheckExtensionSupport_ExtensionNotFoundName.append(extension);
                        return false;
                }
        }

        return true;
}

void Eclipse::RenderDevice::CreateInstance(const std::vector<const char*>& extensions) {
        volkInitialize();

        VkApplicationInfo appInfo {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName = "Eclipse",
                .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
                .pEngineName = "Eclipse",
                .engineVersion = VK_MAKE_VERSION(0, 0, 1),
                .apiVersion = volkGetInstanceVersion(),
        };

        VkInstanceCreateInfo info {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &appInfo,
                .enabledExtensionCount = static_cast<u32>(extensions.size()),
                .ppEnabledExtensionNames = extensions.data(),
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
        };

        if (!CheckExtensionSupport(extensions)) {
                EERROR("Extension \"{}\" not supported!", VAR_FOR_CheckExtensionSupport_ExtensionNotFoundName);
                crash();
        }

        if (vkCreateInstance(&info, nullptr, &state.instance) != VK_SUCCESS) {
                EERROR("Failed to create instance!");
                crash();
        }

        volkLoadInstance(state.instance);
}

Eclipse::RenderDevice::RenderDevice(const CreateInfo& info) {
        CreateInstance(info.extensions);
        EINFO("Vulkan Instance Created!");
}

Eclipse::RenderDevice::~RenderDevice() {
        vkDestroyInstance(state.instance, nullptr);
}