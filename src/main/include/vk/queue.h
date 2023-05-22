#ifndef __H_SRC_MAIN_INCLUDE_VK_QUEUE_H
#define __H_SRC_MAIN_INCLUDE_VK_QUEUE_H

#include <vk/ldev.h>

/* Like a thread pool but for GPU? */
struct vkqueue {
	VkQueue queue;
};

static inline int get_vkqueue_from_vkldev (struct vkqueue* dst,
			     		   struct vkldev*  ldev,
			     		   int queue_family_idx,
			     		   int queue_idx)
{
	vkGetDeviceQueue (ldev->core, queue_family_idx, queue_idx, &dst->queue);
	return 0;
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_QUEUE_H */
