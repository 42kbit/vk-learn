#ifndef __H_SRC_MAIN_INCLUDE_VK_SHADER_MODULE_H
#define __H_SRC_MAIN_INCLUDE_VK_SHADER_MODULE_H

#include <vulkan/vulkan.h>

struct vkldev;

struct vkshader_module {
	VkShaderModule core;
	struct vkldev* ldev;
};

int init_vkshader_module_from_file (struct vkshader_module* dst,
				    struct vkldev* ldev,
				    const char* filename);

int term_vkshader_module (struct vkshader_module* dst);

#endif /* __H_SRC_MAIN_INCLUDE_VK_SHADER_MODULE_H */
