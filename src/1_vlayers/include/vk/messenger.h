#ifndef __H_SRC_INCLUDE_VK_MESSENGER_H
#define __H_SRC_INCLUDE_VK_MESSENGER_H

#include <vk/vk.h>

VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT   msg_severity,
		VkDebugUtilsMessageTypeFlagsEXT	       	    message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* _udata);

VkResult init_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT* 		  p,
			                VkInstance		 		  instance,
			                const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			                const VkAllocationCallbacks*		  callbacks);

VkResult term_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT     p,
			        	VkInstance		     instance,
			        	const VkAllocationCallbacks* callbacks);

#endif /* __H_SRC_INCLUDE_VK_MESSENGER_H */
