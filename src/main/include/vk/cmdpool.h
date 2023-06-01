#ifndef __H_SRC_MAIN_INCLUDE_VK_CMDPOOL_H
#define __H_SRC_MAIN_INCLUDE_VK_CMDPOOL_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>
#include <vk/defs.h>

struct vkcmdpool {
	VkCommandPool core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

VKEXT_GEN_LDEV_WRAPPER_HEADER_FUNCS (vkcmdpool, CommandPool);

#endif /* __H_SRC_MAIN_INCLUDE_VK_CMDPOOL_H */
