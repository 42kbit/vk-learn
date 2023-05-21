#ifndef __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_CAPS_H
#define __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_CAPS_H

#include <vulkan/vulkan.h>

#include <vk/pdev.h>
#include <vk/surface.h>

struct vkswapchain_caps_khr {
	VkSurfaceCapabilitiesKHR core;
	struct vkpdev* pdev;
};

static inline VkSurfaceCapabilitiesKHR* vkswapchain_caps_khr_core (struct vkswapchain_caps_khr* p)
{
	return &p->core;
}

int init_vkswapchain_caps_khr (struct vkswapchain_caps_khr* dst, 
			       struct vkpdev* pdev,
			       struct vksurface_khr * surface);

#endif /* __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_CAPS_H */
