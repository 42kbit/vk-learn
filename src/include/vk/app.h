#ifndef __H_SRC_INCLUDE_VK_APP_H
#define __H_SRC_INCLUDE_VK_APP_H

#include <vk/vk.h>

struct eguint {
	guint	  value;
	GOptional opt;
};

static inline
G_OPTIONAL_GENCB (struct eguint,
		  vkq_gfamily_set_cb,
		  value,
		  opt);

/* All info needed for rendeing Vulkan Application. */
struct vkapp {
	GLFWwindow*		 glfw_window;
	GArray*			 exts;
	VkInstance 		 instance;
	GArray*			 physdevs; /* All VkPhysicalDevices   */
	VkPhysicalDevice	 pd_used;  /* Picked VkPhysicalDevice */

	GArray*       		 qprops_all;
	struct eguint 		 gfamily_idx;
#ifdef __VK_VLAYERS_NEEDED
	GArray*			 vlayers;  /* struct vkvlayer* */
#endif
#ifdef DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif
};

gint init_vkapp (struct vkapp** dst, GError** e);

void term_vkapp (struct vkapp* p, GError** e);

#endif /* __H_SRC_INCLUDE_VK_APP_H */
