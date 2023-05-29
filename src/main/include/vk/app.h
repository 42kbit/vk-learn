#ifndef __H_SRC_MAIN_INCLUDE_VK_APP_H
#define __H_SRC_MAIN_INCLUDE_VK_APP_H

#include <GLFW/glfw3.h>
#include <vk/ldev.h>
#include <vk/queue.h>
#include <vk/instance.h>

#include <vk/surface.h>
#include <vk/messenger.h>

#include <vk/swapchain.h>
#include <vk/shader_module.h>
#include <vk/render_pass.h>
#include <vk/pipeline_layout.h>
#include <vk/pipeline.h>

/* All info needed for rendeing Vulkan Application. */
struct vkapp {
	GLFWwindow*		glfw_window;
	GArray*			exts;

	struct vkinstance	instance;

	GArray*			pdevs;    /* All struct vkpdev   */
	struct vkpdev*		pd_used;  /* Picked VkPhysicalDevice */

	struct vkldev		ld_used;
	
	struct vkqueue		gqueue;   /* Graphics queue */
	struct vkqueue		pqueue;   /* Presentation queue */
	
	struct vksurface_khr	surface;
	
	struct vkswapchain_khr	swapchain;
	
	struct vkrender_pass     render_pass;
	struct vkpipeline_layout pipeline_layout;
	struct vkpipeline	 pipeline;
	
	GArray* 		framebuffers;

#ifdef __VK_VLAYERS_NEEDED
	GArray*			vlayers;  /* struct vkvlayer* */
#endif  
#ifdef DEBUG
	struct vkmessenger	messenger;
#endif
};

gint init_vkapp (struct vkapp** dst, GError** e);
    
void term_vkapp (struct vkapp* p, GError** e);

#endif /* __H_SRC_MAIN_INCLUDE_VK_APP_H */
