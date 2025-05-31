#pragma once

#undef UNICODE
#undef _UNICODE
#define NOMINMAX
#define NOCRYPT       
#define NORPC        
#define NOIME      
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "comdef.h"

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "dxcapi.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "expected"
#include "optional"
#include "memory"
#include "algorithm"
#include "vector"
#include "span"
#include "ranges"
#include "string_view"
#include "print"

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

namespace FS
{
    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T>
    constexpr Ref<T> MakeRef()
    {
        return std::make_shared<T>();
    }

    template <typename T>
    using Scoped = std::unique_ptr<T>;

    template <typename T>
    constexpr Scoped<T> MakeScoped()
    {
        return std::make_unique<T>();
    }

    template <typename T>
    using Opt = std::optional<T>;

    template <typename T, typename Error>
    using Exp = std::expected<T, Error>;

    template <typename T>
    using Vec = std::vector<T>;

    template <typename T, size_t N>
    using Arr = std::array<T, N>;

    template <typename T>
    using Span = std::span<T>;
}

#include "Tools/Warnings.hpp"
