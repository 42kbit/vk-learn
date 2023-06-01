#ifndef __H_SRC_MAIN_INCLUDE_VK_SURFACE_H
#define __H_SRC_MAIN_INCLUDE_VK_SURFACE_H

#include <GLFW/glfw3.h>

#include <vk/instance.h>

struct vksurface_khr {
	VkSurfaceKHR core;
	GLFWwindow* win;
	struct vkinstance* instance;
	const VkAllocationCallbacks* callbacks;
};

static inline VkSurfaceKHR vksurface_khr_core (struct vksurface_khr* p)
{
	return p->core;
}

VkResult init_vksurface_khr (struct vksurface_khr* dst,
			     struct vkinstance* _instance,
			     GLFWwindow* win,
			     const VkAllocationCallbacks* cb);

static inline void term_vksurface_khr (struct vksurface_khr* p)
{
	vkDestroySurfaceKHR (p->instance->core, p->core, p->callbacks);
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_SURFACE_H */
