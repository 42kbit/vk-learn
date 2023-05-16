#include <vk/messenger.h>

#include <vk/common.h>
#include <vk/defs.h>

#ifdef DEBUG

static inline const char *
getstr_VkDebugUtilsMessageSeverityFlagBitsEXT (VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity)
{
	switch (msg_severity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return "VRB";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    return "LOG";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return "WRN";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   return "ERR";
		default: return "INV"; /* Invalid */
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT      msg_severity,
		   VkDebugUtilsMessageTypeFlagsEXT	       msg_type,
		   const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		   void* _udata)
{
	const char* severity_str = getstr_VkDebugUtilsMessageSeverityFlagBitsEXT (msg_severity);
	
	g_printerr ("[%s] %s\n", severity_str, callback_data->pMessage);
	return msg_severity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
}

VkResult init_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT* 		  p,
			       		VkInstance		 		  instance,
			       		const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			       		const VkAllocationCallbacks*		  callbacks)
{
	PFN_vkCreateDebugUtilsMessengerEXT func = NULL;
	VKEXT_GETFN(func, vkCreateDebugUtilsMessengerEXT, instance);
	if (!func)
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	return func (instance, create_info, callbacks, p);
}

VkResult term_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT     p,
					VkInstance		     instance,
					const VkAllocationCallbacks* callbacks)
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = NULL;
	VKEXT_GETFN(func, vkDestroyDebugUtilsMessengerEXT, instance);
	if (!func)
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	func (instance, p, callbacks);
	return VK_SUCCESS;
}
#endif

