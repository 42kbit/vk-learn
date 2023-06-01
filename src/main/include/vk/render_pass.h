#ifndef __H_SRC_MAIN_INCLUDE_VK_RENDER_PASS_H
#define __H_SRC_MAIN_INCLUDE_VK_RENDER_PASS_H

#include <vulkan/vulkan.h>

#include <vk/ldev.h>
#include <vk/surface.h>

#include <vk/defs.h>

struct vkrender_pass {
	VkRenderPass core;
	
	struct vkldev* ldev;
	const VkAllocationCallbacks* alloc_cbs;
};

VKEXT_GEN_LDEV_WRAPPER_HEADER_FUNCS (vkrender_pass, RenderPass);

#endif /* __H_SRC_MAIN_INCLUDE_VK_RENDER_PASS_H */
