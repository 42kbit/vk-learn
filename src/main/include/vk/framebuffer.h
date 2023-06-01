#ifndef __H_SRC_MAIN_INCLUDE_VK_FRAMEBUFFER_H
#define __H_SRC_MAIN_INCLUDE_VK_FRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>

struct vkframebuffer {
	VkFramebuffer core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

/*
static inline VkResult init_vkframebuffer (struct vkframebuffer* p,
					   struct vkldev* ldev,
					   const VkFramebufferCreateInfo* cinfo,
					   const VkAllocationCallbacks *alloc_cbs)
{
	p->ldev = ldev;
	p->alloc_cbs = alloc_cbs;
	
	return vkCreateFramebuffer (ldev->core, cinfo, alloc_cbs, &p->core);
}

static inline void term_vkframebuffer (struct vkframebuffer* p)
{
	vkDestroyFramebuffer (p->ldev->core, p->core, p->alloc_cbs);
}
*/

#endif /* __H_SRC_MAIN_INCLUDE_VK_FRAMEBUFFER_H */
