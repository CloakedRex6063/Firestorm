#pragma once

#define NON_COPYABLE(ClassName)           \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define NON_MOVABLE(ClassName)       \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define MOVABLE(ClassName)                     \
    ClassName(ClassName&&) noexcept = default; \
    ClassName& operator=(ClassName&&) noexcept = default;

#define UNDERLYING(Underlying, Name) \
    operator Underlying() const { return m##Name; }

#define ASSERT(Result) assert(Result != 0);

#define VK_ASSERT(Result) assert(Result == 0);
#define VK_PIPELINE()                                        \
    operator VkPipeline() const { return mPipelineBuilder; } \
    VkPipelineLayout GetLayout() { return mPipelineBuilder.GetLayout(); }