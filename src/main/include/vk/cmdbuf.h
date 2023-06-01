#ifndef __H_SRC_MAIN_INCLUDE_VK_CMDBUF_H
#define __H_SRC_MAIN_INCLUDE_VK_CMDBUF_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>

struct vkcmdbuf {
	VkCommandBuffer core;
	
	struct vkldev* ldev;
};

static inline VkResult init_vkcmdbufs (struct vkcmdbuf* p,
				       struct vkldev* ldev,
				       const VkCommandBufferAllocateInfo* ainfo)
{
	struct vkcmdbuf* iter;
	for (uint32_t i = 0; i < ainfo->commandBufferCount; i++) {
		iter = &p[i];
		iter->ldev = ldev;
		vkAllocateCommandBuffers (ldev->core, ainfo, &iter->core);
	}
	return VK_SUCCESS;
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_CMDBUF_H */
