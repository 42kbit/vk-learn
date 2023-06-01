#ifndef __H_SRC_MAIN_INCLUDE_VK_SEMAPHORE_H
#define __H_SRC_MAIN_INCLUDE_VK_SEMAPHORE_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>
#include <vk/defs.h>

struct vksemaphore {
	VkSemaphore core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

VKEXT_GEN_LDEV_WRAPPER_HEADER_FUNCS (vksemaphore, Semaphore);

#endif /* __H_SRC_MAIN_INCLUDE_VK_SEMAPHORE_H */
