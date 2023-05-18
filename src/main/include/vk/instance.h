#ifndef __H_SRC_MAIN_INCLUDE_VK_INSTANCE_H
#define __H_SRC_MAIN_INCLUDE_VK_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vk/vlayers.h>
#include <ztarray.h>

struct vkinstance {
	VkInstance instance;
	const VkAllocationCallbacks* callbacks;
};

static inline VkInstance vkinstance_core (struct vkinstance* p)
{
	return p->instance;
}

struct vkmessenger;

VkResult init_vkinstance (struct vkinstance* p,
			  const VkApplicationInfo* info,
			  const VkAllocationCallbacks* cbs,
			  GArray* enable_exts,
			  struct vkmessenger* msgr);

static inline void term_vkinstance (struct vkinstance* p)
{
	return vkDestroyInstance (vkinstance_core (p), p->callbacks);
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_INSTANCE_H */
