local ffi = require("ffi")
local renderLib = ffi.load("Engine/Renderer/GL/Engine_Renderer_EXT_GL.dll")

ffi.cdef[[
    typedef unsigned char u8;
    typedef int i32;
    typedef struct {
        bool (*CreateDevice)(EWindow* window);
        void (*DestroyDevice)();
        void (*Clear)(u8 r, u8 g, u8 b, u8 a);
        void (*SetViewport)(i32 x, i32 y, i32 width, i32 height);
        void (*SetVsync)(bool on);
    } ERenderDevice_Layout;

    typedef ERenderDevice_Layout* (*PFN_GetRenderDeviceLayout)();
    ERenderDevice_Layout* ERenderDevice_GetLayout();
]]

local getLayout = ffi.cast("PFN_GetRenderDeviceLayout", renderLib.ERenderDevice_GetLayout)
local layout = getLayout()

local Renderer = {}

function Renderer.Init(window)
    layout.CreateDevice(window)
end

function Renderer.Destroy()
    layout.DestroyDevice()
end

function Renderer.Clear(r, g, b, a)
    layout.Clear(r, g, b, a)
end

function Renderer.SetViewport(width, height)
    layout.SetViewport(0, 0, width, height)
end

return Renderer