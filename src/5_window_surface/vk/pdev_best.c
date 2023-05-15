#include <vk/vk.h>
#include <vk/pdev.h>

static inline gboolean
__is_suitable_VkPhysicalDevice (GArray*  arr,
			      	guint    idx,
			      	gpointer _udata)
{
	gboolean is_discrete;
	gboolean has_geometry_shader;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceFeatures   features;
	VCOPY (picked, struct vkpdev**, _udata);

	struct vkpdev* iter = &g_array_index (arr, struct vkpdev, idx);
	
	vkGetPhysicalDeviceProperties (iter->pdev, &props);
	vkGetPhysicalDeviceFeatures   (iter->pdev, &features);
	
	is_discrete = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	has_geometry_shader = features.geometryShader;
	
	if (is_discrete && has_geometry_shader) {
		*picked = iter;
		return FALSE;
	}
	
	return TRUE;
}

int get_vkpdevs_best (struct vkpdev** dst, GArray* p) {
	g_array_traverse (p, __is_suitable_VkPhysicalDevice, dst);
	return 0;
}