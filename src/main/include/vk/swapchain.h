#ifndef __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H
#define __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <glib.h>

#include <vk/pdev.h>
#include <vk/ldev.h>
#include <vk/surface.h>
#include <vk/surface_caps.h>

#include <vk/image.h>
#include <vk/image_view.h>

struct vkswapchain_khr {
	VkSwapchainKHR core;
	GArray* images; /* struct vkimage */
	GArray* image_views; /* struct vkimage_view */

	struct vkpdev* pdev;
	struct vkldev* ldev;
	struct vksurface_khr* surface;

	struct vksurface_caps_khr surface_caps;
	
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

int term_vkswapchain_khr (struct vkswapchain_khr* p);

#endif /* __H_SRC_MAIN_INCLUDE_VK_SWAPCHAIN_H */
