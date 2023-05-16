#include <vk/instance.h>
#include <vk/messenger.h>
#include <isdefined.h>

VkResult init_vkinstance (struct vkinstance* p,
			  const VkApplicationInfo* info,
			  const VkAllocationCallbacks* cbs,
			  GArray* enable_exts,
			  struct vkmessenger* msgr)
{
	VkResult result;

	p->callbacks = cbs;

#ifdef DEBUG
	VkDebugUtilsMessengerCreateInfoEXT dm_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
			       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = vk_debug_callback,
		.pUserData = NULL
	};
#endif

	VkInstanceCreateInfo vk_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = info,
		.enabledExtensionCount = enable_exts->len,
		.ppEnabledExtensionNames = (const char * const *)enable_exts->data,
#ifndef __VK_VLAYERS_NEEDED
		.enabledLayerCount = 0,
#else
		.ppEnabledLayerNames = vkapp_required_vlayers,
		.enabledLayerCount = count_ztarray_len ((void**)vkapp_required_vlayers),
#endif
#ifdef DEBUG
		.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&dm_create_info
#else
		.pNext = NULL
#endif
	};

	result = vkCreateInstance (&vk_create_info, cbs, &p->instance);
	if (G_UNLIKELY(result != VK_SUCCESS))
		return result;

#ifdef DEBUG
	if (msgr) {
		result = init_vkmessenger (msgr, p, &dm_create_info, cbs);
		if (G_UNLIKELY(result != VK_SUCCESS))
			return result;
	}
#endif
	return 0;
}

/*
VkResult init_vkinstance (struct vkinstance* p,
			  const VkApplicationInfo* info,
			  const VkAllocationCallbacks* cbs,
			  GArray* enable_exts,
			  struct vkmessenger* msgr)
{
	VkResult result;
	VkDebugUtilsMessengerCreateInfoEXT dm_create_info;
	VkInstanceCreateInfo vk_create_info;

	p->callbacks = cbs;

	if (IS_DEFINED (DEBUG)) {
		dm_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		dm_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
						 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		dm_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
					     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		dm_create_info.pfnUserCallback = vk_debug_callback;
		dm_create_info.pUserData = NULL;
	}

	vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_create_info.pApplicationInfo = info;
	vk_create_info.enabledExtensionCount = enable_exts->len;
	vk_create_info.ppEnabledExtensionNames = (const char * const *)enable_exts->data;

	if (IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		vk_create_info.ppEnabledLayerNames = vkapp_required_vlayers;
		vk_create_info.enabledLayerCount = count_ztarray_len ((void**)vkapp_required_vlayers);
	}
	else {
		vk_create_info.enabledLayerCount = 0;
	}

	if (IS_DEFINED (DEBUG))
		vk_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&dm_create_info;
	else
		vk_create_info.pNext = NULL;

	result = vkCreateInstance (&vk_create_info, cbs, &p->instance);
	if (G_UNLIKELY(result != VK_SUCCESS))
		return result;

	if (IS_DEFINED (DEBUG) && msgr) {
		result = init_vkmessenger (msgr, p, &dm_create_info, cbs);
		if (G_UNLIKELY(result != VK_SUCCESS))
			return result;
	}
	return 0;
}
*/


