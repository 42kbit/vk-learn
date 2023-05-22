#include <vk/surface.h>
#include <glibext/gemacro.h>

VkResult init_vksurface_khr (struct vksurface_khr* dst,
			     struct vkinstance* _instance,
			     GLFWwindow* win,
			     const VkAllocationCallbacks* cb)
{
	int retcode;
	VkResult result;
	VkInstance instance = vkinstance_core (_instance);

	GE_ZEROTYPE (dst);
	
	dst->win = win;
	dst->callbacks = cb;
	dst->instance = _instance;

	result = glfwCreateWindowSurface (instance, win, cb, &dst->core);
	if (result != VK_SUCCESS)
		return result;
	return VK_SUCCESS;
}

void term_vksurface_khr (struct vksurface_khr* p)
{
	VkInstance instance = vkinstance_core (p->instance);
	VkSurfaceKHR surface = vksurface_khr_core (p);

	vkDestroySurfaceKHR (instance, surface, p->callbacks);
}
