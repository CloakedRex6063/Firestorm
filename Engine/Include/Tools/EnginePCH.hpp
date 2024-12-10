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
#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"
#include "VkBootstrap.h"
#include "entt/entt.hpp"

extern PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasks;
extern PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCount;
extern PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirect;

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#ifdef FS_RELEASE
#define GLM_INLINE
#endif
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Tools/TypeDefs.hpp"
#include "Tools/Macros.hpp"
#include "Tools/Log.h"
#include "Tools/ArrayProxy.h"
#include "Tools/Tools.h"
#include "Core/Components.h"