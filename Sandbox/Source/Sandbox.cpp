#include "Core/App.hpp"
#include "Core/Systems/FileSystem.h"
#include "Core/Systems/ResourceSystem.h"

class Sandbox final : public FS::App
{
public:
    FS::ResourceHandle model;
    void Init() override
    {

    }
    void Update() override
    {
    }
    void Shutdown() override
    {
        
    }
};

std::unique_ptr<FS::App> CreateApp() { return std::make_unique<Sandbox>(); }