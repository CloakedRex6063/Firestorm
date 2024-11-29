#pragma once

namespace FS
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;
        virtual void BeginFrame() = 0;
        virtual void Render() = 0;
        virtual void EndFrame() = 0;
    };
}
