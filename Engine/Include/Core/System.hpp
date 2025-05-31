#pragma once

namespace FS
{
    class System;
    template <class T>
    concept SystemConcept = std::is_base_of_v<System, T>;

    class System
    {
    public:
        virtual ~System() = default;
        virtual void Init() = 0;
        virtual void Update(float) = 0;
        virtual void Shutdown() = 0;

        int m_priority = -1;
    };
}
