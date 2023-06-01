#ifndef __H_SRC_MAIN_INCLUDE_VK_INSTANCE_H
#define __H_SRC_MAIN_INCLUDE_VK_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vk/vlayers.h>
#include <ztarray.h>

struct vkinstance {
	VkInstance core;
};

static inline VkInstance vkinstance_core (struct vkinstance* p)
{
	return p->core;
}

struct vkmessenger;

#ifdef DEBUG
#define init_vkinstance(p, info, cbs, exts, nexts, vlayers, nvlayers, msgr)	\
	__init_vkinstance(p, info, cbs, exts, nexts, vlayers, nvlayers, msgr)	
#else
#define init_vkinstance(p, info, cbs, exts, nexts, vlayers, nvlayers, msgr)	\
	__init_vkinstance(p, info, cbs, exts, nexts, NULL, 0, NULL)	
#endif

VkResult __init_vkinstance (struct vkinstance* p,
			    const VkApplicationInfo* info,
			    const VkAllocationCallbacks* cbs,
			    const char * const * enable_exts,
			    guint nexts,
			    const char * const * enable_vlayers,
			    guint nvlayers,
			    struct vkmessenger* msgr);

static inline void term_vkinstance (struct vkinstance* p)
{
	return vkDestroyInstance (vkinstance_core (p), NULL);
}

#endif /* __H_SRC_MAIN_INCLUDE_VK_INSTANCE_H */
