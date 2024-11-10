#include "Core/App.hpp"

class Sandbox final : public FS::App
{
public:
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

std::unique_ptr<FS::App> CreateApp() {return std::make_unique<Sandbox>();}