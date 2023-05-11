#ifndef __H_SRC_INCLUDE_VK_PDEV_H
#define __H_SRC_INCLUDE_VK_PDEV_H

#include <vulkan/vulkan.h>
#include <glibext/glibext.h>

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

int get_vkpdevs_from_instance (GArray**, VkInstance instance);
int get_vkpdevs_best (struct vkpdev**, GArray*);
void term_vkpdevs (GArray* vkpdevs);

#endif /* __H_SRC_INCLUDE_VK_PDEV_H */
