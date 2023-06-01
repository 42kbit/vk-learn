#ifndef __H_SRC_MAIN_INCLUDE_VK_PIPELINE_LAYOUT_H
#define __H_SRC_MAIN_INCLUDE_VK_PIPELINE_LAYOUT_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>
#include <vk/defs.h>

struct vkpipeline_layout {
	VkPipelineLayout core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

VKEXT_GEN_LDEV_WRAPPER_HEADER_FUNCS (vkpipeline_layout, PipelineLayout);

#endif /* __H_SRC_MAIN_INCLUDE_VK_PIPELINE_LAYOUT_H */
