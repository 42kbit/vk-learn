#ifndef __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H
#define __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <glib.h>
#include <vk/ldev.h>

struct vkpdev;
struct vksurface_khr;

struct vkswapchain_khr {
	struct vkldev* ldev;

	VkSwapchainKHR core;
	
	VkSurfaceFormatKHR sfmt;
	VkPresentModeKHR   pmode;
	VkExtent2D	   res;
};

static inline VkSwapchainKHR vkswapchain_khr_core (struct vkswapchain_khr* p)
{
	return p->core;
}

int init_vkswapchain_khr (struct vkswapchain_khr* dst,
			  struct vkpdev* pdev,
			  struct vkldev* ldev,
			  struct vksurface_khr* surface);

static inline int term_vkswapchain_khr (struct vkswapchain_khr* p)
{
	vkDestroySwapchainKHR (p->ldev->core, p->core, NULL);
	return 0;
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H */
