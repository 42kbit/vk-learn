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

/* VkInstance extensions, requred for this vulkan application. */
static const char * const vk_required_extensions[] = {
#ifdef DEBUG
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

static const char* vkapp_required_vlayers[] = {
#ifdef __VK_VLAYERS_NEEDED
	"VK_LAYER_KHRONOS_validation",
#endif
};

/* VkDevice extensions, required by this vulkan application. */
static const char * const vk_required_ldev_extensions [] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/* Gets GLFW extensions, adds to them extensions from params, then removes
 * duplicates.
 */
static int init_vkapp_exts (struct vkapp* p, GError** e)
{
	p->exts = vk_get_required_ext (vk_required_extensions,
				       G_N_ELEMENTS (vk_required_extensions));
	return 0;
}

/* Inits application relationship with GLFW library */
static int init_vkapp_glfw (struct vkapp* p, GError** e)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	return 0;
}

/* Creates GLFWWindow Object, that makes platform agnostic 
 * connection between application and a window.
*/
static int init_vkapp_glfw_window (struct vkapp* p, GError** e)
{
	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_RESIZABLE,  GLFW_FALSE);
	glfwWindowHint (GLFW_RESIZABLE,	 GLFW_TRUE);

	p->glfw_window = glfwCreateWindow (640, 480, "Hello Vulkan!", NULL, NULL);
	if (!p->glfw_window) {
		g_set_error (e, EVKDEFAULT, 1, "Failed to create GLFW window");
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(p->glfw_window);
	return 0;
}

/**/
static int init_vkapp_vlayers (struct vkapp* p, GError** e)
{
	if (!IS_DEFINED (__VK_VLAYERS_NEEDED)) {
		p->vlayers = NULL;
		return 0;
	}

	p->vlayers = vk_get_vlayers ();
	const char* failed_at = NULL;
	if (!vkvlayers_matches_name (p->vlayers,
				    vkapp_required_vlayers,
				    G_N_ELEMENTS (vkapp_required_vlayers),
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

	
	result = init_vkinstance (&p->instance,
				  &vk_app_info,
				  NULL,
				  (const char* const *)p->exts->data,
				  p->exts->len,
				  vkapp_required_vlayers,
				  G_N_ELEMENTS (vkapp_required_vlayers),
				  &p->messenger);

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
	GE_ERET(init_vkldev_from_vkpdev (&p->ld_used,
					 p->pd_used,
					 (int*)idxs->data,
					 prio,
					 idxs->len,
					 vk_required_ldev_extensions,
					 G_N_ELEMENTS (vk_required_ldev_extensions),
					 vkapp_required_vlayers,
					 G_N_ELEMENTS (vkapp_required_vlayers)));
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

	VkSubpassDependency subpass_dep = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};
	
	VkRenderPassCreateInfo render_pass_cinfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &subpass_dep
	};

	VkResult result;
	result = init_vkrender_pass (&p->render_pass, &p->ld_used, &render_pass_cinfo, NULL);
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
	
	result = init_vkpipeline_layout (&p->pipeline_layout, &p->ld_used, &pipeline_layout_cinfo, NULL);
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
	
	result = init_vkgraphics_pipeline (&p->pipeline, &p->ld_used, NULL, &pipeline_cinfo, NULL);
	
	if (result != VK_SUCCESS)
		ecode = -EINVAL;
free_exit:;
	term_vkshader_module (&vert);
	term_vkshader_module (&frag);
	return ecode;
}

static int init_vkapp_framebuffer (struct vkapp* p, GError** e)
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
			g_set_error (e, EVKDEFAULT, EINVAL, "Failed to init Vulkan framebuffer! VkResult: %d", result);
			return -EINVAL;
		}
	}
	return 0;
}

static int init_vkapp_cmdpool (struct vkapp* p, GError** e)
{
	VkCommandPoolCreateInfo cmdpool_cinfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = getval_vkq_gfamily (&p->pd_used->qfamily.gfamily)
	};
	VkResult result = VK_SUCCESS;
	result = init_vkcmdpool (&p->cmdpool, &p->ld_used, &cmdpool_cinfo, NULL);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed to init Vulkan command buffer: VkResult: %d", result);
		return -EINVAL;
	}
	return 0;
}

static int init_vkapp_cmdbuf (struct vkapp* p, GError** e)
{
	VkCommandBufferAllocateInfo cmdbuf_ainfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = p->cmdpool.core,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	VkResult result = init_vkcmdbuf (&p->cmdbuf, &p->ld_used, &cmdbuf_ainfo);
	if (result != VK_SUCCESS)
		return -EINVAL;

	return 0;
}

static int init_vkapp_sync (struct vkapp* p, GError** e)
{
	VkSemaphoreCreateInfo sem_create_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fnc_create_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};
	
	int ok = init_vksemaphore (&p->image_avail_bsem, &p->ld_used, &sem_create_info, NULL) == VK_SUCCESS &&
		 init_vksemaphore (&p->render_finished_bsem, &p->ld_used, &sem_create_info, NULL) == VK_SUCCESS &&
		 init_vkfence	  (&p->flight_fnc, &p->ld_used, &fnc_create_info, NULL) == VK_SUCCESS;
	
	if (!ok) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Failed to init Vulkan sync mechanisms!");
		return -EINVAL;
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
	GE_ERET (init_vkapp_cmdpool	 (p, e));
	GE_ERET (init_vkapp_cmdbuf	 (p, e));
	GE_ERET (init_vkapp_sync	 (p, e));
	return 0;
}

