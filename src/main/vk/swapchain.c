#include <vk/pdev.h>
#include <vk/ldev.h>
#include <vk/surface.h>
#include <vk/surface_caps.h>
#include <vk/swapchain.h>
#include <vk/image.h>

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

G_STATIC_ASSERT (sizeof (VkImage) == sizeof (struct vkimage));
static int __init_swapchain_images (struct vkswapchain_khr* dst)
{
	VkResult res;
	guint32 nimgs;
	res = vkGetSwapchainImagesKHR (dst->ldev->core, dst->core, &nimgs, NULL);
	if (res != VK_SUCCESS)
		return -1;
	dst->images = g_array_sized_new (FALSE, FALSE, sizeof (struct vkimage), nimgs);
	g_array_set_size (dst->images, nimgs);
	res = vkGetSwapchainImagesKHR (dst->ldev->core, dst->core,
				       &nimgs, (VkImage*)dst->images->data);
	if (res != VK_SUCCESS)
		return -1;
	return 0;
}

G_STATIC_ASSERT (sizeof (VkImageView) == sizeof (struct vkimage_view));
static int __init_swapchain_image_views (struct vkswapchain_khr* dst)
{
	VkResult result = VK_SUCCESS;
	guint32 nimgs = dst->images->len;

	dst->image_views = g_array_sized_new (FALSE, FALSE, sizeof (struct vkimage_view), nimgs);
	g_array_set_size (dst->image_views, nimgs);

	for (guint i = 0; i < dst->images->len; i++) {
		VkImageViewCreateInfo create_info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = dst->sfmt.format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.image = g_array_index (dst->images, struct vkimage, i).core
		};
		
		result = vkCreateImageView (dst->ldev->core, &create_info,
					    NULL, &g_array_index (dst->image_views, struct vkimage_view, i).core);
		if (result != VK_SUCCESS)
			return -1;
	}
	return 0;
}

static int __term_swapchain_image_views (struct vkswapchain_khr* dst)
{
	for (guint i = 0; i < dst->image_views->len; i++) {
		VkImageView view = g_array_index (dst->image_views, VkImageView, i);
		vkDestroyImageView (dst->ldev->core, view, NULL);
	}
	return 0;
}

/*
static int __term_swapchain_images (struct vkswapchain_khr* dst)
{
	for (guint i = 0; i < dst->images->len; i++) {
		VkImage img = g_array_index (dst->images, VkImage, i);
		vkDestroyImage (dst->ldev->core, img, NULL);
	}
	return 0;
}
*/

int init_vkswapchain_khr (struct vkswapchain_khr* dst,
			  struct vkpdev* pdev,
			  struct vkldev* ldev,
			  struct vksurface_khr* surface)
{
	GE_ZEROTYPE (dst);

	dst->ldev = ldev;
	dst->pdev = pdev;
	dst->surface = surface;

	int ecode = 0;
	/* Maybe wastefull to call malloc for 1 function pass, but i doubt that
	 * init_vkswapchain_khr will be called more than one (maybe twice). */
	init_vksurface_caps_khr (&dst->surface_caps, pdev, surface);
	struct vksurface_caps_khr* caps = &dst->surface_caps;
	if (__swapchain_pick_format (dst, caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__swapchain_pick_pmode (dst, caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__swapchain_pick_glfw_fbsize (dst, surface->win, caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__init_swapchain_core (dst, pdev, ldev, surface, caps) < 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__init_swapchain_images (dst) != 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
	if (__init_swapchain_image_views (dst) != 0) {
		ecode = -EINVAL;
		goto free_exit;
	}
free_exit:;
	return ecode;
}

int term_vkswapchain_khr (struct vkswapchain_khr* p)
{
	vkDestroySwapchainKHR (p->ldev->core, p->core, NULL);
	term_vksurface_caps_khr (&p->surface_caps);

	//__term_swapchain_images (p);
	__term_swapchain_image_views (p);

	g_array_free (p->images, TRUE);
	g_array_free (p->image_views, TRUE);

	return 0;
}