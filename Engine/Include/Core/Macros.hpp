#pragma once

#define NON_COPYABLE(ClassName)                                                                                        \
    ClassName(const ClassName&) = delete;                                                                              \
    ClassName& operator=(const ClassName&) = delete;

#define NON_MOVABLE(ClassName)                                                                                         \
    ClassName(ClassName&&) = delete;                                                                                   \
    ClassName& operator=(ClassName&&) = delete;

#define MOVABLE(ClassName)                                                                                             \
    ClassName(ClassName&&) noexcept = default;                                                                         \
    ClassName& operator=(ClassName&&) noexcept = default;
