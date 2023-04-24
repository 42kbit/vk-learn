#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec4.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_validation_error_messages.h>

#include <glib.h>

#include <vk-learn/common.h>

#ifdef DEBUG
 #define __VK_VLAYERS_NEEDED 1
#endif

struct vkapp __vkapp; /* Shall not be accessed directly */
const char* vkapp_required_vlayers[] = {
#ifdef __VK_VLAYERS_NEEDED
	"VK_LAYER_KHRONOS_validation"
#endif
};

static inline GArray*
get_vlayers (void)
{
	GArray *vlayers;
	guint vlayers_cnt;
	/* Assume vkEnumerateInstanceLayerProperties does throw VK_SUCCESS */
	vkEnumerateInstanceLayerProperties (&vlayers_cnt, NULL);
	vlayers = g_array_sized_new (FALSE, FALSE, sizeof (VkLayerProperties), vlayers_cnt);
	vkEnumerateInstanceLayerProperties (&vlayers_cnt, (VkLayerProperties*)vlayers->data);
	g_array_set_size (vlayers, vlayers_cnt);
	return vlayers;
}

/* All info needed for rendeing Vulkan Application. */
struct vkapp {
	GLFWwindow *glfw_window;
	GArray *vlayers;
};

static inline void
init_vkapp (struct vkapp* p,
	    GLFWwindow* glfw_window)
{
	p->vlayers = get_vlayers ();
	p->glfw_window = glfw_window;
}

static inline void
term_vkapp (struct vkapp* p)
{
	g_array_free (p->vlayers, TRUE);
	glfwDestroyWindow (p->glfw_window);
}

struct __cmp_VkLayerProperties_name {
	VkLayerProperties** dst;
	const char* search_for;
};

static inline bool
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

static inline bool
vkapp_matches_vlayers (struct vkapp* p,
		       const char**  vlneeded,
		       guint	     vlneeded_len,
		       const char**  vlfailed_on)
{
#ifdef __VK_VLAYERS_NEEDED
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
#endif
	return TRUE;
} 

int
main(void)
{
	GLFWwindow* window;
	struct vkapp* vkapp = &__vkapp;

	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_RESIZABLE,  GLFW_FALSE);

	window = glfwCreateWindow (640, 480, "Hello Vulkan!", NULL, NULL);
	if (window == NULL) {
		glfwTerminate ();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	init_vkapp (vkapp, window);
	
	g_print ("Listing avaliable %d layers:\n", vkapp->vlayers->len);
	for (guint i = 0; i < vkapp->vlayers->len; i++) {
		VkLayerProperties* layer = &g_array_index (vkapp->vlayers, VkLayerProperties, i);
		g_print ("[%2d] %s\n", i, layer->layerName);
	}

	const char* failed_at = NULL;
	if (!vkapp_matches_vlayers (vkapp,
				    vkapp_required_vlayers,
				    G_N_ELEMENTS (vkapp_required_vlayers),
				    &failed_at))
	{
		g_error ("Missing validation layers. Failed at: %s\n", failed_at);
		return -1;
	}

	uint32_t ext_count;
	vkEnumerateInstanceExtensionProperties (NULL, &ext_count, NULL);
	g_print ("Extensions count: %d\n", ext_count);
	
	uint32_t glfw_extensions_count = 0;
	const char** glfw_extensions = NULL;
	glfw_extensions = glfwGetRequiredInstanceExtensions (&glfw_extensions_count);
	g_print ("Listing %d glfw extensions: \n", glfw_extensions_count);
	for (uint32_t i = 0; i < glfw_extensions_count; i++) {
		g_print ("[%2d] %s\n", i, glfw_extensions[i]);
	}

	VkApplicationInfo vk_appinfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello, Triangle!",
		.applicationVersion = VK_MAKE_VERSION (1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION (1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	VkInstanceCreateInfo vk_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &vk_appinfo,
		.enabledExtensionCount = glfw_extensions_count,
		.ppEnabledExtensionNames = glfw_extensions,
#ifndef __VK_VLAYERS_NEEDED
		.enabledLayerCount = 0
#else
		.ppEnabledLayerNames = vkapp_required_vlayers,
		.enabledLayerCount = G_N_ELEMENTS (vkapp_required_vlayers)
#endif
	};

	VkInstance vk_instance;
	VkResult result = vkCreateInstance (&vk_create_info, NULL, &vk_instance);
	if (result != VK_SUCCESS) {
		g_error ("vkCreateInstance failed, VkResult: %d\n",
			 result);
		goto exit;
	}

	/* Loop until the user closes  the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Swap front a nd back buffers */
		glfwSwapBuffers(window);

		/* Poll for an d process events */
		glfwPollEvents();
	}
	
	term_vkapp (vkapp);
	vkDestroyInstance (vk_instance, NULL);
exit: 
	glfwTerminate();
	return 0;
}
