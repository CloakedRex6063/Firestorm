#pragma once 
#include "System.hpp"
#include "Render/IRenderBackend.hpp"

namespace FS
{
    class Renderer final : public System
    {
    public:
        void Init() override;
        void Update(float) override;
        void Shutdown() override;

    private:
        Scoped<IRenderBackend> m_context;
    };
}
