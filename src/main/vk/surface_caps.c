#include <vk/surface_caps.h>
#include <glibext/glibext.h>

#include <vk/pdev.h>
#include <vk/surface.h>

static int __init_vksurface_caps_khr_core (struct vksurface_caps_khr* dst,
					   struct vkpdev* _pdev,
					   struct vksurface_khr* _surface)
{
	VkPhysicalDevice pdev_core = vkpdev_core (_pdev);
	VkSurfaceKHR surface_core  = vksurface_khr_core (_surface);
	VkSurfaceCapabilitiesKHR * caps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR (pdev_core, surface_core, &dst->core);
	return 0;
}

static int __init_vksurface_caps_khr_sfmts (struct vksurface_caps_khr* dst,
					    struct vkpdev* _pdev,
					    struct vksurface_khr* _surface)
{
	VkPhysicalDevice pdev = vkpdev_core (_pdev);
	VkSurfaceKHR surface = vksurface_khr_core (_surface);
	
	guint32 nsfmts;
	nsfmts = vksurface_caps_khr_sfmts_cnt (_pdev, _surface);
	if (nsfmts == 0)
		return 0;
	dst->sfmts = g_array_sized_new (FALSE, FALSE, sizeof (VkSurfaceFormatKHR), nsfmts);
	vkGetPhysicalDeviceSurfaceFormatsKHR (pdev, surface, &nsfmts,
					      (VkSurfaceFormatKHR*) dst->sfmts->data);
	g_array_set_size (dst->sfmts, nsfmts);
	return 0;
}

static int __init_vksurface_caps_khr_pmodes (struct vksurface_caps_khr* dst,
					     struct vkpdev* _pdev,
					     struct vksurface_khr* _surface)
{
	VkPhysicalDevice pdev = vkpdev_core (_pdev);
	VkSurfaceKHR surface = vksurface_khr_core (_surface);
	
	guint32 npmodes;

	npmodes = vksurface_caps_khr_pmodes_cnt (_pdev, _surface);
	if (npmodes == 0)
		return 0;
	dst->pmodes = g_array_sized_new (FALSE, FALSE, sizeof (VkSurfaceFormatKHR), npmodes);
	vkGetPhysicalDeviceSurfacePresentModesKHR (pdev, surface, &npmodes,
					      	   (VkPresentModeKHR*) dst->pmodes->data);
	g_array_set_size (dst->pmodes, npmodes);
	return 0;
}

int init_vksurface_caps_khr (struct vksurface_caps_khr* dst, 
			     struct vkpdev* pdev,
			     struct vksurface_khr * surface)
{
	GE_ERET (__init_vksurface_caps_khr_core	    (dst, pdev, surface));
	GE_ERET (__init_vksurface_caps_khr_sfmts    (dst, pdev, surface));
	GE_ERET (__init_vksurface_caps_khr_pmodes   (dst, pdev, surface));
	return 0;
}

void term_vksurface_caps_khr (struct vksurface_caps_khr* dst)
{
	g_array_free (dst->sfmts, TRUE);
	g_array_free (dst->pmodes, TRUE);
}