#include <vk/vk.h>

static inline int __init_vkpdev (struct vkpdev* dst,
				  VkInstance instance)
{
	guint qfamily_props_cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties (dst->pdev, &qfamily_props_cnt, NULL);
	dst->qfamily.props = g_array_sized_new (FALSE, FALSE, sizeof (VkQueueFamilyProperties),
						qfamily_props_cnt);
	vkGetPhysicalDeviceQueueFamilyProperties (dst->pdev, &qfamily_props_cnt,
						  (VkQueueFamilyProperties*) dst->qfamily.props->data);
	g_array_set_size (dst->qfamily.props, qfamily_props_cnt);

	initopt_vkq_gfamily (&dst->qfamily.gfamily);
	for (guint i = 0; i < dst->qfamily.props->len; i++) {
		VkQueueFamilyProperties* iter;
		iter = &g_array_index (dst->qfamily.props, VkQueueFamilyProperties, i);
		if (iter->queueCount & VK_QUEUE_GRAPHICS_BIT) {
			setopt_vkq_gfamily (&dst->qfamily.gfamily, i);
			break;
		}
	}
	
	if (!getopt_vkq_gfamily (&dst->qfamily.gfamily))
		return -ENODEV;

	return 0;
}

#define VK_MAX_PDEVS 32

/* Just in case.
 * Better rewrite if this fails
*/
G_STATIC_ASSERT (sizeof (VkPhysicalDevice) == sizeof (void*));

int get_vkpdevs_from_VkInstance (GArray** dst, VkInstance instance)
{
	VkResult result;
	GArray*  physdevs;
	guint    physdevs_cnt;

	VkPhysicalDevice tmp[VK_MAX_PDEVS];
	
	result = vkEnumeratePhysicalDevices (instance, &physdevs_cnt, NULL);
	g_assert (result == VK_SUCCESS);
	g_assert (physdevs_cnt <= VK_MAX_PDEVS); /* 32 physical devices? we dont suppport fucking hyper-mainframes */

	if (physdevs_cnt == 0)
		return -ENODEV;

	physdevs = g_array_sized_new (FALSE, FALSE, sizeof (struct vkpdev), physdevs_cnt);

	result = vkEnumeratePhysicalDevices (instance, &physdevs_cnt, (VkPhysicalDevice*)tmp);
	g_assert (result == VK_SUCCESS);

	/* Valid copy of array */
	for (guint i = 0; i < physdevs_cnt; i++) {
		struct vkpdev* dst = &g_array_index (physdevs, struct vkpdev, i);
		VkPhysicalDevice src = tmp[i];
		dst->pdev = src;
	}

	g_array_set_size (physdevs, physdevs_cnt);
	
	for (guint i = 0; i < physdevs->len; i++) {
		struct vkpdev* iter = &g_array_index (physdevs, struct vkpdev, i);
		ERET (__init_vkpdev (iter, instance));
	}

	*dst = physdevs;
	return 0;
}

void term_vkpdevs (GArray* p) {
	for (guint i = 0; i < p->len; i++) {
		struct vkpdev* iter = &g_array_index (p, struct vkpdev, i);
		g_array_free (iter->qfamily.props, TRUE);
	}
	g_array_free (p, TRUE);
}