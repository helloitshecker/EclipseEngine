#include "Scripting.h"

#include <luajit/lua.h>
#include <luajit/lauxlib.h>
#include <luajit/lualib.h>

#include <Engine/Core/FileSystem.h>
#include <Engine/Core/Memory.h>
#include <Engine/Core/Types.h>

thread_local lua_State* L = nullptr;
static time_t last_modified = 0;
static const char* fileName = "../Engine/Scripts/main.lua";
static f64 timeSinceLastCheck = 0.0f;
static const f64 hotReloadInterval = 2.0f;

void eScript_Create() {
        EDEBUG("Initializing script library");
        L = luaL_newstate();
        luaL_openlibs(L);
}

void eScript_Run() {
        EDEBUG("Running script file: \"%s\"", fileName);
        if (luaL_dofile(L, fileName) != LUA_OK) {
                EERROR("Lua Error: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}

void eScript_Destroy() {
        EDEBUG("Destroying script library");
        lua_close(L);
}


void eScript_CallInit() {
        lua_getglobal(L, "init");
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                EERROR("Lua Error: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}

void eScript_CallUpdate(f64 delta) {
        lua_getglobal(L, "update");
        lua_pushnumber(L, delta);
        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                EERROR("Lua Error: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}

void eScript_CallDestroy() {
        lua_getglobal(L, "destroy");
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                EERROR("Lua Error: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}

bool eScript_HasScriptChanged(const char* fileName) {
        struct stat attr;
        if (stat(fileName, &attr) == 0) {
                if (attr.st_mtime != last_modified) {
                        last_modified = attr.st_mtime;
                        return true;
                }
        }
        return false;
}

void eScript_Update(float delta) {
#ifndef NDEBUG
        timeSinceLastCheck += delta;
        if (timeSinceLastCheck >= hotReloadInterval) {
                timeSinceLastCheck = 0.0f;
                if (eScript_HasScriptChanged(fileName)) {
                        EDEBUG("Detected Script Change! Reloading...");

                        if (luaL_loadfile(L, fileName) || lua_pcall(L, 0, 0, 0)) {
                                EERROR("Lua Error while reloading: %s", lua_tostring(L, -1));
                                lua_pop(L, 1); // Pop error message
                        }
                }
        }
#endif
        lua_getglobal(L, "update");
        if (lua_isfunction(L, -1)) {
                lua_pushnumber(L, delta);
                if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                        EERROR("Lua Error in update(): %s", lua_tostring(L, -1));
                        lua_pop(L, 1); // Pop error message
                }
        } else {
                lua_pop(L, 1); // Clean non-function off the stack
        }
}
