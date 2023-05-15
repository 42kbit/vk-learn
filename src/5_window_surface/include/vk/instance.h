#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_INSTANCE_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_INSTANCE_H

#include <vulkan/vulkan.h>

struct vkinstance {
	VkInstance instance;
};

static inline VkInstance vkinstance_core (struct vkinstance* p)
{
	return p->instance;
}

static inline VkResult init_vkinstance (struct vkinstance* p,
					const VkInstanceCreateInfo* info,
					const VkAllocationCallbacks* cbs)
{
	return vkCreateInstance (info, cbs, &p->instance);
}

#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_INSTANCE_H */
