#ifndef __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H
#define __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <glib.h>

struct vkswapchain_khr {
	VkSwapchainKHR core;
};

static inline VkSwapchainKHR vkswapchain_khr_core (struct vkswapchain_khr* p)
{
	return p->core;
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H */
