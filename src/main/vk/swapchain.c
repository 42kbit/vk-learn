#include <vk/pdev.h>
#include <vk/ldev.h>
#include <vk/surface.h>
#include <vk/surface_caps.h>
#include <vk/swapchain.h>

static int __swapchain_pick_format (struct vkswapchain_khr* dst,
				    struct vksurface_caps_khr* caps)
{
	VkSurfaceFormatKHR* iter;
	for (guint i = 0 ; i < caps->sfmts->len; i++) {
		iter = &g_array_index (caps->sfmts, VkSurfaceFormatKHR, i);
		if (iter->format == VK_FORMAT_B8G8R8_SRGB &&
		    iter->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			dst->sfmt = *iter;
		}
	}
	dst->sfmt = g_array_index (caps->sfmts, VkSurfaceFormatKHR, 0);
	return 0;
}

static inline int __swapchain_pick_pmode (struct vkswapchain_khr* dst,
				   	  struct vksurface_caps_khr* caps)
{
	/* This code may check if we want some specific presentation queue,
	 * but since we dont care for now, lets just pick guaranteed one, which is
	 * VK_PRESENT_MODE_FIFO_KHR */
	dst->pmode = VK_PRESENT_MODE_FIFO_KHR;
	return 0;
}

static int __swapchain_pick_glfw_fbsize (struct vkswapchain_khr* dst,
					 GLFWwindow* win,
					 struct vksurface_caps_khr* caps)
{
	int width, height;
	
	if (caps->core.currentExtent.width != INT_MAX) {
		dst->res = caps->core.currentExtent;
		return 0;
	}

	glfwGetFramebufferSize (win, &width, &height);
	
	VkExtent2D real_extent = {
		.width  = CLAMP (width,
				 (int)caps->core.minImageExtent.width,
				 (int)caps->core.maxImageExtent.width),
		.height = CLAMP (height,
		                 (int)caps->core.minImageExtent.height,
                                 (int)caps->core.maxImageExtent.height),
	};
	
	dst->res = real_extent;

	return 0;
}

static int __init_swapchain_core (struct vkswapchain_khr* dst,
				  struct vkpdev* pdev,
			  	  struct vkldev* ldev,
				  struct vksurface_khr* surface,
				  struct vksurface_caps_khr* caps)
{
	int gidx, pidx;
	guint32 max_image_cnt = caps->core.maxImageCount;
	guint32 image_cnt = caps->core.minImageCount + 1;
	VkResult result;
	
	/* Zero means no limit */
	if (max_image_cnt > 0 && image_cnt > max_image_cnt) {
		image_cnt = caps->core.minImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface->core,
		.minImageCount = image_cnt,
		.imageFormat = dst->sfmt.format,
		.imageColorSpace = dst->sfmt.colorSpace,
		.imageExtent = dst->res,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = caps->core.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = dst->pmode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};
	
	gidx = get_vkpdev_gfamily_idx (pdev);
	pidx = get_vkpdev_pfamily_idx (pdev);
	
	guint qf_idxs [] = {gidx, pidx};
	if (gidx != pidx) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = G_N_ELEMENTS (qf_idxs);
		create_info.pQueueFamilyIndices = qf_idxs;
	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = NULL;
	}
	
	result = vkCreateSwapchainKHR (ldev->core, &create_info, NULL, &dst->core);
	if (result != VK_SUCCESS) {
		return -1;
	}

	return 0;
}

int init_vkswapchain_khr (struct vkswapchain_khr* dst,
			  struct vkpdev* pdev,
			  struct vkldev* ldev,
			  struct vksurface_khr* surface)
{
	GE_ZEROTYPE (dst);
	dst->ldev = ldev;

	struct vksurface_caps_khr caps;
	int ecode = 0;
	/* Maybe wastefull to call malloc for 1 function pass, but i doubt that
	 * init_vkswapchain_khr will be called more than one (maybe twice). */
	init_vksurface_caps_khr (&caps, pdev, surface);
	if (__swapchain_pick_format (dst, &caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__swapchain_pick_pmode (dst, &caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__swapchain_pick_glfw_fbsize (dst, surface->win, &caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__init_swapchain_core (dst, pdev, ldev, surface, &caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
free_exit:;
	term_vksurface_caps_khr (&caps);
	return ecode;
}