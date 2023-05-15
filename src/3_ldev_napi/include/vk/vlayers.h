#ifndef __H_SRC_INCLUDE_VK_VLAYERS_H
#define __H_SRC_INCLUDE_VK_VLAYERS_H

#include <vk/vk.h>
#include <string.h>

/* ->data is typeof struct vkvlayer */
GArray* vk_get_vlayers (void);

struct vkvlayer {
	VkLayerProperties props;
	/* May contain cached info if needed. */
};

/* NULL terminated */
extern const char* vkapp_required_vlayers[];

gboolean vkvlayers_matches_name (GArray* p,
			         const char**  vlneeded,
				 const char**  vlfailed_on);

#endif /* __H_SRC_INCLUDE_VK_VLAYERS_H */
