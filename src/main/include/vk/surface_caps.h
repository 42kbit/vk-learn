#ifndef __H_SRC_MAIN_INCLUDE_VK_SURFACE_CAPS_H
#define __H_SRC_MAIN_INCLUDE_VK_SURFACE_CAPS_H

#include <vulkan/vulkan.h>

#include <vk/pdev.h>
#include <vk/surface.h>

struct GArray;
typedef struct _GArray GArray;

struct vksurface_caps_khr {
	VkSurfaceCapabilitiesKHR core;
	GArray* sfmts;	  /* VkSurfaceFormatKHR */
	GArray* pmodes;   /* VkPresentModeKHR */
};

static inline VkSurfaceCapabilitiesKHR* vksurface_caps_khr_core (struct vksurface_caps_khr* p)
{
	return &p->core;
}

int init_vksurface_caps_khr (struct vksurface_caps_khr* dst, 
			     struct vkpdev* pdev,
			     struct vksurface_khr * surface);

void term_vksurface_caps_khr (struct vksurface_caps_khr* dst);

static inline guint32 vksurface_caps_khr_sfmts_cnt (struct vkpdev* pdev,
						    struct vksurface_khr* surface)
{
	guint32 nsfmts;
	vkGetPhysicalDeviceSurfaceFormatsKHR (vkpdev_core (pdev), vksurface_khr_core(surface),
					      &nsfmts, NULL);
	return nsfmts;
}

static inline guint32 vksurface_caps_khr_pmodes_cnt (struct vkpdev* pdev,
						     struct vksurface_khr* surface)
{
	guint32 npmodes;
	vkGetPhysicalDeviceSurfacePresentModesKHR (vkpdev_core (pdev), vksurface_khr_core(surface),
					      	   &npmodes, NULL);
	return npmodes;
}



#endif /* __H_SRC_MAIN_INCLUDE_VK_SURFACE_CAPS_H */
