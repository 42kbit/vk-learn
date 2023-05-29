#include <vk/shader_module.h>
#include <glib.h>
#include <vk/ldev.h>

static int __vkshader_module_load_core (struct vkshader_module* dst,
					struct vkldev* ldev,
					char * code, gsize len)
{
	VkResult result;
	VkShaderModuleCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = len,
		.pCode = (uint32_t*)code,
	};
	
	result = vkCreateShaderModule (ldev->core, &create_info, NULL, &dst->core);
	if (result != VK_SUCCESS)
		return -EINVAL;

	return 0;
}

int init_vkshader_module_from_file (struct vkshader_module* dst,
				    struct vkldev* ldev,
				    const char* filename)
{
	gchar* code;
	gsize  code_len;
	gboolean ok;
	int ecode = 0;
	
	dst->ldev = ldev;

	ok = g_file_get_contents (filename, &code,
				  &code_len, NULL);
	if (!ok)
		return -EINVAL;
	
	if (__vkshader_module_load_core (dst, ldev, code, code_len) < 0)
		ecode = -EINVAL;
	
	g_free (code);
	return ecode;
}

int term_vkshader_module (struct vkshader_module* dst)
{
	vkDestroyShaderModule (dst->ldev->core, dst->core, NULL);
	return 0;
}