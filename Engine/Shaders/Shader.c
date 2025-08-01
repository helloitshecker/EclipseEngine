#include "Shader.h"

#include <Engine/Core/FileSystem.h>

EShader_Modules* eShader_Create(EShader_CreateInfo* createInfo) {
        EShader_Modules* shaderModules = eMemory_Alloc(createInfo->memory, sizeof(EShader_Modules));
        shaderModules->maxModules = createInfo->maxModules;
        shaderModules->shaders = eMemory_Alloc(createInfo->memory, sizeof(u32) * createInfo->maxModules);
        shaderModules->lastModule = 0;
        shaderModules->shaderType = createInfo->type;
        shaderModules->api = createInfo->api;
        shaderModules->memory = createInfo->memory;
        return shaderModules;
}



void eShader_Destroy(EShader_Modules* modules) {

}

u32 eShader_CompileModule(EShader_Modules* modules, const char* vertex_filepath, const char* fragment_filepath) {
        return 0;
}