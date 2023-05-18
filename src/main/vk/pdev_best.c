#include <glibext/glibext.h>
#include <vk/common.h>
#include <vk/pdev.h>
#include <string.h>

const char * const required_device_exts[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	NULL
};


static gboolean __has_needed_queues (struct vkpdev* p)
{
	return getopt_vkq_gfamily (&p->qfamily.gfamily) &&
	       getopt_vkq_pfamily (&p->qfamily.pfamily);
}

static gboolean __has_needed_exts (struct vkpdev* p)
{
	int cmpres = 0;
	for (guint i = 0; required_device_exts[i] != NULL; i++) {
		const char * iter = required_device_exts [i];
		if (!vkpdev_has_ext (p, iter))
			return FALSE;
	}
	return TRUE;
}

static gboolean __is_suitable_VkPhysicalDevice (GArray*  arr,
						guint    idx,
			      			gpointer _udata)
{
	VCOPY (picked, struct vkpdev**, _udata);

	struct vkpdev* iter = &g_array_index (arr, struct vkpdev, idx);
	
	gboolean ok = __has_needed_queues (iter) && 
		      __has_needed_exts   (iter);
	
	if (ok) {
		*picked = iter;
		return FALSE;
	}
	
	return TRUE;
}

int get_vkpdevs_best (struct vkpdev** dst, GArray* p) {
	ge_array_traverse (p, __is_suitable_VkPhysicalDevice, dst);
	return 0;
}