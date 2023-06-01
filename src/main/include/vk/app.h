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
#include <vk/graphics_pipeline.h>

#include <vk/cmdpool.h>
#include <vk/cmdbuf.h>

#include <vk/semaphore.h>
#include <vk/fence.h>

#include <vk/framebuffer.h>

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
	
	struct vkrender_pass     	render_pass;
	struct vkpipeline_layout 	pipeline_layout;
	struct vkgraphics_pipeline	pipeline;
	
	GArray* 		framebuffers; /* struct vkframebuffer */
	
	struct vkcmdpool	cmdpool;
	struct vkcmdbuf		cmdbuf;
	
	struct vksemaphore	image_avail_bsem;
	struct vksemaphore	render_finished_bsem;
	struct vkfence		flight_fnc;
	

#ifdef __VK_VLAYERS_NEEDED
	GArray*			vlayers;  /* struct vkvlayer* */
#endif  
#ifdef DEBUG
	struct vkmessenger	messenger;
#endif
};

int init_vkapp (struct vkapp** dst, GError** e);
    
void term_vkapp (struct vkapp* p, GError** e);

int vkapp_enter_mainloop (struct vkapp* p, GError** e);

#endif /* __H_SRC_MAIN_INCLUDE_VK_APP_H */
