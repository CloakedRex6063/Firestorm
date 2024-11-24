#pragma once

namespace FS
{
    class System
    {
    public:
        System() = default;
        virtual ~System() = default;
        NON_COPYABLE(System);
        MOVABLE(System);

        virtual void Init() {}
        virtual void Update(float /*deltaTime*/) {}
        virtual void FixedUpdate(float /*deltaTime*/) {}
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

        bool operator==(const System& other) const { return mName == other.mName; }

        std::string mName;

    protected:
        friend class EngineClass;
    };
}  // namespace FS

template <>
struct std::hash<FS::System>
{
    size_t operator()(const FS::System& system) const noexcept
    {
        return hash<std::string>()(system.mName); 
    }
};