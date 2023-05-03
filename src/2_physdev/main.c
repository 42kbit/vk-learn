#ifdef DEBUG
 #define __VK_VLAYERS_NEEDED 1
#else
 #define G_DISABLE_ASSERT 1
#endif

#define EVKDEFAULT 42

#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec4.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_validation_error_messages.h>

#include <glib.h>

#include <vk/common.h>

/*
 * On error return.
 * If function (argument x) returns negative value (error)
 * return from current function with the same error code, else
 * continue execution.
 *
 * Usage:
 * ERET (func (a, b, c));
*/

#define ERET(x)					\
G_STMT_START {					\
	int __retval = 0;			\
	if ((__retval = x) < 0)			\
		return __retval;		\
} G_STMT_END

/* All info needed for rendeing Vulkan Application. */
struct vkapp {
	GLFWwindow*		 glfw_window;
	GArray*			 exts;
	VkInstance 		 instance;
	GArray*			 physdevs; /* All VkPhysicalDevices */
	VkPhysicalDevice	 pd_used;  /* Picked VkPhysicalDevice*/
#ifdef __VK_VLAYERS_NEEDED
	GArray*			 vlayers;
#endif
#ifdef DEBUG
	VkDebugUtilsMessengerEXT debug_messenger;
#endif
};
struct vkapp __vkapp; /* Shall not be accessed directly */

struct __cmp_VkLayerProperties_name {
	VkLayerProperties** dst;
	const char* search_for;
};

static inline gboolean
g_array_cmp_VkLayerProperties_name (GArray*  arr,
		     		    guint    idx,
		     		    gpointer _udata);

#ifdef __VK_VLAYERS_NEEDED
const char* vkapp_required_vlayers[] = {
	"VK_LAYER_KHRONOS_validation"
};
static inline gboolean
vkapp_matches_vlayers (struct vkapp* p,
		       const char**  vlneeded,
		       guint	     vlneeded_len,
		       const char**  vlfailed_on);

static inline gboolean
vkapp_matches_vlayers (struct vkapp* p,
		       const char**  vlneeded,
		       guint	     vlneeded_len,
		       const char**  vlfailed_on)
{
	VkLayerProperties* found;
	struct __cmp_VkLayerProperties_name args;

	guint i = 0;
	for (i = 0; i < vlneeded_len; i++) {
		found = NULL;
		args.dst = &found;
		args.search_for = vlneeded[i];
		g_array_traverse (p->vlayers, g_array_cmp_VkLayerProperties_name, &args);
		if (!found) {
			*vlfailed_on = args.search_for;
			return FALSE;
		}
	}
	return TRUE;
} 

static inline GArray*
vk_get_vlayers (void)
{
	VkResult result;
	GArray * vlayers;
	guint    vlayers_cnt;
	/* Assume vkEnumerateInstanceLayerProperties does throw VK_SUCCESS */
	result = vkEnumerateInstanceLayerProperties (&vlayers_cnt, NULL);
	g_assert (result == VK_SUCCESS);
	
	if (vlayers_cnt == 0)
		return NULL;

	vlayers = g_array_sized_new (FALSE, FALSE, sizeof (VkLayerProperties), vlayers_cnt);
	result = vkEnumerateInstanceLayerProperties (&vlayers_cnt, (VkLayerProperties*)vlayers->data);
	g_assert (result == VK_SUCCESS);

	g_array_set_size (vlayers, vlayers_cnt);
	return vlayers;
}
#endif


static inline GArray*
vk_get_physdevs (VkInstance instance)
{
	VkResult result;
	GArray*  physdevs;
	guint    physdevs_cnt;
	
	result = vkEnumeratePhysicalDevices (instance, &physdevs_cnt, NULL);
	g_assert (result == VK_SUCCESS);

	if (physdevs_cnt == 0)
		return NULL;

	physdevs = g_array_sized_new (FALSE, FALSE, sizeof (VkPhysicalDevice), physdevs_cnt);
	result = vkEnumeratePhysicalDevices (instance, &physdevs_cnt, (VkPhysicalDevice*)physdevs->data);
	g_assert (result == VK_SUCCESS);
	g_array_set_size (physdevs, physdevs_cnt);
	return physdevs;
}

#ifdef DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT      msg_severity,
		VkDebugUtilsMessageTypeFlagsEXT	       	    message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* _udata);


