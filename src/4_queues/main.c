#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec4.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_validation_error_messages.h>

#include <glib.h>

#include <vk/vk.h>

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

int
main(void)
{
	struct vkapp* vkapp;
	GError*	      ecode = NULL;

	init_vkapp (&vkapp, &ecode);
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