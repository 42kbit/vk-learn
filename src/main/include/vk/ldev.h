#ifndef __H_SRC_INCLUDE_VK_LDEV_H
#define __H_SRC_INCLUDE_VK_LDEV_H

#include <vk/pdev.h>

struct vkldev {
	VkDevice core;
};

int init_vkldev_from_vkpdev (struct vkldev* dst,
			     struct vkpdev* pdev,
			     int* 	    idxs, /* Array of indexes, identifying queues */
			     float*	    prios,/* Array of priorities, with respect to idxs array */
			     guint          nidxs /* Number of elements in both arrays (should be equal) */,
			     const char * const *   exts_enable,
			     guint nexts,
			     const char * const * vlayers,
			     guint nvlayers);

int term_vkldev (struct vkldev* p);

#endif /* __H_SRC_INCLUDE_VK_LDEV_H */
