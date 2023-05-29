#include <vulkan/vulkan.h>

#include <vk/pdev.h>
#include <vk/ldev.h>
#include <vk/vlayers.h>
#include <vk/app.h>
#include <vk/instance.h>
#include <vk/surface.h>
#include <vk/messenger.h>
#include <vk/defs.h>
#include <vk/gets.h>

#include <ztarray.h>
#include <isdefined.h>

static struct vkapp __vkapp; /* Shall not be accessed directly */

static const char * const vk_required_extensions[] = {
#ifdef DEBUG
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

static const char * const vk_required_ldev_extensions [] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static int init_vkapp_exts (struct vkapp* p, GError** e)
{
	p->exts = vk_get_required_ext (vk_required_extensions,
				       G_N_ELEMENTS (vk_required_extensions));
	return 0;
}

static int init_vkapp_glfw (struct vkapp* p, GError** e)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	return 0;
}

static int init_vkapp_glfw_window (struct vkapp* p, GError** e)
{
	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_RESIZABLE,  GLFW_FALSE);

	p->glfw_window = glfwCreateWindow (640, 480, "Hello Vulkan!", NULL, NULL);
	if (!p->glfw_window) {
		g_set_error (e, EVKDEFAULT, 1, "Failed to create GLFW window");
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(p->glfw_window);
	return 0;
}

static int init_vkapp_vlayers (struct vkapp* p, GError** e)
{
	if (!IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		p->vlayers = NULL;
		return 0;
	}

	p->vlayers = vk_get_vlayers ();
	const char* failed_at = NULL;
	if (!vkvlayers_matches_name (p->vlayers,
				    (const char**)vkapp_required_vlayers,
				    &failed_at))
	{
		g_set_error (e, EVKDEFAULT, ENODEV,
			     "Missing validation layers. Failed at: %s\n",
			     failed_at);
		return -ENODEV;
	}
	return 0;
}

static int init_vkapp_instance (struct vkapp* p, GError** e)
{
	VkResult result;
	VkApplicationInfo vk_app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello, Triangle!",
		.applicationVersion = VK_MAKE_VERSION (1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION (1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	
#ifdef DEBUG
	result = init_vkinstance (&p->instance, &vk_app_info, NULL, p->exts, &p->messenger);
#else  /* DEBUG */
	result = init_vkinstance (&p->instance, &vk_app_info, NULL, p->exts, NULL);
#endif /* DEBUG */
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed to create vulkan insatnce, VkResult: %d", result);
		return -1;
	}
	return 0;
}

static int vkapp_pick_suitable_pdev (struct vkapp* p)
{
	for (guint i = 0; i < p->pdevs->len; i++) {
		struct vkpdev* iter = &g_array_index (p->pdevs, struct vkpdev, i);
		int ok = vkpdev_has_exts (iter, vk_required_ldev_extensions,
					  G_N_ELEMENTS (vk_required_ldev_extensions))
			&& vkpdev_has_graphics (iter)
			&& vkpdev_has_presentaion (iter)
			&& vkpdev_has_swapchain_support (iter, &p->surface);
		if (ok) {
			p->pd_used = iter;
			return 0;
		}
	}
	return -ENODEV;
}

static int init_vkapp_pdevs (struct vkapp* p, GError** e)
{
	int retcode;

	retcode = init_vkpdevs (&p->pdevs, &p->instance, &p->surface);
	if (retcode < 0) {
		g_set_error (e, EVKDEFAULT, ENODEV, "Vulkan devices not found");
		return -ENODEV;
	}

	retcode = vkapp_pick_suitable_pdev (p);
	if (retcode < 0) {
		g_set_error (e, EVKDEFAULT, ENODEV,
			     "Found %d Vulkan devices, but none are suitable",
			     p->pdevs->len);
		return -ENODEV;
	}
	return 0;
}

static inline int init_vkapp_ldevs (struct vkapp* p, GError** e)
{
	GArray* idxs = g_array_sized_new (FALSE, FALSE, sizeof (int), 8);
	g_array_insert_val (idxs, 0, p->pd_used->qfamily.gfamily.idx);
	g_array_insert_val (idxs, 1, p->pd_used->qfamily.pfamily.idx);

	/* May be duplicates, so remove them. */
	ge_array_traverse  (idxs, ge_atcb_remove_dups, NULL); 

	float prio[8] = { 1.f };
	GE_ERET(init_vkldev_from_vkpdev (&p->ld_used, p->pd_used,
					 (int*)idxs->data, prio, idxs->len,
					 vk_required_ldev_extensions,
					 G_N_ELEMENTS (vk_required_ldev_extensions)));
	g_array_free (idxs, TRUE);
	return 0;
}

static int __init_vkapp_queues_graphics (struct vkapp* p, GError** e)
{
	int gidx = get_vkpdev_gfamily_idx (p->pd_used);

	if (G_UNLIKELY (gidx < 0)) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Graphics queue not found!");
		return -EINVAL;
	}
	
	GE_ERET (get_vkqueue_from_vkldev (&p->gqueue, &p->ld_used, gidx, 0));
	return 0;
}

