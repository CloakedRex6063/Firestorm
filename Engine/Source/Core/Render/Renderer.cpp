#include "Core/Render/Renderer.h"
#include "Core/Render/Window.h"

FS::Renderer::Renderer(): mWindow(std::make_unique<Window>()) {}
