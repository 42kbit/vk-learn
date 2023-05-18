#ifndef __H_SRC_MAIN_INCLUDE_VK_PDEV_H
#define __H_SRC_MAIN_INCLUDE_VK_PDEV_H

#include <vulkan/vulkan.h>
#include <glibext/glibext.h>
#include <vk/instance.h>
#include <vk/surface.h>

struct vkpdev {
	VkPhysicalDevice pdev;
	struct vkqfamily {
		GArray* props; /* VkQueueFamilyProperties */
		
		struct gfamily {
			int idx;
			gboolean opt;
		} gfamily;
		
		struct pfamily {
			int idx;
			gboolean opt;
		} pfamily;
	} qfamily;
};

GE_GEN_OPT (struct gfamily, vkq_gfamily, idx, opt);
GE_GEN_OPT (struct pfamily, vkq_pfamily, idx, opt);

int get_vkpdevs_best (struct vkpdev**, GArray*);
int init_vkpdevs (GArray** dst, struct vkinstance* instance,
		  struct vksurface_khr* surface);
void term_vkpdevs (GArray* vkpdevs);

static inline VkPhysicalDevice vkpdev_core (struct vkpdev* p)
{
	return p->pdev;
}

#define __gen_vkpdev_family_idx_getter(fp)						\
static inline int get_vkpdev_## fp ##family_idx (struct vkpdev* p)			\
{											\
	if (G_UNLIKELY(getopt_vkq_##fp##family (&p->qfamily.fp##family) == FALSE))	\
		return -1;								\
	return getval_vkq_##fp##family (&p->qfamily.fp##family);			\
}

__gen_vkpdev_family_idx_getter (g);
__gen_vkpdev_family_idx_getter (p);

#endif /* __H_SRC_MAIN_INCLUDE_VK_PDEV_H */
