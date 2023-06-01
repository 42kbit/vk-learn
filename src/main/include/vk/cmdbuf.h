#ifndef __H_SRC_MAIN_INCLUDE_VK_CMDBUF_H
#define __H_SRC_MAIN_INCLUDE_VK_CMDBUF_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>

struct vkcmdbuf {
	VkCommandBuffer core;
	
	struct vkldev* ldev;
};

static inline VkResult init_vkcmdbuf (struct vkcmdbuf* p,
				      struct vkldev* ldev,
				      const VkCommandBufferAllocateInfo* ainfo)
{
	p->ldev = ldev;
	return vkAllocateCommandBuffers (ldev->core, ainfo, &p->core);
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_CMDBUF_H */