static int __init_vkapp_queues_presentation (struct vkapp* p, GError** e)
{
	int gidx = get_vkpdev_pfamily_idx (p->pd_used);
	
	if (G_UNLIKELY (gidx < 0)) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Presentation queue not found!");
		return -EINVAL;
	}
	
	GE_ERET (get_vkqueue_from_vkldev (&p->pqueue, &p->ld_used, gidx, 0));
	return 0;
}

static int init_vkapp_queues (struct vkapp* p, GError** e)
{
	GE_ERET (__init_vkapp_queues_graphics (p, e));
	GE_ERET (__init_vkapp_queues_presentation (p, e));
	return 0;
}

static int init_vkapp_surface (struct vkapp* p, GError** e)
{
	VkResult result;
	result = init_vksurface_khr (&p->surface, &p->instance,
				     p->glfw_window, NULL);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Failed to init Vulkan Surface, VkResult: %d", result);
		return -1;
	}
	return 0;
}

static int init_vkapp_swapchain (struct vkapp* p, GError** e)
{
	int retcode;
	retcode = init_vkswapchain_khr (&p->swapchain, p->pd_used, &p->ld_used, &p->surface);
	if (retcode < 0) {
		g_set_error (e, EVKDEFAULT, ENODEV, "Failed to init Vulkan swapchain, retcode: %d", retcode);
		return -1;
	}
	return 0;
}

static int init_vkapp_render_pass (struct vkapp* p, GError** e)
{
	VkAttachmentDescription color_attachment = {
		.format  = p->swapchain.sfmt.format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	

	/* layout (location = 0) out vec4 out_color; */
	VkAttachmentReference color_attachment_ref = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	
	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_ref
	};
	
	VkRenderPassCreateInfo render_pass_cinfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass
	};

	VkResult result;
	result = vkCreateRenderPass (p->ld_used.core, &render_pass_cinfo, NULL, &p->render_pass.core);
	if (result != VK_SUCCESS)
		return -EINVAL;

	return 0;
}

static int init_vkapp_graphics_pipeline (struct vkapp* p, GError** e)
{
	struct vkshader_module vert, frag;
	VkResult result;
	int ecode = 0;

	init_vkshader_module_from_file (&vert, &p->ld_used, "shaders/basic_vert.spv");
	init_vkshader_module_from_file (&frag, &p->ld_used, "shaders/basic_frag.spv");
	
	VkPipelineShaderStageCreateInfo vert_stage_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vert.core,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo frag_stage_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = frag.core,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo shader_stage_cinfos[] = {
		vert_stage_create_info,
		frag_stage_create_info
	};
	
	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	
	VkPipelineDynamicStateCreateInfo dynamic_state_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pDynamicStates = dynamic_states,
		.dynamicStateCount = G_N_ELEMENTS (dynamic_states)
	};

	VkPipelineVertexInputStateCreateInfo vertex_input_state_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = NULL,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = NULL
	};
	
	VkPipelineInputAssemblyStateCreateInfo input_assembly_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
	
	VkViewport viewport = {
		.x = 0.f,
		.y = 0.f,
		.width  = p->swapchain.res.width,
		.height = p->swapchain.res.height,
		.minDepth = 0.f,
		.maxDepth = 1.f
	};
	
	VkRect2D scissor = {
		.offset = {
			.x = 0,
			.y = 0
		},
		.extent = p->swapchain.res
	};
	
	VkPipelineViewportStateCreateInfo viewport_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};
	
	VkPipelineRasterizationStateCreateInfo raster_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.f,
		.depthBiasClamp = 0.f,
		.depthBiasSlopeFactor = 0.f
	};
	
	VkPipelineMultisampleStateCreateInfo multisample_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};
	
	/* Here lies VkPipelineDepthStencilStateCreateInfo */
	
	VkPipelineColorBlendAttachmentState color_blend_attachment_cinfo = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
				  VK_COLOR_COMPONENT_G_BIT |
				  VK_COLOR_COMPONENT_B_BIT |
				  VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD
	};
	
	VkPipelineColorBlendStateCreateInfo color_blend_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment_cinfo,
		.blendConstants [0] = 0.f,
		.blendConstants [1] = 0.f,
		.blendConstants [2] = 0.f,
		.blendConstants [3] = 0.f
	};
	
	VkPipelineLayout pipeline_layout;
	VkPipelineLayoutCreateInfo pipeline_layout_cinfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = NULL,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	};
	
	result = vkCreatePipelineLayout (p->ld_used.core, &pipeline_layout_cinfo, NULL, &p->pipeline_layout.core);
	if (result != VK_SUCCESS) {
		ecode = -EINVAL;
		goto free_exit;
	}
	
	VkGraphicsPipelineCreateInfo pipeline_cinfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = G_N_ELEMENTS (shader_stage_cinfos),
		.pStages = shader_stage_cinfos,
		.pVertexInputState = &vertex_input_state_cinfo,
		.pInputAssemblyState = &input_assembly_cinfo,
		.pViewportState = &viewport_cinfo,
		.pRasterizationState = &raster_cinfo,
		.pMultisampleState = &multisample_cinfo,
		.pDepthStencilState = NULL,
		.pColorBlendState = &color_blend_cinfo,
		.pDynamicState = &dynamic_state_cinfo,
		.layout = p->pipeline_layout.core,
		.renderPass = p->render_pass.core,
		.subpass = 0,
		.basePipelineHandle = NULL,
		.basePipelineIndex  = -1
	};
	
	result = vkCreateGraphicsPipelines (p->ld_used.core, NULL, 1, &pipeline_cinfo,
					    NULL, &p->pipeline.core);
	if (result != VK_SUCCESS)
		ecode = -EINVAL;
