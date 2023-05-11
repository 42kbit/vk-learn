#include <vk/vk.h>

/* Same thing with vlayers.
 * to see why static assert is used, check vlayers.c
*/

G_STATIC_ASSERT (sizeof (struct vkpdev) == sizeof (VkPhysicalDevice));
GArray* vk_get_pdevs (VkInstance instance)
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