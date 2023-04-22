#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec4.h>

#include <vulkan/vulkan.h>

#include <glib.h>

int main(void)
{
	GLFWwindow* window;
	
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint (GLFW_RESIZABLE,  GLFW_FALSE);

	window = glfwCreateWindow(640, 480, "Hello Vulkan!", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Test glm */
	mat4 mat = GLM_MAT4_IDENTITY_INIT;
	vec4 vec = GLM_VEC4_ZERO_INIT;
	glm_mat4_mulv (mat, vec, vec);

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

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	vkDestroyInstance (vk_instance, NULL);
exit:
	glfwTerminate();
	return 0;
}
