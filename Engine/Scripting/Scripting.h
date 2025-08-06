#pragma once

#include <Engine/Core/Debug.h>
#include <Engine/Core/Types.h>
#include <Engine/Core/Time.h>

#include <sys/stat.h>

void eScript_Create();
void eScript_Run();
void eScript_Destroy();

void eScript_CallInit();
void eScript_CallUpdate(f64);
void eScript_CallDestroy();

bool eScript_HasScriptChanged(const char* fileName);
void eScript_Update(float delta);