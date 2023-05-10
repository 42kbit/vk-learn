#include <vk/vk.h>

GArray*
vk_get_physdevs (VkInstance instance)
{
	VkResult result;
	GArray*  physdevs;
	guint    physdevs_cnt;
	
	result = vkEnumeratePhysicalDevices (instance, &physdevs_cnt, NULL);
	g_assert (result == VK_SUCCESS);

	if (physdevs_cnt == 0)
		return NULL;

	physdevs = g_array_sized_new (FALSE, FALSE, sizeof (VkPhysicalDevice), physdevs_cnt);
	result = vkEnumeratePhysicalDevices (instance, &physdevs_cnt, (VkPhysicalDevice*)physdevs->data);
	g_assert (result == VK_SUCCESS);
	g_array_set_size (physdevs, physdevs_cnt);
	return physdevs;
}

GArray*
vk_get_required_ext (void)
{
	GArray* p;
	const char** glfw_exts;
	const char * const debug_ext = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	guint glfw_n;
	guint nexts;

	glfw_exts = glfwGetRequiredInstanceExtensions (&glfw_n);
	nexts = glfw_n;

#ifdef DEBUG
	nexts += 1; /* VK_EXT_DEBUG_UTILS_EXTENSION_NAME */
#endif

	p = g_array_sized_new (FALSE, TRUE, sizeof (const char*), nexts);
	g_array_append_vals (p, glfw_exts, glfw_n);

#ifdef DEBUG
	g_array_append_val  (p, debug_ext);
#endif
	return p;
}

GArray*
vk_get_queue_family_props (VkPhysicalDevice physdev)
{
	GArray* p;
	guint nfamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties (physdev,
						  &nfamilies,
						  NULL);
	if (nfamilies == 0)
		return NULL;
	p = g_array_sized_new (FALSE, FALSE, sizeof (VkQueueFamilyProperties), nfamilies);
	g_array_set_size (p, nfamilies);
	vkGetPhysicalDeviceQueueFamilyProperties (physdev, &nfamilies,
						  (VkQueueFamilyProperties*) p->data);
	return p;
}