#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_SURFACE_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_SURFACE_H

#include <vulkan/vulkan.h>

#include <vk/instance.h>

struct vksurface_khr {
	VkSurfaceKHR surface;

	GLFWwindow* win;
	struct vkinstance* instance;
	const VkAllocationCallbacks* callbacks;
};

static inline VkSurfaceKHR vksurface_khr_core (struct vksurface_khr* p)
{
	return p->surface;
}

static inline VkResult init_vksurface_khr (struct vksurface_khr* dst,
					   struct vkinstance* _instance,
					   GLFWwindow* win,
					   const VkAllocationCallbacks* cb)
{
	VkInstance instance = vkinstance_core (_instance);
	
	dst->win = win;
	dst->callbacks = cb;
	dst->instance = _instance;

	return glfwCreateWindowSurface (instance, win, cb, &dst->surface);
}

static inline void term_vksurface_khr (struct vksurface_khr* p)
{
	VkInstance instance = vkinstance_core (p->instance);
	VkSurfaceKHR surface = vksurface_khr_core (p);

	vkDestroySurfaceKHR (instance, surface, p->callbacks);
}

#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_SURFACE_H */
