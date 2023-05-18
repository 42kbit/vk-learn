#include <vulkan/vulkan.h>
#include <vk/pdev.h>

#include <vk/common.h>
#include <vk/defs.h>

static int __vkpdev_load_eprops (struct vkpdev* p)
{
	/* VkExtensionProperties */
	guint32 nexts;
	VkPhysicalDevice p_core = vkpdev_core (p);
	vkEnumerateDeviceExtensionProperties (p_core, NULL, &nexts, NULL);

	p->eprops = g_array_sized_new (FALSE, FALSE, sizeof (VkExtensionProperties), nexts);
	if (G_UNLIKELY (!p->eprops))
		return -ENOMEM;
	vkEnumerateDeviceExtensionProperties (p_core, NULL, &nexts, (VkExtensionProperties*)p->eprops->data);
	g_array_set_size (p->eprops, nexts);
	return 0;
}

static int __init_vkpdev_gfamily (struct vkpdev* dst)
{
	initopt_vkq_gfamily (&dst->qfamily.gfamily);

	/* Seek graphics family queue index */
	for (guint i = 0; i < dst->qfamily.props->len; i++) {
		VkQueueFamilyProperties* iter;
		iter = &g_array_index (dst->qfamily.props, VkQueueFamilyProperties, i);
		if (iter->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			setopt_vkq_gfamily (&dst->qfamily.gfamily, i);
			break;
		}
	}

	if (getopt_vkq_gfamily (&dst->qfamily.gfamily) == FALSE)
		return -ENODEV;
	return 0;
}

static int __init_vkpdev_pfamily (struct vkpdev* dst,
				 	 struct vkinstance* _instance,
					 struct vksurface_khr* _surface)
{
	VkResult result = VK_SUCCESS;
	VkBool32 is_suitable = VK_FALSE;

	initopt_vkq_pfamily (&dst->qfamily.pfamily);

	/* Seek presentation family queue index */
	for (guint i = 0; i < dst->qfamily.props->len; i++) {
		VkBool32 has_support = VK_FALSE;
		result = vkGetPhysicalDeviceSurfaceSupportKHR (vkpdev_core (dst), i,
							       vksurface_khr_core (_surface),
							       &is_suitable);
		g_assert (result == VK_SUCCESS);
		if (is_suitable) {
			setopt_vkq_pfamily (&dst->qfamily.pfamily, i);
			break;
		}
	}

	if (getopt_vkq_pfamily (&dst->qfamily.pfamily) == FALSE)
		return -ENODEV;

	return 0;
}

static int __init_vkpdev_queues (struct vkpdev* dst,
				 	struct vkinstance* _instance,
					struct vksurface_khr* _surface)
{
	guint qfamily_props_cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties (dst->pdev, &qfamily_props_cnt, NULL);
	dst->qfamily.props = g_array_sized_new (FALSE, FALSE, sizeof (VkQueueFamilyProperties),
						qfamily_props_cnt);
	vkGetPhysicalDeviceQueueFamilyProperties (dst->pdev, &qfamily_props_cnt,
						  (VkQueueFamilyProperties*) dst->qfamily.props->data);
	g_array_set_size (dst->qfamily.props, qfamily_props_cnt);
	
	ERET(__init_vkpdev_gfamily (dst));
	ERET(__init_vkpdev_pfamily (dst, _instance, _surface));

	return 0;
}

#define VK_MAX_PDEVS 32

/* Just in case.
 * Better rewrite if this fails
*/
G_STATIC_ASSERT (sizeof (VkPhysicalDevice) == sizeof (void*));

int __get_vkpdevs_from_VkInstance (GArray** dst, VkInstance instance)
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
		dst->eprops = NULL;
		initopt_vkq_gfamily (&dst->qfamily.gfamily);
		initopt_vkq_pfamily (&dst->qfamily.pfamily);
	}

	g_array_set_size (physdevs, physdevs_cnt);

	*dst = physdevs;
	return 0;
}

int init_vkpdevs (GArray** dst, struct vkinstance* instance,
		  struct vksurface_khr* surface)
{
	ERET (__get_vkpdevs_from_VkInstance (dst, vkinstance_core (instance)));
	GArray* pdevs = *dst;
	
	for (guint i = 0; i < pdevs->len; i++) {
		struct vkpdev* iter = &g_array_index (pdevs, struct vkpdev, i);
		ERET (__init_vkpdev_queues (iter, instance, surface));
	}
	return 0;
}

void term_vkpdevs (GArray* p) {
	for (guint i = 0; i < p->len; i++) {
		struct vkpdev* iter = &g_array_index (p, struct vkpdev, i);
		g_array_free (iter->qfamily.props, TRUE);
	}
	g_array_free (p, TRUE);
}


gboolean vkpdev_has_ext (struct vkpdev* p, const char* ext)
{
	if (!p->eprops)
		__vkpdev_load_eprops (p);
	for (guint i = 0; i < p->eprops->len; i++) {
		VkExtensionProperties* iter = &g_array_index (p->eprops, VkExtensionProperties, i);
		int cmpres = strncmp (iter->extensionName, ext, VK_MAX_EXTENSION_NAME_SIZE);
		if (cmpres == 0) {
			return TRUE;
		}
	}
	return FALSE;
}