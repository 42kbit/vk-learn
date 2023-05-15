#ifndef __H_SRC_INCLUDE_VK_INSTANCE_H
#define __H_SRC_INCLUDE_VK_INSTANCE_H

#include <vulkan/vulkan.h>

struct vkinstance {
	VkInstance instance;
};

static inline VkInstance vkinstance_core (struct vkinstance* p)
{
	return p->instance;
}

#endif /* __H_SRC_INCLUDE_VK_INSTANCE_H */
