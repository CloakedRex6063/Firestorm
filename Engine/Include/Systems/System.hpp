#pragma once

namespace FS
{
    class System
    {
    public:
        virtual ~System() = default;
        NON_COPYABLE(System);
        NON_MOVABLE(System);

        virtual void Init() {}
        virtual void Update(float) {}
        virtual void FixedUpdate(float) {}
        virtual void BeginFrame() {}
        virtual void EndFrame() {}
        virtual void Shutdown() {}

        template <typename T, typename... Args>
        static std::shared_ptr<T> CreateSystem(Args&&... args)
        {
            return std::shared_ptr<T>(new T(std::forward<Args>(args)...),
                                      [](System* system)
                                      {
                                          system->Shutdown();
                                          delete system;
                                      });
        }

    protected:
        friend class EngineClass;
        System() = default;
    };
}  // namespace bee