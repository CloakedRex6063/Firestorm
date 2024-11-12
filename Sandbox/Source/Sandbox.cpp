#include "Core/App.hpp"
#include "Core/Render/Resources/Model.hpp"

class Sandbox final : public FS::App
{
public:
    void Init() override
    {
        auto indices = std::vector{0u, 1u, 2u, 2u, 3u, 0u};
        auto vertices = std::vector
        {
            FS::Vertex({-0.5f, -0.5f, 1.f}, 0, {}, 0, {1.f, 0.f, 0.f, 1.f}),
            FS::Vertex({0.5f, -0.5f, 1.f}, 0, {}, 0, {0.f, 1.f, 0.f, 1.f}),
            FS::Vertex({0.5f, 0.5f, 1.f}, 0, {}, 0, {0.f, 0.f, 0.f, 1.f}),
            FS::Vertex({-0.5f, 0.5f, 1.f}, 0, {}, 0, {0.f, 0.f, 1.f, 1.f}),
        };
    }
    void Update() override
    {
	    
    }
    void Shutdown() override
    {
	    
    }
};

std::unique_ptr<FS::App> CreateApp() {return std::make_unique<Sandbox>();}