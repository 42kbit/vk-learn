#include <vk/instance.h>
#include <vk/messenger.h>
#include <isdefined.h>

VkResult __init_vkinstance (struct vkinstance* p,
			    const VkApplicationInfo* info,
			    const VkAllocationCallbacks* cbs,
			    const char * const * enable_exts,
			    guint nexts,
			    const char * const * enable_vlayers,
			    guint nvlayers,
			    struct vkmessenger* msgr)
{
	VkResult result = 0;
	VkDebugUtilsMessengerCreateInfoEXT dm_create_info = { 0 };
	VkInstanceCreateInfo vk_create_info = { 0 };

	if (IS_DEFINED (DEBUG)) {
		dm_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		dm_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
						 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		dm_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
					     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
#ifdef DEBUG
		dm_create_info.pfnUserCallback = vk_debug_callback;
#endif
		dm_create_info.pUserData = NULL;
	}

	vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_create_info.pApplicationInfo = info;
	vk_create_info.enabledExtensionCount = nexts;
	vk_create_info.ppEnabledExtensionNames = enable_exts;

	if (IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		vk_create_info.ppEnabledLayerNames = enable_vlayers;
		vk_create_info.enabledLayerCount = nvlayers;
	}
	else {
		vk_create_info.enabledLayerCount = 0;
	}

#ifdef DEBUG
		vk_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&dm_create_info;
#else
		vk_create_info.pNext = NULL;
#endif

	result = vkCreateInstance (&vk_create_info, cbs, &p->core);
	if (G_UNLIKELY(result != VK_SUCCESS))
		return result;

	if (IS_DEFINED (DEBUG) && msgr) {
		result = init_vkmessenger (msgr, p, &dm_create_info, cbs);
		if (G_UNLIKELY(result != VK_SUCCESS))
			return result;
	}
	return 0;
}