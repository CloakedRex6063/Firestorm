#pragma once

// Inspired from vulkan-hpp's implementation
namespace FS
{
    template <typename T> class ArrayProxy
    {
    public:
        constexpr ArrayProxy()
            : mCount(0), mPtr(nullptr)
        {
        }

        constexpr ArrayProxy(std::nullptr_t)
            : mCount(0), mPtr(nullptr)
        {
        }

        ArrayProxy(T const& value)
            : mCount(1), mPtr(&value)
        {
        }

        ArrayProxy(const uint32_t count, T const* ptr)
            : mCount(count), mPtr(ptr)
        {
        }

        template <std::size_t C>
        ArrayProxy(T const (&ptr)[C])
            : mCount(C), mPtr(ptr)
        {
        }

        ArrayProxy(std::initializer_list<T> const& list)
            : mCount(static_cast<uint32_t>(list.size())), mPtr(list.begin())
        {
        }

        template <typename B = T, std::enable_if_t<std::is_const_v<B>, int> = 0>
        ArrayProxy(std::initializer_list<std::remove_const_t<T>> const& list)
            : mCount(static_cast<uint32_t>(list.size())), mPtr(list.begin())
        {
        }

        template <typename V,
                  std::enable_if_t<std::is_convertible_v<decltype(std::declval<V>().data()), T*> &&
                                   std::is_convertible_v<decltype(std::declval<V>().size()), std::size_t>>* = nullptr>
        ArrayProxy(V const& v)
            : mCount(static_cast<uint32_t>(v.size())), mPtr(v.data())
        {
        }

        [[nodiscard]] uint32_t size() const { return mCount; }

        T const* data() const { return mPtr; }

    private:
        uint32_t mCount;
        T const* mPtr;
    };
} // namespace FS