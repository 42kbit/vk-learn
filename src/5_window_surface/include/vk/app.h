#ifndef __H_SRC_INCLUDE_VK_APP_H
#define __H_SRC_INCLUDE_VK_APP_H

#include <GLFW/glfw3.h>

#include <vk/ldev.h>
#include <vk/queue.h>
#include <vk/instance.h>

#include <vk/surface.h>

/* All info needed for rendeing Vulkan Application. */
struct vkapp {
	GLFWwindow*		glfw_window;
	GArray*			exts;

	struct vkinstance	instance;

	GArray*			pdevs;    /* All struct vkpdev   */
	struct vkpdev*		pd_used;  /* Picked VkPhysicalDevice */

	struct vkldev		ld_used;
	
	struct vkqueue		gqueue;   /* Graphics queue */
	
	struct vksurface_khr	surface;

#ifdef __VK_VLAYERS_NEEDED
	GArray*			vlayers;  /* struct vkvlayer* */
#endif  
#ifdef DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif
};

gint init_vkapp (struct vkapp** dst, GError** e);
    
void term_vkapp (struct vkapp* p, GError** e);

#endif /* __H_SRC_INCLUDE_VK_APP_H */
