package.path = package.path .. ";../Engine/Scripts/?.lua"
local Engine = require("Engine.Engine")

function init()
    local win = Engine.Window.create("Eclipse Engine", 800, 600, true, false, Engine.GL)
    Engine.Renderer.Init(win)
end

x = 0

function update(delta)
    x = (x + 1) % 255
    Engine.Renderer.Clear(0, 0, 0, 255)
end

function destroy()
    Engine.Renderer.Destroy()
end
