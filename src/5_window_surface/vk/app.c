
#include <vk/pdev.h>
#include <vk/ldev.h>
#include <vk/vlayers.h>
#include <vk/app.h>
#include <vk/instance.h>
#include <vk/surface.h>

#include <ztarray.h>

static struct vkapp __vkapp; /* Shall not be accessed directly */

static int init_vkapp_instance (struct vkapp* p, GError**e)
{
	VkResult result;

#ifdef __VK_VLAYERS_NEEDED
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
#endif
	
#ifdef DEBUG
	VkDebugUtilsMessengerCreateInfoEXT dm_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
			       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = vk_debug_callback,
		.pUserData = NULL
	};
#endif

	VkApplicationInfo vk_app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello, Triangle!",
		.applicationVersion = VK_MAKE_VERSION (1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION (1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo vk_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &vk_app_info,
		.enabledExtensionCount = p->exts->len,
		.ppEnabledExtensionNames = (const char * const *)p->exts->data,
#ifndef __VK_VLAYERS_NEEDED
		.enabledLayerCount = 0,
#else
		.ppEnabledLayerNames = vkapp_required_vlayers,
		.enabledLayerCount = count_ztarray_len ((void**)vkapp_required_vlayers),
#endif
#ifdef DEBUG
		.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&dm_create_info
#else
		.pNext = NULL
#endif
	};
	
	result = init_vkinstance (&p->instance, &vk_create_info, NULL);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed to create vulkan insatnce, VkResult: %d", result);
		return -1;
	}

#ifdef DEBUG
	result = init_VkDebugUtilsMessengerEXT (&p->debug_messenger, vkinstance_core(&p->instance),
						&dm_create_info, NULL);
	if (result != VK_SUCCESS) {
		g_set_error (e, EVKDEFAULT, EINVAL,
			     "Failed to init debug messenger, VkResult: %d", result);
		return -1;
	}
#endif

	return 0;
}


static int init_vkapp_pdevs (struct vkapp* p, GError** e)
{
	guint all_devs = 0;
	int retcode;

	retcode = get_vkpdevs_from_instance (&p->pdevs, &p->instance);
	if (retcode < 0) {
		g_set_error (e, EVKDEFAULT, ENODEV, "Vulkan devices not found");
		return -ENODEV;
	}

	retcode = get_vkpdevs_best (&p->pd_used, p->pdevs);

	if (!p->pd_used) {
		g_set_error (e, EVKDEFAULT, ENODEV,
			     "Found %d Vulkan devices, but none are suitable",
			     all_devs);
		return -ENODEV;
	}
	return 0;
}

static inline int init_vkapp_ldevs (struct vkapp* p, GError** e)
{
	int   idxs[1] = { p->pd_used->qfamily.gfamily.idx };
	float prio[1] = { 1.f };
	ERET(init_vkldev_from_vkpdev (&p->ld_used, p->pd_used, idxs,
				      prio, G_N_ELEMENTS (idxs)));
	return 0;
}

static int init_vkapp_queues (struct vkapp* p, GError** e)
{
	int gidx = get_vkpdev_gfamily_idx (p->pd_used);

	if (G_UNLIKELY (gidx < 0)) {
		g_set_error (e, EVKDEFAULT, EINVAL, "Graphics queue not found!");
		return -EINVAL;
	}
	ERET (get_vkqueue_from_vkldev (&p->gqueue, &p->ld_used, gidx, 0));
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

int init_vkapp (struct vkapp** dst, GError** e)
{
	struct vkapp* p = &__vkapp;
	*dst = p;
	ZEROTYPE (p);
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_RESIZABLE,  GLFW_FALSE);

	p->glfw_window = glfwCreateWindow (640, 480, "Hello Vulkan!", NULL, NULL);
	if (!p->glfw_window) {
		g_set_error (e, EVKDEFAULT, 1, "Failed to create GLFW window");
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(p->glfw_window);
	p->exts = vk_get_required_ext ();
#ifdef __VK_VLAYERS_NEEDED
	p->vlayers = vk_get_vlayers ();
#endif
	ERET (init_vkapp_instance (p, e));
	ERET (init_vkapp_surface  (p, e));
	ERET (init_vkapp_pdevs    (p, e));
	ERET (init_vkapp_ldevs    (p, e));
	ERET (init_vkapp_queues   (p, e));
	return 0;
}

void term_vkapp (struct vkapp* p, GError** e)
{
	VkResult result;
#ifdef DEBUG
	result = term_VkDebugUtilsMessengerEXT (p->debug_messenger, vkinstance_core (&p->instance), NULL);
	g_assert (result == VK_SUCCESS);
#endif
#ifdef __VK_VLAYERS_NEEDED
	g_array_free (p->vlayers, TRUE);
#endif
	term_vkpdevs (p->pdevs);
	term_vkldev  (&p->ld_used);
	vkDestroySurfaceKHR (vkinstance_core (&p->instance), vksurface_khr_core (&p->surface), NULL);
	vkDestroyInstance (vkinstance_core (&p->instance), NULL);
	glfwDestroyWindow (p->glfw_window);
	glfwTerminate();
}