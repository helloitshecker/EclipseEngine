#pragma once

#include <Engine/Core/Types.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Memory.h>

typedef enum {
        ESHADER_STAGE_VERTEX,
        ESHADER_STAGE_PIXEL,
        ESHADER_STAGE_GEOMETRY,
} EShader_Stage;

typedef enum {
        ESHADER_TYPE_SPIRV,
        ESHADER_TYPE_GLSL,
        ESHADER_TYPE_HLSL,
} EShader_Type;

typedef struct {
        u32* shaders;
        u32 lastModule;
        u32 maxModules;
        EShader_Type shaderType;
        EApi api;
        EMemory* memory;
} EShader_Modules;

typedef struct {
        EShader_Type type;
        EMemory* memory;
        u32 maxModules;
        EApi api;
} EShader_CreateInfo;

EShader_Modules* eShader_Create(EShader_CreateInfo* createInfo);
void eShader_Destroy(EShader_Modules* modules);

u32 eShader_CompileModule(EShader_Modules* modules, const char* vertex_filepath, const char* fragment_filepath);