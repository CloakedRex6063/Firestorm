#include "Core/App.hpp"

namespace FS
{
    class Editor final : public App
    {
    public:
        void Init() override
        {
        
        }
        void Update(float dt) override
        {

        }
        void Shutdown() override
        {
        
        }
    };

    Scoped<App> CreateApp()
    {
        return std::make_unique<Editor>();
    }
}
