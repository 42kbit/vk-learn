#include <vk/vk.h>
#include <ztarray.h>

static struct vkapp __vkapp; /* Shall not be accessed directly */

static inline gboolean
is_suitable_VkPhysicalDevice (GArray*  arr,
			      guint    idx,
			      gpointer _udata)
{
	gboolean is_discrete;
	gboolean has_geometry_shader;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceFeatures   features;
	VCOPY (picked, VkPhysicalDevice*, _udata);

	VkPhysicalDevice* iter = &g_array_index (arr, VkPhysicalDevice, idx);
	
	vkGetPhysicalDeviceProperties (*iter, &props);
	vkGetPhysicalDeviceFeatures   (*iter, &features);
	
	is_discrete = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	has_geometry_shader = features.geometryShader;
	
	if (is_discrete && has_geometry_shader) {
		*picked = *iter;
		return FALSE;
	}
	
	return TRUE;
}

static inline gint
init_vkapp_instance (struct vkapp* p,
		     GError**	   e)
{
	VkResult result;

#ifdef __VK_VLAYERS_NEEDED
	const char* failed_at = NULL;
	if (!vkvlayers_matches_name (p->vlayers,
				    (const char**)vkapp_required_vlayers,
				    &failed_at))
	{
		g_set_error (e,
			     EVKDEFAULT,
			     ENODEV,
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
	
	result = vkCreateInstance (&vk_create_info, NULL, &p->instance);
	g_assert (result == VK_SUCCESS);

#ifdef DEBUG
	result = init_VkDebugUtilsMessengerEXT (&p->debug_messenger,
						p->instance,
						&dm_create_info,
						NULL);
	g_assert (result == VK_SUCCESS);
#endif

	return 0;
}


static inline gint
init_vkapp_physdev (struct vkapp* p,
		    GError** 	  e)
{
	guint all_devs = 0;

	p->physdevs = vk_get_physdevs (p->instance);
	if (!p->physdevs) {
		g_set_error (e, EVKDEFAULT, ENODEV, "Vulkan devices not found");
		return -ENODEV;
	}

	g_array_traverse (p->physdevs, is_suitable_VkPhysicalDevice, &p->pd_used);

	if (!p->pd_used) {
		g_set_error (e,
			     EVKDEFAULT,
			     ENODEV,
			     "Found %d Vulkan devices, but none are suitable",
			     all_devs);
		return -ENODEV;
	}
	return 0;
}


static inline gint
init_vkapp_queues (struct vkapp* p,
		   GError** 	 e)
{
	p->qprops_all = vk_get_queue_family_props (p->pd_used);
	for (guint i = 0; i < p->qprops_all->len; i++) {
		VkQueueFamilyProperties* iter;
		iter = &g_array_index (p->qprops_all, VkQueueFamilyProperties, i);
		if (iter->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			g_optional_set (&p->gfamily_idx.opt, vkq_gfamily_set_cb, &i);
			break;
		}
	}
}

gint
init_vkapp (struct vkapp** dst,
	    GError**	   e)
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
	g_optional_init (&p->gfamily_idx.opt);
	ERET (init_vkapp_instance (p, e));
	ERET (init_vkapp_physdev  (p, e));
	ERET (init_vkapp_queues   (p, e));
	return 0;
}

void
term_vkapp (struct vkapp* p, GError** e)
{
	VkResult result;
#ifdef DEBUG
	result = term_VkDebugUtilsMessengerEXT (p->debug_messenger, p->instance, NULL);
	g_assert (result == VK_SUCCESS);
#endif
#ifdef __VK_VLAYERS_NEEDED
	g_array_free (p->vlayers, TRUE);
#endif
	g_array_free (p->physdevs, TRUE);
	g_array_free (p->qprops_all, TRUE);
	vkDestroyInstance (p->instance, NULL);
	glfwDestroyWindow (p->glfw_window);
	glfwTerminate();
}

