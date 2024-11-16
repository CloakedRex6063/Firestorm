#include "Core/App.hpp"
#include "Core/FileSystem.h"
#include "Core/ResourceSystem.h"

class Sandbox final : public FS::App
{
public:
    FS::ResourceHandle model;
    void Init() override
    {
        const auto path = FS::gEngine.FileSystem().GetPath(FS::Directory::eGameAssets, "Models/Avocado.glb");
        model = FS::gEngine.ResourceSystem().LoadModel(path).value();
    }
    void Update() override
    {
    }
    void Shutdown() override {}
};

std::unique_ptr<FS::App> CreateApp() { return std::make_unique<Sandbox>(); }