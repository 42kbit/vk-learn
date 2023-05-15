#ifndef __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_GETS_H
#define __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_GETS_H

#include <vk/vk.h>
	
GArray*
vk_get_required_ext (void);

GArray*
vk_get_queue_family_props (VkPhysicalDevice physdev);

#endif /* __H_SRC_5_WINDOW_SURFACE_INCLUDE_VK_GETS_H */
