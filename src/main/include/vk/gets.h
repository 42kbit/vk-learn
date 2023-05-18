#ifndef __H_SRC_MAIN_INCLUDE_VK_GETS_H
#define __H_SRC_MAIN_INCLUDE_VK_GETS_H

#include <glib.h>
#include <vulkan/vulkan.h>
	
GArray* vk_get_required_ext (const char * const * exts, guint nexts);

GArray* vk_get_queue_family_props (VkPhysicalDevice physdev);

#endif /* __H_SRC_MAIN_INCLUDE_VK_GETS_H */
