#pragma once

#include "memory"
#include "string"
#include "fstream"
#include "unordered_map"
#include "vector"
#include "filesystem"
#include "ranges"
#include "algorithm"
#include "unordered_set"

#include "vma/vk_mem_alloc.h"
#include "vulkan/vulkan.h"
#include "VkBootstrap.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#ifdef FS_RELEASE
#define GLM_INLINE
#endif
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Core/Constants.hpp"
#include "Core/Macros.hpp"
#include "Tools/Log.h"
#include "Tools/ArrayProxy.h"
#include "Tools/Tools.h"