#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_SURFACE_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_SURFACE_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN 1
#endif

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vk/instance.h>

struct vksurface_khr {
	VkSurfaceKHR surface;
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

	return glfwCreateWindowSurface (instance, win, cb, &dst->surface);
}


#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_SURFACE_H */
