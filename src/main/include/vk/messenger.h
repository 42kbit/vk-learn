#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_MESSENGER_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_MESSENGER_H

#include <vk/instance.h>

#ifdef DEBUG

VkResult init_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT* 		  p,
			                VkInstance		 		  instance,
			                const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			                const VkAllocationCallbacks*		  callbacks);

VkResult term_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT     p,
			        	VkInstance		     instance,
			        	const VkAllocationCallbacks* callbacks);

struct vkmessenger {
	VkDebugUtilsMessengerEXT core;
	
	struct vkinstance* instance;
	const VkAllocationCallbacks* alloc_cbs;
};

static inline VkDebugUtilsMessengerEXT vkmessenger_core (struct vkmessenger* p)
{
	return p->core;
}

static inline VkResult init_vkmessenger (struct vkmessenger* dst,
					 struct vkinstance* instance,
					 const VkDebugUtilsMessengerCreateInfoEXT* create_info,
					 const VkAllocationCallbacks* callbacks)
{
	dst->instance = instance;
	dst->alloc_cbs = callbacks;
	return init_VkDebugUtilsMessengerEXT (&dst->core, instance->core, create_info, callbacks);
}

static inline VkResult term_vkmessenger (struct vkmessenger* p)
{
	return term_VkDebugUtilsMessengerEXT (p->core, p->instance->core, p->alloc_cbs);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT   msg_severity,
		VkDebugUtilsMessageTypeFlagsEXT	       	    message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* _udata);

#endif /* DEBUG */

#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_MESSENGER_H */