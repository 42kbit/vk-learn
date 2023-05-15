#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_PDEV_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_PDEV_H

#include <vulkan/vulkan.h>
#include <glibext/glibext.h>
#include <vk/instance.h>

struct vkpdev {
	VkPhysicalDevice pdev;
	struct vkqfamily {
		GArray* props; /* VkQueueFamilyProperties */
		
		struct gfamily {
			int idx;
			gboolean opt;
		} gfamily;
	} qfamily;
};

G_GEN_OPT (struct gfamily, vkq_gfamily, idx, opt);

int get_vkpdevs_from_VkInstance (GArray**, VkInstance instance);
int get_vkpdevs_best (struct vkpdev**, GArray*);
void term_vkpdevs (GArray* vkpdevs);

static inline int get_vkpdevs_from_instance (GArray** dst, struct vkinstance* instance)
{
	return get_vkpdevs_from_VkInstance (dst, vkinstance_core (instance));
}

static inline int get_vkpdev_gfamily_idx (struct vkpdev* p)
{
	if (G_UNLIKELY(getopt_vkq_gfamily (&p->qfamily.gfamily) == FALSE))
		return -1;
	return getval_vkq_gfamily (&p->qfamily.gfamily);
}

#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_PDEV_H */
