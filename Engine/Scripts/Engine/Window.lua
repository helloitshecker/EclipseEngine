local ffi = require("ffi")
local engine = ffi.load("Engine_System")

ffi.cdef[[
    // Structs
    typedef struct {
        void* ptr;
        void* glcontext;
    } EWindow;

    typedef enum {
        GL,
        VK,
        METAL
    } EApi;

    EWindow* eWindow_Simple_Create(const char* title, int width, int height, bool resizable, bool fullscreen, EApi api);
    void eWindow_Simple_PollEvent();
    void eWindow_Simple_Swap();
]]

Window = {
    current = nil
}

function Window.create(title, width, height, resizable, fullscreen, api)
    api = api or ffi.C.GL   -- Defaults to OpenGL if API is not provided
    Window.current = engine.eWindow_Simple_Create(title, width, height, resizable, fullscreen, api)
    return Window.current
end

function Window.swap()
    if Window.current ~= nil then
        engine.eWindow_Simple_Swap()
    end
end

function Window.poll()
    if Window.current ~= nil then
        engine.eWindow_Simple_PollEvent()
    end
end

return Window