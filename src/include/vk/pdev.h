#ifndef __H_SRC_INCLUDE_VK_PDEV_H
#define __H_SRC_INCLUDE_VK_PDEV_H

#include <vulkan/vulkan.h>

struct vkpdev {
	VkPhysicalDevice pdev;
};
GArray* vk_get_pdevs (VkInstance instance);

#endif /* __H_SRC_INCLUDE_VK_PDEV_H */