free_exit:;
	term_vkshader_module (&vert);
	term_vkshader_module (&frag);
	return ecode;
}

int init_vkapp_framebuffer (struct vkapp* p, GError** e)
{
	p->framebuffers = g_array_sized_new (FALSE, FALSE, sizeof (VkFramebuffer),
					     p->swapchain.image_views->len);
	g_array_set_size (p->framebuffers, p->swapchain.image_views->len);
	for (guint i = 0; i < p->swapchain.image_views->len; i++) {
		VkFramebufferCreateInfo framebuffer_cinfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = p->render_pass.core,
			.attachmentCount = 1,
			.pAttachments = &g_array_index(p->swapchain.image_views, VkImageView, i),
			.width  = p->swapchain.res.width,
			.height = p->swapchain.res.height,
			.layers = 1
		};
		VkResult result;
		result = vkCreateFramebuffer (p->ld_used.core, &framebuffer_cinfo,
					      NULL, &g_array_index (p->framebuffers, VkFramebuffer, i));
		if (result != VK_SUCCESS) {
			g_error_set (e, EVKDEFAULT, EINVAL, "Failed to init framebuffer! VkResult: %d", result);
			return -EINVAL;
		}
	}
	return 0;
}

int init_vkapp (struct vkapp** dst, GError** e)
{
	struct vkapp* p = &__vkapp;
	*dst = p;
	GE_ZEROTYPE (p);

	GE_ERET (init_vkapp_glfw	 (p, e));
	GE_ERET (init_vkapp_glfw_window  (p, e));
	GE_ERET (init_vkapp_exts	 (p, e));
	GE_ERET (init_vkapp_vlayers  	 (p, e));
	GE_ERET (init_vkapp_instance 	 (p, e));
	GE_ERET (init_vkapp_surface  	 (p, e));
	GE_ERET (init_vkapp_pdevs    	 (p, e));
	GE_ERET (init_vkapp_ldevs    	 (p, e));
	GE_ERET (init_vkapp_queues   	 (p, e));
	GE_ERET (init_vkapp_swapchain	 (p, e));
	GE_ERET (init_vkapp_render_pass	 (p, e));
	GE_ERET (init_vkapp_graphics_pipeline  (p, e));
	GE_ERET (init_vkapp_framebuffer  (p, e));
	return 0;
}

static inline void __term_vkmessenger_if_debug (struct vkapp* p)
{
#ifdef DEBUG
	VkResult result;
	result = term_vkmessenger (&p->messenger, NULL);
	g_assert (result == VK_SUCCESS);
#endif /* DEBUG */
}

void term_vkapp (struct vkapp* p, GError** e)
{
	VkResult result;
	
	__term_vkmessenger_if_debug (p);
	
	for (guint i = 0; i < p->framebuffers->len; i++) {
		vkDestroyFramebuffer (p->ld_used.core, g_array_index (p->framebuffers, VkFramebuffer, i), NULL);
	}
	g_array_free (p->framebuffers, TRUE);
	
	vkDestroyPipeline (p->ld_used.core, p->pipeline.core, NULL);
	vkDestroyPipelineLayout (p->ld_used.core, p->pipeline_layout.core, NULL);
	vkDestroyRenderPass (p->ld_used.core, p->render_pass.core, NULL);

	if (IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		g_array_free (p->vlayers, TRUE);
	}
	term_vkswapchain_khr (&p->swapchain);
	term_vkldev  (&p->ld_used);
	term_vkpdevs (p->pdevs);
	term_vksurface_khr (&p->surface);

	term_vkinstance (&p->instance);
	glfwDestroyWindow (p->glfw_window);
	glfwTerminate();
}