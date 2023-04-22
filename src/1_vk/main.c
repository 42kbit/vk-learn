#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec4.h>

#include <vulkan/vulkan.h>

#include <glib.h>

#include <vk-learn/common.h>

static inline GArray*
get_vlayers (void)
{
	GArray* vlayers;
	uint32_t vlayers_cnt;
	/* Assume vkEnumerateInstanceLayerProperties does throw VK_SUCCESS */
	vkEnumerateInstanceLayerProperties (&vlayers_cnt, NULL);
	vlayers = g_array_sized_new (FALSE, FALSE, sizeof (VkLayerProperties), vlayers_cnt);
	vkEnumerateInstanceLayerProperties (&vlayers_cnt, (VkLayerProperties*)vlayers->data);
	g_array_set_size (vlayers, vlayers_cnt);
	return vlayers;
}

struct app {
	GLFWwindow *glfw_window;
	GArray *vlayers;
};

static inline void
init_app (struct app* p)
{
	p->vlayers = get_vlayers ();
}

static inline void
term_app (struct app* p)
{
	g_array_free (p->vlayers, TRUE);
}

int
main(void)
{
	GLFWwindow* window;
	struct app app;

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
	glfwMakeContextCurrent (window);

	uint32_t ext_count;
	vkEnumerateInstanceExtensionProperties (NULL, &ext_count, NULL);
	printf ("Extensions count: %d\n", ext_count);

	VkApplicationInfo vk_appinfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello, Triangle!",
		.applicationVersion = VK_MAKE_VERSION (1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION (1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	uint32_t glfw_extensions_count = 0;
	const char** glfw_extensions = NULL;
	glfw_extensions = glfwGetRequiredInstanceExtensions (&glfw_extensions_count);
	printf ("Listing %d glfw extensions: \n", glfw_extensions_count);
	for (uint32_t i = 0; i < glfw_extensions_count; i++) {
		printf ("[%2d] %s\n", i, glfw_extensions[i]);
	}

	VkInstanceCreateInfo vk_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &vk_appinfo,
		.enabledExtensionCount = glfw_extensions_count,
		.ppEnabledExtensionNames = glfw_extensions,
		.enabledLayerCount = 0
	};

	VkInstance vk_instance;
	VkResult result = vkCreateInstance (&vk_create_info, NULL, &vk_instance);
	if (result != VK_SUCCESS) {
		perror ("vkCreateInstance failed\n");
		goto exit;
	}
	init_app (&app);
	
	printf ("Listing avaliable %d layers:\n", app.vlayers->len);
	for (guint i = 0; i < app.vlayers->len; i++) {
		VkLayerProperties* layer = &g_array_index (app.vlayers, VkLayerProperties, i);
		printf ("[%2d] %s\n", i, layer->layerName);
	}
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose (window))
	{
		/* Swap front and back buffers */
		glfwSwapBuffers (window);

		/* Poll for and process events */
		glfwPollEvents ();
	}
	
	term_app (&app);
	vkDestroyInstance (vk_instance, NULL);
exit:
	glfwTerminate ();
	return 0;
}
