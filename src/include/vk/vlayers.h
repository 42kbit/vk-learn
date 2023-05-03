#ifndef __H_SRC_INCLUDE_VK_VLAYERS_H
#define __H_SRC_INCLUDE_VK_VLAYERS_H

#include <vk/vk.h>

struct __cmp_VkLayerProperties_name {
	VkLayerProperties** dst;
	const char* search_for;
};

/* NULL terminated */
extern const char* vkapp_required_vlayers[];

gboolean
vkapp_matches_vlayers (struct vkapp* p,
		       const char**  vlneeded,
		       const char**  vlfailed_on);

#endif /* __H_SRC_INCLUDE_VK_VLAYERS_H */
