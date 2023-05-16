#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_MESSENGER_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_MESSENGER_H

#include <vk/instance.h>

VkResult init_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT* 		  p,
			                VkInstance		 		  instance,
			                const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			                const VkAllocationCallbacks*		  callbacks);

VkResult term_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT     p,
			        	VkInstance		     instance,
			        	const VkAllocationCallbacks* callbacks);

struct vkmessenger {
	VkDebugUtilsMessengerEXT messenger;
	struct vkinstance* instance;
};

static inline VkDebugUtilsMessengerEXT vkmessenger_core (struct vkmessenger* p)
{
	return p->messenger;
}

static inline VkResult init_vkmessenger (struct vkmessenger* dst,
					 struct vkinstance* _instance,
					 const VkDebugUtilsMessengerCreateInfoEXT* create_info,
					 const VkAllocationCallbacks* callbacks)
{
	VkInstance instance = vkinstance_core (_instance);
	dst->instance = _instance;
	return init_VkDebugUtilsMessengerEXT (&dst->messenger, instance, create_info, callbacks);
}

static inline VkResult term_vkmessenger (struct vkmessenger* p,
					 const VkAllocationCallbacks* cbs)
{
	VkDebugUtilsMessengerEXT messenger = vkmessenger_core (p);
	VkInstance instance = vkinstance_core (p->instance);

	return term_VkDebugUtilsMessengerEXT (messenger, instance, cbs);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT   msg_severity,
		VkDebugUtilsMessageTypeFlagsEXT	       	    message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* _udata);

#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_MESSENGER_H */