static inline const char *
getstr_VkDebugUtilsMessageSeverityFlagBitsEXT (VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity)
{
	switch (msg_severity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return "VRB";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    return "LOG";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return "WRN";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   return "ERR";
		default: return "INV"; /* Invalid */
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
vk_debug_callback (VkDebugUtilsMessageSeverityFlagBitsEXT      msg_severity,
		   VkDebugUtilsMessageTypeFlagsEXT	       msg_type,
		   const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		   void* _udata)
{
	const char* severity_str = getstr_VkDebugUtilsMessageSeverityFlagBitsEXT (msg_severity);
	
	g_printerr ("[%s] %s\n", severity_str, callback_data->pMessage);
	return msg_severity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
}

static inline VkResult
init_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT* p,
			       VkInstance		 instance,
			       const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			       const VkAllocationCallbacks*		 callbacks)
{
	PFN_vkVoidFunction _func;
	_func = vkGetInstanceProcAddr (instance, "vkCreateDebugUtilsMessengerEXT");
						  
	if (!_func)
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	VCOPY (func, PFN_vkCreateDebugUtilsMessengerEXT, _func);
	return func (instance, create_info, callbacks, p);
}

static inline void
term_VkDebugUtilsMessengerEXT (VkDebugUtilsMessengerEXT     p,
			       VkInstance		    instance,
			       const VkAllocationCallbacks* callbacks)
{
	PFN_vkVoidFunction _func;
	_func = vkGetInstanceProcAddr (instance, "vkDestroyDebugUtilsMessengerEXT");
	g_assert (_func != NULL);
	VCOPY (func, PFN_vkDestroyDebugUtilsMessengerEXT, _func);
	func (instance, p, callbacks);
}
#endif

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

static inline GArray*
vk_get_required_ext (void)
{
	GArray* p;
	const char** glfw_exts;
	const char * const debug_ext = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	guint glfw_n;
	guint nexts;

	glfw_exts = glfwGetRequiredInstanceExtensions (&glfw_n);
	nexts = glfw_n;

#ifdef DEBUG
	nexts += 1; /* VK_EXT_DEBUG_UTILS_EXTENSION_NAME */
#endif

	p = g_array_sized_new (FALSE, TRUE, sizeof (const char*), nexts);
	g_array_append_vals (p, glfw_exts, glfw_n);

#ifdef DEBUG
	g_array_append_val  (p, debug_ext);
#endif
	return p;
}


static inline gint
init_vkapp_instance (struct vkapp* p,
		     GError**	   e)
{
	VkResult result;

#ifdef __VK_VLAYERS_NEEDED
	const char* failed_at = NULL;
	if (!vkapp_matches_vlayers (p,
				    vkapp_required_vlayers,
				    G_N_ELEMENTS (vkapp_required_vlayers),
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
		.enabledLayerCount = G_N_ELEMENTS (vkapp_required_vlayers),
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
		    GError** e)
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
init_vkapp (struct vkapp* p,
	    GError**	  e)
{
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
	ERET (init_vkapp_physdev  (p, e));
	return 0;
}

static inline void
term_vkapp (struct vkapp* p, GError** e)
{
#ifdef DEBUG
	term_VkDebugUtilsMessengerEXT (p->debug_messenger, p->instance, NULL);
#endif
#ifdef __VK_VLAYERS_NEEDED
	g_array_free (p->vlayers, TRUE);
#endif
	g_array_free (p->physdevs, TRUE);
	vkDestroyInstance (p->instance, NULL);
	glfwDestroyWindow (p->glfw_window);
	glfwTerminate();
}

static inline void
vkapp_enter_mainloop (struct vkapp* p, GError** e) {
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(p->glfw_window))
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(p->glfw_window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}

static inline gboolean
g_array_cmp_VkLayerProperties_name (GArray*  arr,
		     		    guint    idx,
		     		    gpointer _udata)
{
	VCOPY (udata, struct __cmp_VkLayerProperties_name*, _udata);
	VkLayerProperties* current = &g_array_index (arr, VkLayerProperties, idx);

	int cmpres = strcmp (udata->search_for, current->layerName);
	if (cmpres == 0) {
		*udata->dst = current;
		return FALSE;
	}
	return TRUE;
}

int
main(void)
{
	struct vkapp* vkapp = &__vkapp;
	GError*	      ecode = NULL;

	init_vkapp (vkapp, &ecode);
	if (ecode) {
		g_printerr ("Vulkan init error.\n Error string: %s\n", ecode->message);
		g_error_free (ecode);
		ecode = NULL;
		goto app_term;
	}
	
	vkapp_enter_mainloop (vkapp, &ecode);
	if (ecode) {
		g_printerr ("Vulkan mainloop error.\n Error string: %s\n", ecode->message);
		g_error_free (ecode);
		ecode = NULL;
		goto app_term;
	}
	
app_term:;
	term_vkapp (vkapp, &ecode);
	if (ecode) {
		g_printerr ("Vulkan termination error.\n Error string: %s\n", ecode->message);
		g_error_free (ecode);
		ecode = NULL;
	}
	return 0;
}