static inline void __term_vkmessenger_if_debug (struct vkapp* p)
{
#ifdef DEBUG
	VkResult result;
	result = term_vkmessenger (&p->messenger, &p->instance, NULL);
	g_assert (result == VK_SUCCESS);
#endif /* DEBUG */
}

void term_vkapp (struct vkapp* p, GError** e)
{
	VkResult result;
	
	__term_vkmessenger_if_debug (p);
	
	term_vksemaphore (&p->image_avail_bsem);
	term_vksemaphore (&p->render_finished_bsem);
	term_vkfence	 (&p->flight_fnc);
	
	term_vkcmdpool (&p->cmdpool);
	
	for (guint i = 0; i < p->framebuffers->len; i++) {
		vkDestroyFramebuffer (p->ld_used.core, g_array_index (p->framebuffers, VkFramebuffer, i), NULL);
	}
	g_array_free (p->framebuffers, TRUE);
	
	term_vkgraphics_pipeline (&p->pipeline);
	term_vkpipeline_layout (&p->pipeline_layout);

	term_vkrender_pass (&p->render_pass);

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

static int vkapp_write_to_cmdbuf (struct vkapp* p, GError** e, guint32 fb_idx)
{
	VkCommandBufferBeginInfo cmdbuf_binfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = NULL
	};
	VkResult result = vkBeginCommandBuffer (p->cmdbuf.core, &cmdbuf_binfo);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed vkBeginCommandBuffer, VkResult: %d", result);
		return -EINVAL;
	}
	
	VkClearValue clear_color = {
		.color = {
			.float32 = {
				0.f, 0.f, 0.f, 1.f
			}
		}
	};
	
	VkRenderPassBeginInfo render_pass_binfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = p->render_pass.core,
		.framebuffer = g_array_index (p->framebuffers, VkFramebuffer, fb_idx),
		.renderArea = {
			.offset = {0, 0},
			.extent = p->swapchain.res
		},
		.clearValueCount = 1,
		.pClearValues = &clear_color
	};
	vkCmdBeginRenderPass (p->cmdbuf.core, &render_pass_binfo, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline (p->cmdbuf.core, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipeline.core);

	VkViewport viewport = {
		.x = 0.f,
		.y = 0.f,
		.width = p->swapchain.res.width,
		.height = p->swapchain.res.height,
		.minDepth = 0.f,
		.maxDepth = 1.f
	};

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = p->swapchain.res
	};

	vkCmdSetViewport (p->cmdbuf.core, 0, 1, &viewport);
	vkCmdSetScissor  (p->cmdbuf.core, 0, 1, &scissor);
	
	vkCmdDraw (p->cmdbuf.core, 3, 1, 0, 0);

	vkCmdEndRenderPass (p->cmdbuf.core);
	
	result = vkEndCommandBuffer (p->cmdbuf.core);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed vkEndCommandBuffer, VkResult: %d", result);
		return -EINVAL;
	}

	return 0;
}

static int vkapp_draw_frame (struct vkapp* p, GError** e)
{
	guint32 image_idx;
	VkResult result;

	vkWaitForFences (p->ld_used.core, 1, &p->flight_fnc.core, VK_TRUE, UINT64_MAX);
	vkResetFences (p->ld_used.core, 1, &p->flight_fnc.core);
	result = vkAcquireNextImageKHR (p->ld_used.core, p->swapchain.core, UINT64_MAX,
					p->image_avail_bsem.core, NULL, &image_idx);
	g_assert (result == VK_SUCCESS); /* assert due to runtime */

	vkResetCommandBuffer (p->cmdbuf.core, 0);
	GE_ERET (vkapp_write_to_cmdbuf (p, e, image_idx));

	/* Commands recorded, submit them. */
	VkSemaphore wait_bsem[] = { p->image_avail_bsem.core };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkCommandBuffer cmdbufs[] = { p->cmdbuf.core };
	
	VkSemaphore ping_bsem[] = { p->render_finished_bsem.core };

	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = G_N_ELEMENTS (wait_bsem),
		.pWaitSemaphores = wait_bsem,
		.pWaitDstStageMask = wait_stages,
		.commandBufferCount = G_N_ELEMENTS (cmdbufs),
		.pCommandBuffers = cmdbufs,
		.signalSemaphoreCount = G_N_ELEMENTS (ping_bsem),
		.pSignalSemaphores = ping_bsem
	};
	
	result = vkQueueSubmit (p->gqueue.core, 1, &submit_info, p->flight_fnc.core);
	g_assert (result == VK_SUCCESS);
	
	VkSwapchainKHR swapchains [] = { p->swapchain.core };
	
	VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = ping_bsem,
		.swapchainCount = G_N_ELEMENTS (swapchains),
		.pSwapchains = swapchains,
		.pImageIndices = &image_idx,
		.pResults = NULL
	};

	vkQueuePresentKHR (p->pqueue.core, &present_info);

	return 0;
}

int vkapp_enter_mainloop (struct vkapp* p, GError** e) {
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(p->glfw_window))
	{
		/* Poll for and process events */
		glfwPollEvents();

		GE_ERET (vkapp_draw_frame (p, e));
	}
	vkDeviceWaitIdle (p->ld_used.core);

	return 0;
}