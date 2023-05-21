
#include <vk/swapchain_caps.h>

int init_vkswapchain_caps_khr (struct vkswapchain_caps_khr* dst, 
			       struct vkpdev* pdev,
			       struct vksurface_khr * surface)
{
	VkPhysicalDevice pdev_core = vkpdev_core (pdev);
	VkSurfaceKHR surface_core  = vksurface_khr_core (surface);
	VkSurfaceCapabilitiesKHR * caps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR (pdev_core, surface_core, &dst->core);
	caps = vkswapchain_caps_khr_core (dst);
	g_print ("%d, %d\n", caps->maxImageExtent.width, caps->maxImageExtent.width);
	return 0;
}