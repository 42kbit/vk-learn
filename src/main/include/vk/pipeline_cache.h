#ifndef __H_SRC_MAIN_INCLUDE_VK_PIPELINE_CACHE_H
#define __H_SRC_MAIN_INCLUDE_VK_PIPELINE_CACHE_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>
#include <vk/defs.h>

struct vkpipeline_cache {
	VkPipelineCache core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

VKEXT_GEN_LDEV_WRAPPER_HEADER_FUNCS (vkpipeline_cache, PipelineCache);

#endif /* __H_SRC_MAIN_INCLUDE_VK_PIPELINE_CACHE_H */
