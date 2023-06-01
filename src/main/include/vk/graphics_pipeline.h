#ifndef __H_SRC_MAIN_INCLUDE_VK_GRAPHICS_PIPELINE_H
#define __H_SRC_MAIN_INCLUDE_VK_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>
#include <vk/pipeline_cache.h>

#include <vk/defs.h>

struct vkgraphics_pipeline {
	VkPipeline core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

struct vkpipeline_cache;
static inline VkResult init_vkgraphics_pipeline (struct vkgraphics_pipeline* p,
						 struct vkldev* ldev,
						 struct vkpipeline_cache* _cache,
						 const VkGraphicsPipelineCreateInfo* cinfo,
						 const VkAllocationCallbacks *alloc_cbs)
{
	VkPipelineCache cache = VK_NULL_HANDLE;

	p->ldev = ldev;
	p->alloc_cbs = alloc_cbs;
	
	if (_cache != NULL)
		cache = _cache->core;

	return vkCreateGraphicsPipelines (ldev->core, cache, 1, cinfo, alloc_cbs, &p->core);
}

static inline VKEXT_GEN_LDEV_WRAPPER_TERM (vkgraphics_pipeline, Pipeline);

#endif /* __H_SRC_MAIN_INCLUDE_VK_GRAPHICS_PIPELINE_H */
